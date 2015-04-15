// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_config.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/adevs_config.hh>

#include <efscape/impl/Cloneable.hh>
#include <efscape/impl/InitObject.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>
#include <efscape/impl/ModelFactory.ipp>

namespace efscape {

  namespace impl {

    char const gcp_libname[] = "efscapeimpl";

    /**
     * Helper function for injecting a bag events into a model.
     *
     * @param e time
     * @param xb bag of events
     * @param aCp_model handle to model
     */
    void inject_events(double e, const adevs::Bag<IO_Type>& xb,
		       DEVS* aCp_model)
    {
      ATOMIC* lCp_atomic = NULL;
      if ( (lCp_atomic = aCp_model->typeIsAtomic() ) != NULL ) {
	lCp_atomic->delta_ext(e, xb);
      }
      else {
	NETWORK* lCp_network = NULL;
	adevs::Set<DEVS*> components;
	lCp_network->getComponents(components);
	/*typename*/ adevs::Set<DEVS*>::iterator iter =
	  components.begin();
	for (; iter != components.end(); iter++) {
	  inject_events(e, xb, *iter);
	}
      }
    }

    /**
     * Helper function for extracting a bag events from a model.
     *
     * @param yb bag of events
     * @param aCp_model handle to model
     */
    void get_output(adevs::Bag<IO_Type>& xb,
		    DEVS* aCp_model)
    {
      if (aCp_model == NULL) {
	
      }
      ATOMIC* lCp_atomic = NULL;
      if ( (lCp_atomic = aCp_model->typeIsAtomic() ) != NULL ) {
	lCp_atomic->output_func(xb);
      }
      else {
	NETWORK* lCp_network = NULL;
	if ( (lCp_network = aCp_model->typeIsNetwork()) == NULL);
	     return;
	adevs::Set<DEVS*> components;
	lCp_network->getComponents(components);
	/*typename*/ adevs::Set<DEVS*>::iterator iter =
	  components.begin();
	for (; iter != components.end(); iter++) {
	  get_output(xb, *iter);
	}
      }
    }

    /**
     * Helper function for creating an adevs model from the model factory.
     *
     * @param acp_classname class name of model
     * @returns handle to model (null if missing from the factory)
     */
    DEVS* createModel( const char* acp_classname ) {
      return ( Singleton<ModelHomeI>::Instance().GetModelFactory().CreateModel(acp_classname) );
    }

    /**
     * Helper function for cloning an adevs model
     *
     * @param aCp_model handle to model
     * @returns handle to model clone (null if cloning fails)
     */
    DEVS* cloneModel( const DEVS* aCp_model ) {
      // first attempt to use the Cloneable interface if it is available
      const Cloneable* lCp_cloneable;
      if ( (lCp_cloneable = dynamic_cast<const Cloneable*>(aCp_model) ) )
	return ( dynamic_cast<DEVS*>(lCp_cloneable->clone()) );

      // next, if this is a digraph
      const DIGRAPH* lCp_digraph = 0;

      if ( ( lCp_digraph = dynamic_cast<const DIGRAPH*>(aCp_model) ) )
	return (DEVS*)cloneDigraph(lCp_digraph);

      return 0;
    }

