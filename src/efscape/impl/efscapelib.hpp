// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscapelib.hpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_EFSCAPELIB_HPP
#define EFSCAPE_IMPL_EFSCAPELIB_HPP

// adevs model definitions
#include <adevs.h>
#include <efscape/impl/adevs_decorator.h>

// boost libary definitions
#include <boost/any.hpp>

// jsoncpp library definitions
#include <json/json.h>

#include <string>
#include <vector>
#include <set>
#include <map>

namespace efscape {

  namespace impl {

    extern char const gcp_libname[]; // library name
    extern char const gcp_liburl[];  // library url

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
    void saveAdevsToXML(const DEVS* aCp_model, const char* acp_filename);
    void saveAdevsToXML(const DEVS* aCp_model, std::ostream& aCr_ostream);
    DEVS* loadAdevsFromXML(const char* acp_filename);
    DEVS* loadAdevsFromXML(std::istream& aCr_istream);

    void saveAdevsToJSON(const DEVSPtr& aCp_model,
			 std::ostream& aCr_ostream);
    DEVSPtr loadAdevsFromJSON(std::istream& aCr_istream);

    /**
     * Helper function for creating an adevs model from the model factory.
     *
     * @param acp_classname class name of model
     * @returns handle to model (null if missing from the factory)
     */
    DEVS* createModel(const char* acp_classname);

    /**
     * This template function creates a DEVS object of type T with attributes
     * embedded in a JSON. May be used to register model types with arguments
     * with the efscape model factory.
     *
     * @tparam T derived class
     * @param aC_value JSON value
     * @return handle to new model object
     */
    template <typename T>
    DEVS* createModelWithArgs(Json::Value aC_value) {
      return ( dynamic_cast<DEVS*> (new T(aC_value) ) );
    }

    /**
     * Helper function that converts a C++ type name string into a posix file
     * name compatible string by replacing or removing special characters.
     * Should work with simple types up to single parameter template classes.
     *
     * @param aC_cplusTypeName c++ type name
     * @return posix file friendly string
     */
    std::string cplusTypeName2Posix( std::string aC_cplusTypeName );

    /**
     * Helper function for cloning an adevs model using JSON serialization
     *
     * @param aCp_model handle to model
     * @return handle to model clone (null if cloning fails)
     */
    DEVSPtr cloneModelUsingJSON( const DEVSPtr& aCp_model );

    /**
     * Helper function for running a simulation
     *
     * @param aCp_model pointer to model
     * @param ad_timeMax time max
     */
    void runSim( DEVS* aCp_model, double ad_timeMax = DBL_MAX );

    /**
     * Helper function for create a simulation session that will
     * run in the efscape server workspace.
     *
     * @param aCp_model handle to model
     * @param aC_info model/session metadata
     * @return handle to simulator
     */
    adevs::Simulator<IO_Type>*
    createSimSession(DEVS* aCp_model, Json::Value aC_info);

    /**
     * Utility function that returns the root model of the adevs hierarchy
     * containing this model.
     *
     * @param aCp_model
     * @return handle to root model
     */
    adevs::Devs<IO_Type>*
    getRootModel(adevs::Devs<IO_Type>* aCp_model);

    /**
     * Utility function that returns the root model of the adevs hierarchy
     * containing this model.
     *
     * @param aCp_model
     * @return handle to root model
     */
    const adevs::Devs<IO_Type>*
    getRootModel(const adevs::Devs<IO_Type>* aCp_model);

    /**
     * Helper function for extracting the first instance of the specified model
     * type from a model object (if the model object is network).
     *
     * @tparam T model target type
     * @param aCp_model handle to model
     * @return handle to first instance of the specified model type
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
     * @return handle to first instance of the specified model type
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

#endif	// #ifndef EFSCAPE_IMPL_EFSCAPELIB_HPP
