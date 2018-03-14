// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_config.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/adevs_config.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <fstream>
#include <sstream>

namespace efscape {

  namespace impl {

    char const gcp_libname[] = "libefscape-impl";

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
	std::cout << "model is null!\n";
	return;
      }
      ATOMIC* lCp_atomic = NULL;
      if ( (lCp_atomic = aCp_model->typeIsAtomic() ) != NULL ) {
	lCp_atomic->output_func(xb);
      }
      else {
	NETWORK* lCp_network = NULL;
	if ( (lCp_network = aCp_model->typeIsNetwork()) == NULL)
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

    DEVS* createModel( const char* acp_classname ) {
      return ( Singleton<ModelHomeI>::Instance().getModelFactory().createObject(acp_classname) );
    }

   DEVSPtr cloneModelUsingJSON( const DEVSPtr& aCp_model ) {

     DEVSPtr lCp_clone;
     try {
       // serialize the model out to a buffer and then back in as a clone
       std::ostringstream lC_buffer_out;
       saveAdevsToJSON(aCp_model, lC_buffer_out);
       std::istringstream lC_buffer_in(lC_buffer_out.str().c_str());
       lCp_clone = loadAdevsFromJSON(lC_buffer_in);
     } catch(...) {
       LOG4CXX_ERROR(ModelHomeI::getLogger(),
		     "Exception encountered during attempt to clone adevs model");
     }

     return lCp_clone;
    }

    void runSim( DEVS* aCp_model, double ad_timeMax ) {
      adevs::Simulator<IO_Type> lC_sim( aCp_model );

      // simulate model until infinity
      while (lC_sim.nextEventTime() < ad_timeMax) {
	lC_sim.execNextEvent();
      }
    }

    adevs::Devs<IO_Type>*
    getRootModel(adevs::Devs<IO_Type>* aCp_model)
    {
      adevs::Devs<IO_Type>* lCp_rootmodel = aCp_model;

      while (lCp_rootmodel->getParent() != 0)
	lCp_rootmodel = (adevs::Devs<IO_Type>*)lCp_rootmodel->getParent();

      return lCp_rootmodel;

    } // getRooModel(adevs::Devs<IO_Type>*)

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