    /**
     * Helper function for cloning an adevs digraph model
     *
     * @param aCp_model handle to digraph model
     * @returns handle to model clone (null if cloning fails)
     */
    DIGRAPH* cloneDigraph( const DIGRAPH* aCp_digraph ) {
      if ( aCp_digraph == 0)
	return 0;

      // first check if this is a derived (and cloneable) class of digraph
      DIGRAPH* lCp_digraph_clone = NULL;
      const Cloneable* lCp_cloneable = NULL;

      if ( ( lCp_cloneable = dynamic_cast<const Cloneable*>(aCp_digraph) ) ) {
	if ( ( lCp_digraph_clone = dynamic_cast<DIGRAPH*>(lCp_cloneable->clone()) ) )
	  ;
	else
	  return 0;
      }
      else
	lCp_digraph_clone = new DIGRAPH;

      // next, clone sub-components and couplings
      typedef adevs::Set<const DEVS* > ComponentSet;
      typedef std::map<const DEVS*,DEVS*> ModelCloneMap;

      ComponentSet lC_models;
      ComponentSet::iterator iModel;
      ModelCloneMap lC_model_mirror;

      // add digraph and its clone to map (needed for cloning coupling info)
      lC_model_mirror[aCp_digraph] = lCp_digraph_clone;

      // first clone each digraph component
      aCp_digraph->getComponents(lC_models);

      for (iModel = lC_models.begin(); iModel != lC_models.end(); iModel++) {

	DEVS* lCp_clone = 0;

	if ( ( lCp_cloneable = dynamic_cast<const Cloneable*>(*iModel) ) ) {
	  // attempt to clone component and add cloned component
	  if ( ( lCp_clone = dynamic_cast<DEVS*>(lCp_cloneable->clone()) ) ) {
	    lCp_digraph_clone->add(lCp_clone);
	    lC_model_mirror[*iModel] = lCp_clone;
	  }
	}

	if (!lCp_clone) {	// if cloning failed, abort
	  delete lCp_digraph_clone;
	  return 0;
	}
      }

      // next, clone the couplings
      typedef std::map<DIGRAPH::node, adevs::Bag<DIGRAPH::node> > Graph;
      Graph graph;
      Graph::const_iterator graph_iter;
      bool lb_cloning_is_successful = false;

      for (graph_iter = graph.begin(); graph_iter != graph.end();
	   graph_iter++) {

	// find the cloned sub-model for the source node
	ModelCloneMap::iterator iModelPair;
	iModelPair = lC_model_mirror.find(graph_iter->first.model);
	if ( iModelPair != lC_model_mirror.end() ) {
	  DEVS* lCp_SrcModel = iModelPair->second;
	  for (adevs::Bag<DIGRAPH::node>::iterator node_iter =
		 graph_iter->second.begin();
	       node_iter != graph_iter->second.end();
	       node_iter++) {
	    // find the cloned sub-model for the destination node
	    if ( (iModelPair = lC_model_mirror.find( (*node_iter).model ) )
		 != lC_model_mirror.end() ) {
	      DEVS* lCp_DstModel = iModelPair->second;
	      lCp_digraph_clone->couple( lCp_SrcModel, graph_iter->first.port,
					 lCp_DstModel, (*node_iter).port );
	    }
	    else { // destination node not found - coupling failed!
	      delete lCp_digraph_clone;
	      return 0;
	    }
	  } // for (std::set<DIGRAPH::node>::iterator...
	  
	} // if ( iModelPair != ...
	else {		// source node not found - coupling failed!
	  delete lCp_digraph_clone;
	  return 0;
	}
      }   // for (graph_iter...
	
      return lCp_digraph_clone;

    } // cloneDigraph(const DIGRAPH*)

    /**
     * Helper function for initializing an adevs model
     *
     * @param aCp_model handle to model
     * @returns whether the model has been successfully initialized
     */
    bool initializeModel( DEVS* aCp_model ) {
      if (!aCp_model)
	return false;

      try {
	// first attempt to use the InitObject interface if it is available
	InitObject* lCp_InitObj;
	if ( ( lCp_InitObj = dynamic_cast<InitObject*>(aCp_model) ) ) {
	  lCp_InitObj->initialize();
	  return true;
	}
      }
      catch(std::logic_error lC_excp) {
	std::cerr << "*** error *** Encountered <" << lC_excp.what()
		  << "> in function 'initializeModel(DEVS*)'\n";
	return false;
      }

      // next, if this is a network model
      NETWORK* lCp_network = 0;

      if ( ( lCp_network = dynamic_cast<NETWORK*>(aCp_model) ) ) {
	typedef adevs::Set<DEVS* > ComponentSet;

	ComponentSet lC_models;
	lCp_network->getComponents(lC_models);
	ComponentSet::iterator iModel;
	for ( iModel = lC_models.begin(); iModel != lC_models.end(); iModel++)
	  if (!initializeModel(*iModel))
	    return false;
      }

      // note (2009.02.28): initialization status may not be verified
      return true;
    }

