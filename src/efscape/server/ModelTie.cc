// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelTie.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/server/ModelTie.hh>

#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/AdevsModel.hh>
#include <efscape/utils/type.hpp>

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
      efscape::impl::ClockIPtr lCp_clock;
      lCp_clock.reset(new efscape::impl::ClockI);

      efscape::impl::EntityI* lCp_WrappedModel =
	dynamic_cast<efscape::impl::EntityI*>(lCp_model);
      efscape::impl::AdevsModel* lCp_RootModel; // note: root model derived from model
      std::string lC_ParmName("efscape.xml"); // model parameter file
      if (lCp_WrappedModel) {
      	lCp_RootModel = dynamic_cast<efscape::impl::AdevsModel*>(lCp_model);
      	if (lCp_RootModel) {
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"Setting simulation environment from root model attributes");
      	  lCp_clock = lCp_RootModel->getClockIPtr();
      	  lC_ParmName =
      	    lCp_RootModel->getWorkDir() + "/" +
      	    lCp_RootModel->name() + ".xml";
      	}
      }
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Setting the simulation clock");
      mCp_ClockTie = new ClockTie(lCp_clock);

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
	}
	else
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"The simulation model is inert");

	// // save the initial state of the model in xml format
	// efscape::impl::saveAdevs(lCp_model,lC_ParmName.c_str());
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
      return (mCp_ClockTie->getClock()->time() =
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
     * @returns message
     */
    ::efscape::Message
    ModelTie::outputFunction(const Ice::Current& current)
    {
      ::efscape::Message lC_message;

      //   send clock proxy to port <clock_out>
      if (!mCp_ClockPrx) {
	if (mCp_ClockTie.get()) {
	  mCp_ClockPrx =
	    efscape::ClockPrx::uncheckedCast(current.adapter
					     ->addWithUUID( mCp_ClockTie ) );
	}
      }

      // add content to message
      ::efscape::ContentPtr lCp_content =
	  new ::efscape::Content(std::string("clock_out"),
				 mCp_ClockPrx);
      lC_message.push_back( lCp_content );

      translateOutput(current, lC_message);

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
      efscape::impl::EntityI* lCp_model;
      if ( (lCp_model =
	    dynamic_cast<efscape::impl::EntityI*>(aCp_model) ) )
	mC_name = lCp_model->name();
      else
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
      // delegate output processing to output producers
      std::vector<AdevsInputConsumerPtr>::iterator iConsumer;
      for (iConsumer = mC1_InputConsumers.begin();
	   iConsumer != mC1_InputConsumers.end();
	   iConsumer++) {
	(*(*iConsumer))(aCr_current, aCr_external_input, aCr_internal_input);
      }
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
      // delegate output processing to output producers
      std::vector<AdevsOutputProducerPtr>::iterator iProducer;
      for (iProducer = mC1_OutputProducers.begin();
	   iProducer != mC1_OutputProducers.end();
	   iProducer++) {
	(*(*iProducer))(aCr_current, mCC_OutputBuffer, aCr_external_output);
      }

      mCC_OutputBuffer.clear();

    } // ModelTie::translateOutput(...)

    /**
     * Adds an input consumer
     *
     * @param smart pointer to consumer
     */
    void ModelTie::addInputConsumer(const AdevsInputConsumerPtr&
				    aCr_consumer )
    {
      mC1_InputConsumers.push_back(aCr_consumer);
    }

    /**
     * Adds an output producer
     *
     * @param smart pointer to producer
     */
    void ModelTie::addOutputProducer(const AdevsOutputProducerPtr&
				     aCr_producer )
    {
      mC1_OutputProducers.push_back(aCr_producer);
    }

  } // namespace server

} // namespace efscape
