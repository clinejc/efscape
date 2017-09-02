// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_config.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_ADEVS_CONFIG_HPP
#define EFSCAPE_IMPL_ADEVS_CONFIG_HPP

// adevs model definitions
#include <adevs.h>
#include <efscape/impl/adevs_decorator.h>

// boost libary definitions
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <set>
#include <map>

namespace efscape {

  namespace impl {

    extern char const gcp_libname[]; // library name

    // define the base adevs io type and cell event for the efscape library
    typedef std::string PortType;
    typedef adevs::PortValue<boost::any,PortType> IO_Type;
    typedef adevs::Devs<IO_Type> DEVS;
    typedef std::shared_ptr<DEVS> DEVSPtr;
    typedef adevs::Atomic<IO_Type> ATOMIC;
    typedef adevs::ModelWrapper<IO_Type,IO_Type> ModelWrapperBase;
    typedef adevs::Network<IO_Type> NETWORK;
    typedef adevs::Network<IO_Type> NetworkModel;
    typedef adevs::ModelDecorator<IO_Type> ModelDecorator;
    typedef adevs::SimpleDigraph<boost::any> SIMPLEDIGRAPH;
    typedef adevs::Digraph<boost::any,PortType> DIGRAPH;
    typedef adevs::CellEvent<boost::any> CellEvent;
    typedef adevs::Devs<CellEvent> CellDevs;
    typedef adevs::Atomic<CellEvent> CellModelBase;
    typedef adevs::Network<CellEvent> CellNetwork;
    typedef adevs::CellSpace<boost::any> CELLSPACE;
    typedef adevs::ModelWrapper<IO_Type,CellEvent> CellSpaceWrapperBase;
    typedef adevs::ModelWrapper<CellEvent,IO_Type> CellWrapperBase;
    typedef adevs::EventListener<IO_Type> EventListener;
    typedef adevs::EventListener<CellEvent> CellEventListener;

    // injects a bag of events into a model
    void inject_events(double e, const adevs::Bag<IO_Type>& xb,
		       DEVS* aCp_model);

    // get (extract) output from a model
    void get_output(adevs::Bag<IO_Type>& yb,
		    DEVS* aCp_model);

    // serialization functions for adevs::Devs<IO_Type> models
    void saveAdevs(const DEVS* aCp_model, const char* acp_filename);
    void saveAdevs(const DEVS* aCp_model, std::ostream& aCr_ostream);
    DEVS* loadAdevs(const char* acp_filename);
    DEVS* loadAdevs(std::istream& aCr_istream);

    void saveAdevsToJSON(const DEVSPtr& aCp_model,
			 std::ostream& aCr_ostream);
    DEVSPtr loadAdevsFromJSON(std::istream& aCr_istream);

    // Helper functions for creating, cloning, and initializing an adevs model
    DEVS* createModel(const char* acp_classname);
    DEVSPtr cloneModel( const DEVSPtr& aCp_model );

    void runSim( DEVS* aCp_model, double ad_timeMax = DBL_MAX );

    // Returns the root model of the adevs hierarchy.containing this model.
    adevs::Devs<IO_Type>*
    getRootModel(adevs::Devs<IO_Type>* aCp_model);

    const adevs::Devs<IO_Type>*
    getRootModel(const adevs::Devs<IO_Type>* aCp_model);

    /**
     * Helper function for extracting the first instance of the specified model
     * type from a model object (if the model object is network).
     *
     * @tparam T model target type
     * @param aCp_model handle to model
     * @returns handle to first instance of the specified model type
     */
    template <typename T>
    T* getTargetModel( DEVS* aCp_model )
    {
      T* lCp_target = dynamic_cast<T*>(aCp_model);

      if (lCp_target == 0) {

	NETWORK* lCp_network = 0;
	if ( ( lCp_network = dynamic_cast<NETWORK*>(aCp_model) ) ) {
	  typedef adevs::Set< DEVS* > ComponentSet;

	  ComponentSet lC_models;
	  lCp_network->getComponents(lC_models);
	  ComponentSet::iterator iModel;
	  for ( iModel = lC_models.begin(); iModel != lC_models.end(); iModel++ )
	    if ( ( lCp_target = getTargetModel<T>(*iModel) ) )
	      break;
	}
      }
      return lCp_target;
    }