    /**
     * Utility function that returns the root model of the adevs hierarchy
     * containing this model.
     *
     * @param aCp_model
     * @returns handle to root model
     */
    adevs::Devs<IO_Type>*
    getRootModel(adevs::Devs<IO_Type>* aCp_model)
    {
      adevs::Devs<IO_Type>* lCp_rootmodel = aCp_model;

      while (lCp_rootmodel->getParent() != 0)
	lCp_rootmodel = (adevs::Devs<IO_Type>*)lCp_rootmodel->getParent();

      return lCp_rootmodel;

    } // getRooModel(adevs::Devs<IO_Type>*)


    /**
     * Utility function that returns the root model of the adevs hierarchy
     * containing this model.
     *
     * @param aCp_model
     * @returns handle to root model
     */
    const adevs::Devs<IO_Type>*
    getRootModel(const adevs::Devs<IO_Type>* aCp_model)
    {
      const adevs::Devs<IO_Type>* lCp_rootmodel = aCp_model;

      while (lCp_rootmodel->getParent() != 0)
	lCp_rootmodel = (const adevs::Devs<IO_Type>*)lCp_rootmodel->getParent();

      return lCp_rootmodel;

    } // getRooModel(const adevs::Devs<IO_Type>*)


    /**
     * constructor
     *
     * @param aC_InputPort input port
     * @param aCp_CellSpace handle to cell space
     */
    CellBroadcastConsumer::CellBroadcastConsumer(PortType aC_InputPort,
						 efscape::impl::CELLSPACE*
						 aCp_CellSpace) :
      mC_InputPort(aC_InputPort),
      mCp_CellSpace(aCp_CellSpace)
    {
    }

    /**
     * Consumes input and generates a collection of input events
     *
     * @param aCr_input external input
     * @param aCr_InputBuffer bag of cell events
     */
    bool
    CellBroadcastConsumer::operator()(const adevs::Bag<IO_Type>& aCr_input,
				      adevs::Bag< adevs::Event<CellEvent> >&
				      aCCr_InputBuffer)
    {
      if (mCp_CellSpace == 0)
	return false;

      adevs::Bag<IO_Type>::iterator iter;
      for (iter = aCr_input.begin(); iter != aCr_input.end();
	   iter++) {

	if ((*iter).port != mC_InputPort)
	  return false;

	for (long iZ = 0; iZ < mCp_CellSpace->getDepth(); iZ++)
	  for (long iY = 0; iY < mCp_CellSpace->getHeight(); iY++)
	    for (long iX = 0; iX < mCp_CellSpace->getWidth(); iX++) {
	      adevs::Event< CellEvent > lC_event;
	      lC_event.model = mCp_CellSpace;
	      lC_event.value.x = iX;
	      lC_event.value.y = iY;
	      lC_event.value.z = iZ;
	      lC_event.value.value = (*iter).value;
	      aCCr_InputBuffer.insert(lC_event);
	    }
      }

      return true;
    }

    /**
     * constructor
     *
     * @param aC_InputPort output port
     * @param aCp_model handle to model
     */
    CellInputConsumer::CellInputConsumer(PortType aC_InputPort,
					 adevs::Devs<IO_Type>*
					 aCp_model) :
      mC_InputPort(aC_InputPort),
      mCp_model(aCp_model)
    {}

    /**
     * Consumes input and generates a collection of input events
     *
     * @param aCr_input external input
     * @param aCr_InputBuffer bag of events
     */
    bool
    CellInputConsumer::operator()(const adevs::Bag<CellEvent>& aCr_input,
				  adevs::Bag< adevs::Event<IO_Type> >&
				  aCCr_InputBuffer)
    {
      adevs::Bag<CellEvent>::iterator iter;
      for (iter = aCr_input.begin(); iter != aCr_input.end();
	   iter++) {

	// default: route external input to first port (0)
	aCCr_InputBuffer.insert(adevs::Event<IO_Type>(mCp_model,
						      IO_Type(mC_InputPort,
							      *iter)));
      }
      
      return true;
    }

  } // namespace impl

} // namespace efscape
