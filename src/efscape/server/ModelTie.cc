// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelTie.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/server/ModelTie.hh>

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/AdevsModel.hh>
#include <efscape/utils/type.hpp>
#include <efscape/utils/boost_utils.ipp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>

namespace efscape {

  namespace server {

    /**
     * Returns the name of the entity.
     *
     * @returns the name of the entity
     */
    ::std::string
    ModelTie::getName(const Ice::Current& current) const
    {
      return mC_name;
    }

    /**
     * Initializes the model.
     *
     * @param current method invocation
     * @returns status of call
     */
    bool ModelTie::initialize(const Ice::Current& current)
    {
      if (!mCp_WrappedModel.get())
	return false;

      // verify that the wrapped model has been set properly
      adevs::Devs<efscape::impl::IO_Type>* lCp_model = mCp_WrappedModel.get();
      if (lCp_model == 0) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "main model does not exist");
	return false;
      }

      // create simulator and register the wrapper as an event listener
      mCp_simulator.reset
	( new adevs::Simulator<efscape::impl::IO_Type>(lCp_model) );
      mCp_simulator->addEventListener(this);

      // initialize the simulation clock
      mCp_clock.reset(new efscape::impl::ClockI);

      efscape::impl::AdevsModel* lCp_RootModel =
	dynamic_cast<efscape::impl::AdevsModel*>(lCp_model);
      std::string lC_ParmName("efscape.xml"); // model parameter file
      if (lCp_RootModel) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Setting simulation environment from root model attributes");
	mCp_clock = lCp_RootModel->getClockIPtr();
	lC_ParmName =
	  lCp_RootModel->getWorkDir() + "/" +
	  lCp_RootModel->name() + ".xml";
      }
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Setting the simulation clock");

      try {
	// compute the initial state of the model

	if (!efscape::impl::initializeModel(lCp_model) )
	  // initialize wrapped model
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"Unable to initialize DEVS model -- continuing anyway")

	while ( mCp_simulator->nextEventTime() == 0) {
	  mCp_simulator->execNextEvent();
	}
	if ( mCp_simulator->nextEventTime() < DBL_MAX) {
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"The simulation model is ready to run!");

	  // now check if there is initial output from the model
	  adevs::Bag< efscape::impl::IO_Type > yb;
	  efscape::impl::get_output( yb,
				     mCp_WrappedModel.get() );
	  adevs::Bag< efscape::impl::IO_Type >::iterator
	    i = yb.begin();
	  for ( ; i != yb.end(); i++) {
	    try {
	      // if there is there is a property tree in the output,
	      // attempt to extract time parameters for the simulation
	      boost::property_tree::ptree pt =
		boost::any_cast<boost::property_tree::ptree>( (*i).value );
	      boost::optional<int> lC_timeMax = pt.get_optional<int>("timeMax");
	      if (lC_timeMax) {
		mCp_clock->timeMax() =
		  ((double)lC_timeMax.get());

		LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			      "Setting the simulation stop time to "
			      << mCp_clock->timeMax());
	      }
	    }
	    catch (const boost::bad_any_cast &) {
	      ;
	    }
	  } // for ( ; i != yb.end(); i++ )
	}
	else
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"The simulation model is inert");

	// save the initial state of the model in xml format
	try {
	  efscape::impl::saveAdevs(lCp_model,lC_ParmName.c_str());
	}
	catch(std::exception lC_exp) {
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			lC_exp.what()
			<< ": unable to save initial simulation state in XML format -- "
			<< "continuing...");
	}
      }
      catch(std::logic_error lC_exp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      lC_exp.what());
      }

      return true;

    } // ModelTie::initialize(const Ice::Current&)

    /**
     * Time advance function.
     *
     * @param current method invocation
     * @returns the next time advance
     */
    ::Ice::Double ModelTie::timeAdvance(const Ice::Current& current)
    {
      return (mCp_clock->time() =
	      mCp_simulator->nextEventTime());
    }

    /**
     * Internal transition function.
     *
     * @param current method invocation
     * @returns status of call
     */
    bool ModelTie::internalTransition(const Ice::Current& current)
    {
      // clear message buffer (i.e. invert conversion bag)
      mCC_OutputBuffer.clear();

      // compute the next state of the wrapped model at t
      mCp_simulator->execNextEvent();

      return true;

    }

    /**
     * External transition function.
     *
     * @param elapsedTime elapsed time
     * @param incoming message
     * @param current method invocation
     * @returns status of call
     */
    bool ModelTie::externalTransition(::Ice::Double elapsedTime,
				      const ::efscape::Message& msg,
				      const Ice::Current& current)
    {
      // convert incoming message into an acceptable form
      adevs::Bag< adevs::Event<efscape::impl::IO_Type> > lC_input;
      translateInput(current, msg, lC_input);

      // Apply the converted input to the wrapped model at time t
      mCp_simulator->computeNextState(lC_input, elapsedTime);

      return true;

    }

    /**
     * Confluent transition function.
     *
     * @param incoming message
     * @param current method invocation
     * @returns status of call
     */
    bool ModelTie::confluentTransition(const ::efscape::Message& msg,
				       const Ice::Current& current)
    {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "ModelTie::confluentTransition:"
		    << "message length = " << msg.size() );
	
      //---------------------------
      // 1) internalTransition(...)
      //---------------------------
      double ld_time = mCp_simulator->nextEventTime();
      internalTransition(current);

      //--------------------------
      // 2) externalTranstion(...)
      //--------------------------
      externalTransition(ld_time, msg, current);

      return true;
    }

    /**
     * Output function.
     *
     * @param current method invocation
     * @returns output message
     */
    ::efscape::Message
    ModelTie::outputFunction(const Ice::Current& current)
    {
      ::efscape::Message lC_message;
      
      // add content to message
      std::map<std::string, std::string> lC_ClockAttributes;
      lC_ClockAttributes["time_max"] =
	boost::lexical_cast<std::string>(mCp_clock->timeMax());
      boost::property_tree::ptree lC_pt =
	::efscape::utils::map_to_ptree<std::string,std::string>("clock",
								lC_ClockAttributes);
      std::string lC_json_str = ::efscape::utils::ptree_to_json(lC_pt);

      ::efscape::Content lC_content;
      lC_content.port = "clock_out";
      lC_content.valueToJson = ::efscape::utils::ptree_to_json(lC_pt);
      lC_message.push_back( lC_content );

      translateOutput(current, lC_message);

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Returning output message");
      return lC_message;
    }

    /**
     * Returns input port descriptions.
     *
     * @param current method invocation
     * @returns sequence of port descriptions
     */
    ::efscape::PortDescriptions
    ModelTie::getInPorts(const Ice::Current& current) const
    {
      ::efscape::PortDescriptions lC1_InPorts;
      return lC1_InPorts;
    }

    /**
     * Returns output port descriptions.
     *
     * @param current method invocation
     * @returns sequence of port descriptions
     */
    ::efscape::PortDescriptions
    ModelTie::getOutPorts(const Ice::Current& current) const
    {
      ::efscape::PortDescriptions lC1_OutPorts;
      return lC1_OutPorts;
    }

    /**
     * Returns the type of the model.
     *
     * @param current method invocation
     * @returns model type
     */
    ::std::string
    ModelTie::getType(const Ice::Current& current) const
    {
      return "";
    }

    /**
     * Returns the type of the model.
     *
     * @param current method invocation
     * @returns model type
     */
    ::std::string
    ModelTie::getBaseType(const Ice::Current& current) const
    {
      return "";
    }

    /**
     * Sets the name of the model.
     *
     * @param name new model name
     * @param current method invocation
     * @returns model type
     */
    void
    ModelTie::setName(const std::string& aCr_name,
		      const Ice::Current& current)
    {
      mC_name = aCr_name;
    }

    /**
     * Returns the configuration of the model in XML format via a string.
     *
     * @param current method invocation
     * @returns model configuration in XML format
     */
    ::std::wstring
    ModelTie::saveXML(const Ice::Current& current)
    {
      ::std::wstring lC_config = L"";
      return lC_config;
    }

    /**
     * Destroys the model.
     *
     * @param current method invocation
     */
    void ModelTie::destroy(const Ice::Current& current)
    {
      try {
	current.adapter->remove(current.id);
      } catch (const Ice::NotRegisteredException&) {
	throw Ice::ObjectNotExistException(__FILE__, __LINE__);
      }
    }

    //--------------------------------------------------------------------------
    // implementation of local (server-side) methods
    //--------------------------------------------------------------------------

    /** default constructor */
    ModelTie::ModelTie() {}

    /**
     * constructor
     *
     * @param aCp_model handle to model to be tied
     */
    ModelTie::ModelTie(adevs::Devs<efscape::impl::IO_Type>* aCp_model)
    {
      if (aCp_model == NULL)
	return;

      setWrappedModel(aCp_model);

      // Set model name
      mC_name = efscape::utils::type<efscape::impl::DEVS>(*aCp_model);

    }

    /**
     * constructor
     *
     * @param aCp_model handle to model to be tied
     * @param acp_name name of model
     */
    ModelTie::ModelTie(adevs::Devs<efscape::impl::IO_Type>* aCp_model,
		       const char* acp_name)
    {
      if (aCp_model == NULL)
	return;

      setWrappedModel(aCp_model);
      mC_name = acp_name;
    }

    /** destructor */
    ModelTie::~ModelTie() {}

    /**
     * Listen to output from the wrapped model when its
     * output function is executed.
     *
     * @param x event
     * @param t time
     */
    void ModelTie::outputEvent(adevs::Event<efscape::impl::IO_Type> x, double t)
    {
      mCC_OutputBuffer.insert(x);
    } // end of AdevsModel::outputEvent(...)

    /**
     * Converts messages from the wrapped model for output.
     *
     * @param aCr_current current method invocation
     * @param aCr_incoming incoming message
     * @param aCr_result converted input
     */
    void
    ModelTie::translateInput(const Ice::Current& aCr_current,
			     const ::efscape::Message&
			     aCr_external_input,
			     adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
			     aCr_internal_input)
    {
      /** @todo Convert JSON inputs into boost::property_tree::ptree objects */
    }

    /**
     * Converts messages from the wrapped model for output.
     *
     * @param aCr_current current method invocation
     * @param aCr_external_output converted output
     */
    void ModelTie::translateOutput(const Ice::Current& aCr_current,
				   ::efscape::Message& aCr_external_output)
    {
      std::cout << "ModelTie::translateOutput(...): processing output...\n";
      adevs::Bag< adevs::Event<efscape::impl::IO_Type> >::iterator
	i = mCC_OutputBuffer.begin();
      for ( ; i != mCC_OutputBuffer.end(); i++) {
	std::cout << "Processing event on port<"
		  << (*i).value.port << ">...\n";
	try {

	  // first try to extract a boost::property_tree::ptree	  
	  if (::efscape::utils::is_type<boost::property_tree::ptree>((*i).value.value)) {

	    boost::property_tree::ptree pt =
	      boost::any_cast<boost::property_tree::ptree>( (*i).value.value );

	    // generate JSON output from the ptree
	    ::efscape::Content lC_content;
	    lC_content.port = (*i).value.port;
	    lC_content.valueToJson = ::efscape::utils::ptree_to_json(pt);
	    aCr_external_output.push_back( lC_content );
	  }
	  
	}
	catch (const boost::bad_any_cast &) {
	  std::cout << "Unable to translate output\n";
	}
      }

      mCC_OutputBuffer.clear();

    } // ModelTie::translateOutput(...)

  } // namespace server

} // namespace efscape
