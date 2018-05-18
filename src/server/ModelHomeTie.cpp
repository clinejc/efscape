// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.cpp
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
#include "ModelHomeTie.hpp"

#include "ModelTie.hpp"
#include "SimulatorI.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <json/json.h>

#include <sstream>

/**
 * Create a new efscape::Model proxy from the specified model class name.
 *
 * @param name model class name
 * @param current method invocation
 * @returns std::shared_ptr<efscape::ModelPrx> proxy
 */
std::shared_ptr<efscape::ModelPrx>
ModelHomeTie::create(std::string name,
		     const Ice::Current& current)
{
  // attempt create the model
  efscape::impl::DEVSPtr lCp_model =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModel(name);

  if (lCp_model != nullptr) {
    // wrap and activate the model
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Model <" << name << "> found...");
    auto modelI =
      Ice::uncheckedCast<efscape::ModelPrx>(current.adapter
					    ->addWithUUID(::std::make_shared<ModelTie>(lCp_model)));
  
    return modelI;
  }
  
  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		"Model <"
		<< name << "> not found...");

  return nullptr;

} // ModelHomeTie::create(...)

/**
 * Create a new efscape::Model proxy from the specified XML file.
 *
 * @param configuration model XML configuration
 * @param current method invocation
 * @returns efscape::Model proxy
 */
std::shared_ptr<efscape::ModelPrx>
ModelHomeTie::createFromXML(std::wstring configuration,
			    const Ice::Current& current)
{
  try {
    // attempt create the model
    efscape::impl::DEVSPtr lCp_modelI =
      efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModelFromXML(configuration);

    if (lCp_modelI != nullptr) {
      // wrap and activate the model
      auto modelI = Ice::uncheckedCast<efscape::ModelPrx>(current.adapter
							    ->addWithUUID(std::make_shared<ModelTie>(lCp_modelI)));
  
      return modelI;
    }
    else {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "efscape::server::ModelHomeTie::createFromXML(): model implementation not found!");
    }
  }
  catch (std::logic_error exp) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "efscape::server::ModelHomeTie::createFromXML(): "
		  << exp.what() );
  }

  return nullptr;

} // ModelHomeTie::createFromXML(...)

/**
 * Create a new efscape::Model proxy from the specified JSON file.
 *
 * @param configuration model JSON configuration
 * @param current method invocation
 * @returns efscape::Model proxy
 */
std::shared_ptr<efscape::ModelPrx>
ModelHomeTie::createFromJSON(std::string configuration,
			     const Ice::Current& current)
{
  try {
    // attempt create the model
    efscape::impl::DEVSPtr lCp_modelI =
      efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModelFromJSON(configuration);

    if (lCp_modelI != nullptr) {
      auto modelI = Ice::uncheckedCast<efscape::ModelPrx>(current.adapter
							    ->addWithUUID(std::make_shared<ModelTie>(lCp_modelI)));
  
      return modelI;
    }
    else {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "efscape::server::ModelHomeTie::createFromJSON(): model implementation not found!");
    }
  }
  catch (std::logic_error exp) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "efscape::server::ModelHomeTie::createFromJSON(): "
		  << exp.what() );
  }

  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		"efscape::server::ModelHomeTie::createFromJSON(): "

		"Model not found...");
  return nullptr;

} // ModelHomeTie::createFromJSON(...)

/**
 * Create a new efscape::Model proxy from the specified JSON parameter
 * file.
 *
 * @param parameters string containing model parameters JSON
 * @param current method invocation
 * @returns efscape::Model proxy
 */
std::shared_ptr<efscape::ModelPrx>
ModelHomeTie::createFromParameters(std::string parameters,
				   const Ice::Current& current)
{
  try {
    // attempt create the model
    efscape::impl::DEVSPtr lCp_modelI =
      efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModelFromParameters(parameters);

    if (lCp_modelI != nullptr) {
      // tie the model
      auto modelI = Ice::uncheckedCast<efscape::ModelPrx>(current.adapter
							    ->addWithUUID(std::make_shared<ModelTie>(lCp_modelI)));
  
      return modelI;
    }
    else {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "efscape::server::ModelHomeTie::createModelFromParameters(): model implementation not found!");
    }
  }
  catch (std::logic_error exp) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "efscape::server::ModelHomeTie::createModelFromParameters(): "
		  << exp.what() );
  }

  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		"Model not found...");
  return nullptr;

}

/**
 * Returns a list of available models.
 *
 * @param current method invocation
 * @returns list of available models
 */
efscape::ModelNameList ModelHomeTie::getModelList(const Ice::Current& current)
{
  efscape::ModelNameList lC1_ModelNameList;

  // retrieve the list of available models
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"*** Available models: ***");
  std::set<std::string> lC1_ModelNames =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().getTypeIDs();
  std::set<std::string>::iterator iter;
  for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++) {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "=> modelName=" << *iter);
    lC1_ModelNameList.push_back(*iter);
  }

  return lC1_ModelNameList;
}

/**
 * Returns info about the specified model in a JSON string
 *
 * @param aC_name type of model
 * @param current method invocation
 * @returns info about the specified model in a JSON string
 */
std::string ModelHomeTie::getModelInfo(std::string aC_name,
					 const Ice::Current& current)
{
  Json::Value lC_ModelInfo =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().getProperties(aC_name.c_str());

  std::string lC_JsonStr = "{}";
  try {
    std::ostringstream lC_buffer;
    lC_buffer << lC_ModelInfo;
    lC_JsonStr = lC_buffer.str();
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "JSON=>" << lC_JsonStr);
  }
  catch (...) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "unknown exception converting JSON to string.");
  }

  return lC_JsonStr;
}

/**
 * Creates a simulator object for a model.
 *
 * @param rootModel handle to root model
 * @param current method invocation
 * @returns efscape::Simulator proxy
 */
std::shared_ptr<efscape::SimulatorPrx>
ModelHomeTie::createSim(std::shared_ptr<efscape::ModelPrx> rootModel,
			const Ice::Current& current)
{
  auto simulatorI =
    Ice::uncheckedCast<efscape::SimulatorPrx>(current.adapter
						->addWithUUID(std::make_shared<SimulatorI>(rootModel)));
  
  return simulatorI;
}

//--------------------------------------------------------------------------
// implementation of local (server-side) methods
//--------------------------------------------------------------------------

/** constructor */
ModelHomeTie::ModelHomeTie()
{
  try {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Loading libraries");
    // efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().LoadLibraries();
  }
  catch(std::logic_error lC_excp) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  lC_excp.what());
  }
  catch (...) {
    LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		  "A generic exception occurred");
  }

} // ModelHomeTie::ModelHomeTie()


/** destructor */
ModelHomeTie::~ModelHomeTie() {
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"Shutting down EFSCAPE ICE model repository...");
}