    /**
     * Helper function for extracting the first instance of the specified model
     * type from a model object (if the model object is decorator or digraph).
     *
     * @tparam T model target type
     * @param aCp_model handle to model
     * @returns handle to first instance of the specified model type
     */
    template <typename T>
    const T* getTargetModel( const DEVS* aCp_model )
    {
      const T* lCp_target = dynamic_cast<const T*>(aCp_model);

      if (lCp_target == 0) {

	const ModelDecorator* lCp_decorator = dynamic_cast<const ModelDecorator*>(aCp_model);
	const DIGRAPH* lCp_digraph = 0;

	if (lCp_decorator) {
	  aCp_model = lCp_decorator->getWrappedModel();
	  lCp_target = getTargetModel<T>(aCp_model);
	}
	else if ( ( lCp_digraph = dynamic_cast<const DIGRAPH*>(aCp_model) ) ) {
	  typedef adevs::Set<DEVS* > ComponentSet;

	  ComponentSet lC_models;
	  lCp_digraph->getComponents(lC_models);
	  ComponentSet::const_iterator iModel;
	  for ( iModel = lC_models.begin(); iModel != lC_models.end(); iModel++ )
	    if ( ( lCp_target = getTargetModel<T>(*iModel) ) )
	      break;
	}
      }
      return lCp_target;
    }

    //----------------------------------------------
    // classes for converting model input and output
    //----------------------------------------------
    /**
     * Base template class for converting model input into a bag of internal
     * events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 11 Oct 2007, revised 11 Oct 2007
     */
    template <class InputType, class OutputType>
    class InputConsumer
    {
    public:

      /** default constructor */
      InputConsumer() {}

      /** destructor */
      ~InputConsumer() {}

      virtual bool operator()(const adevs::Bag<InputType>& aCr_input,
			      adevs::Bag< adevs::Event<OutputType> >&
			      aCCr_output) = 0;
    };

    /**
     * Base template class for converting internal model input into a bag
     * of external events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.02 created 11 Oct 2007, revised 05 Apr 2008
     */
    template <class InputType, class OutputType>
    class OutputProducer
    {
    public:

      /** default constructor */
      OutputProducer() {}

      /** destructor */
      ~OutputProducer() {}

      virtual bool operator()(const adevs::Bag<adevs::Event<InputType> >&
			      aCr_input,
			      adevs::Bag< OutputType >&
			      aCCr_output) = 0;
    };

    // typedefs for InputConsumers and OutputProducers
    typedef InputConsumer<IO_Type,IO_Type> PortPortConsumer;
    typedef InputConsumer<IO_Type,CellEvent> PortCellConsumer;
    typedef InputConsumer<CellEvent,IO_Type> CellPortConsumer;
    typedef OutputProducer<IO_Type,IO_Type> PortPortProducer;
    typedef OutputProducer<CellEvent,IO_Type> CellPortProducer;
    typedef OutputProducer<IO_Type,CellEvent> PortCellProducer;

    typedef std::shared_ptr< PortPortConsumer > PortPortConsumerPtr;
    typedef std::shared_ptr< PortCellConsumer > PortCellConsumerPtr;
    typedef std::shared_ptr< CellPortConsumer > CellPortConsumerPtr;
    typedef std::shared_ptr< PortPortProducer > PortPortProducerPtr;
    typedef std::shared_ptr< PortCellProducer > PortCellProducerPtr;
    typedef std::shared_ptr< CellPortProducer > CellPortProducerPtr;

    typedef std::vector<PortPortConsumerPtr> PortPortConsumerVector;
    typedef std::vector<PortCellConsumerPtr> PortCellConsumerVector;
    typedef std::vector<CellPortConsumerPtr> CellPortConsumerVector;
    typedef std::vector<PortPortProducerPtr> PortPortProducerVector;
    typedef std::vector<PortCellProducerPtr> PortCellProducerVector;
    typedef std::vector<CellPortProducerPtr> CellPortProducerVector;

    /**
     * A base class for function objects that converts incoming port data into
     * a collection of cell events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 11 Oct 2007, revised 22 Apr 2008
     */
    class CellBroadcastConsumer : public PortCellConsumer
    {
    public:

      CellBroadcastConsumer(PortType aC_InputPort,
			    CELLSPACE* aCp_CellSpace);

      bool operator()(const adevs::Bag<IO_Type>& aCr_input,
		      adevs::Bag< adevs::Event<CellEvent> >&
		      aCCr_output);
    protected:

      /** input port */
      PortType mC_InputPort;

      /** handle to cell space model */
      CELLSPACE* mCp_CellSpace;
    };

    /**
     * A base class for function objects that converts incoming port data into
     * a collection of internal events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 11 Oct 2007, revised 11 Oct 2007
     */
    class CellInputConsumer : public CellPortConsumer
    {
    public:

      CellInputConsumer(PortType aC_InputPort,
			adevs::Devs<IO_Type>* aCp_model);

      bool operator()(const adevs::Bag<CellEvent>& aCr_input,
		      adevs::Bag< adevs::Event<IO_Type> >&
		      aCCr_output);
    protected:

      /** input port */
      PortType mC_InputPort;

      /** handle to model */
      adevs::Devs<IO_Type>* mCp_model;
    };

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_ADEVS_CONFIG_HPP
