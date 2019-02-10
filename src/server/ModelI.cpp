// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelI.cpp
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
#include "ModelI.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/utils/type.hpp>

#include <json/json.h>
#include <fstream>

/**
 * Returns the name of the entity.
 *
 * @returns the name of the entity
 */
std::string
ModelI::getName(const Ice::Current& current) const
{
  return mC_name;
}

/**
 * Initializes the model.
 *
 * @param current method invocation
 * @returns status of call
 */
bool ModelI::initialize(const Ice::Current& current)
{
  if (!mCp_WrappedModel.get())
    return false;

  // verify that the wrapped model has been set properly
  adevs::Devs<efscape::impl::IO_Type>* lCp_model = mCp_WrappedModel.get();
  if (lCp_model == nullptr) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "main model does not exist");
    return false;
  }
  
  //----------------------------------------------------------------------------
  // initialize model
  //----------------------------------------------------------------------------
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"Initializing the model...");

  // adevs::Bag<efscape::impl::IO_Type> xb;
  // efscape::impl::IO_Type x("setup_in",
  // 			   "");
  // xb.insert(x);
  // efscape::impl::inject_events(0., xb, lCp_model);

  // create simulator and register the wrapper as an event listener
  mCp_simulator.reset
    ( efscape::impl::createSimSession(lCp_model, mC_info) );
  mCp_simulator->addEventListener(this);
     
  try {
    if ( mCp_simulator->nextEventTime() < DBL_MAX) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
  		    "The simulation model is ready to run!");
    }
    else
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
  		    "The simulation model is inert");
  }
  catch(std::logic_error lC_exp) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
  		  lC_exp.what());
  }

  return true;

} // ModelI::initialize(const Ice::Current&)

/**
 * Time advance function.
 *
 * @param current method invocation
 * @returns the next time advance
 */
Ice::Double ModelI::timeAdvance(const Ice::Current& current)
{
  return ( mCp_simulator->nextEventTime() );
}

/**
 * Internal transition function.
 *
 * @param current method invocation
 * @returns status of call
 */
bool ModelI::internalTransition(const Ice::Current& current)
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
bool ModelI::externalTransition(Ice::Double elapsedTime,
				  efscape::Message msg,
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
bool ModelI::confluentTransition(efscape::Message msg,
				   const Ice::Current& current)
{
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"ModelI::confluentTransition:"
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
efscape::Message
ModelI::outputFunction(const Ice::Current& current)
{
  efscape::Message lC_message;

  // add content to message
  translateOutput(current, lC_message);

  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"Returning output message");
  return lC_message;
}

/**
 * Returns the type of the model.
 *
 * @param current method invocation
 * @returns model type
 */
std::string
ModelI::getType(const Ice::Current& current) const
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
ModelI::setName(std::string aCr_name,
		  const Ice::Current& current)
{
  mC_name = aCr_name;
}

/**
 * Returns the configuration of the model in JSON format via a string.
 *
 * @param current method invocation
 * @returns model configuration in JSON format
 */
std::string
ModelI::saveJSON(const Ice::Current& current)
{
  std::string lC_config = "";
  return lC_config;
}

/**
 * Destroys the model.
 *
 * @param current method invocation
 */
void ModelI::destroy(const Ice::Current& current)
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
ModelI::ModelI() {}

/**
 * constructor
 *
 * @param aCp_model handle to model to be tied
 * @param aC_info model metadata
 */
ModelI::ModelI(const efscape::impl::DEVSPtr& aCp_model,
	       Json::Value aC_info) :
  mC_info(aC_info)
{
  if (aCp_model == nullptr)
    return;

  setWrappedModel(aCp_model);

  // Set model name
  mC_name = efscape::utils::type<efscape::impl::DEVS>(*aCp_model);

  if (!mC_info.isObject()) {
    mC_info =
      efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
      getModelFactory().getProperties(mC_name.c_str());    
  }
}

/**
 * constructor
 *
 * @param aCp_model handle to model to be tied
 * @param acp_name name of model
 */
ModelI::ModelI(const efscape::impl::DEVSPtr& aCp_model,
		   const char* acp_name)
{
  if (aCp_model == nullptr)
    return;

  setWrappedModel(aCp_model);
  mC_name = acp_name;
}

/** destructor */
ModelI::~ModelI() {}

/**
 * Listen to output from the wrapped model when its
 * output function is executed.
 *
 * @param x event
 * @param t time
 */
void ModelI::outputEvent(adevs::Event<efscape::impl::IO_Type> x, double t)
{
  mCC_OutputBuffer.insert(x);
} // end of ModelI::outputEvent(...)

/**
 * Converts messages from the wrapped model for output.
 *
 * @param aCr_current current method invocation
 * @param aCr_incoming incoming message
 * @param aCr_result converted input
 */
void
ModelI::translateInput(const Ice::Current& aCr_current,
			 const efscape::Message&
			 aCr_external_input,
			 adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
			 aCr_internal_input)
{
  /** @todo Convert JSON inputs into Json::Value objects */
}

/**
 * Converts messages from the wrapped model for output.
 *
 * @param aCr_current current method invocation
 * @param aCr_external_output converted output
 */
void ModelI::translateOutput(const Ice::Current& aCr_current,
			       efscape::Message& aCr_external_output)
{
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"ModelI::translateOutput(...): processing output...");

  //--------------------------------------
  // First check if the simulation is done
  // If so, retrieve final output
  //--------------------------------------
  if (mCp_simulator->nextEventTime() < DBL_MAX) {
    ;
  } else {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Done with simulation! Need to retrieve final output...");
    
    adevs::Bag<efscape::impl::IO_Type> xb;
    efscape::impl::get_output(xb, mCp_WrappedModel.get());
    // adevs::Bag<efscape::impl::IO_Type>::iterator i = xb.begin();
    // for (auto i = xb.begin(); i != xb.end(); i++) {
    for (auto i : xb) {
      try {
	Json::Value lC_value =
	  boost::any_cast<Json::Value>( i.value );

	efscape::Content lC_content;
	lC_content.port = i.port;
	
	std::ostringstream lC_buffer_out;
	lC_buffer_out << lC_value;
	lC_content.valueToJson = lC_buffer_out.str();

	aCr_external_output.push_back( lC_content );

      }
      catch(const boost::bad_any_cast &) {
	;
      }
    }
  }

  //-----------------------------------------------------------------------
  // Else if this simulation is not done, there should be events to process
  //-----------------------------------------------------------------------
  
  // adevs::Bag< adevs::Event<efscape::impl::IO_Type> >::iterator
  //   i = mCC_OutputBuffer.begin();
  for (auto i : mCC_OutputBuffer) {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Processing event on port<"
		  << i.value.port << ">...");
    try {
      Json::Value lC_value =
	boost::any_cast<Json::Value>( i.value.value );

      efscape::Content lC_content;
      lC_content.port = i.value.port;
	
      std::ostringstream lC_buffer_out;
      lC_buffer_out << lC_value;
      lC_content.valueToJson = lC_buffer_out.str();

      aCr_external_output.push_back( lC_content );
    }
    catch (const boost::bad_any_cast &) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Unable to translate output");
    }
  }

  mCC_OutputBuffer.clear();

} // ModelI::translateOutput(...)
