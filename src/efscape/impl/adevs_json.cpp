// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/adevs_json.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ClockI.hpp>

// boost definitions
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <sstream>

namespace efscape {

  namespace impl {

    //
    // utility functions
    //
    
    void convert_from_json(const Json::Value& aCr_value, ClockI& aCr_clock) {
      if (aCr_value["delta_t"].isDouble())
	aCr_clock.timeDelta() = aCr_value["delta_t"].asDouble();
      if (aCr_value["stopAt"].isDouble())
	aCr_clock.timeMax() = aCr_value["stopAt"].asDouble();
      if (aCr_value["units"].isString()) {
	aCr_clock.timeUnits( aCr_value["units"].asString().c_str() );
      }   
    }

    Json::Value convert_to_json(const ClockI& aCr_clock) {
      Json::Value lC_value;
      
      lC_value["delta_t"] = aCr_clock.timeDelta();
      lC_value["stopAt"] = aCr_clock.timeMax();
      lC_value["units"] = aCr_clock.timeUnits();

      return lC_value;
    }

    // utility function for building a model from JSON
    DEVS* createModelFromJSON(const Json::Value& aC_config) {
      // This function supports the following model type configurations:
      // 1. ATOMIC
      //    a. ModelWrapper (contains a "wrappedModel")
      // 2. NETWORK (contains a set of "models")
      //    a. DIGRAPH (contains an array of "couplings")

      DEVS* lCp_model = NULL;
      if (!aC_config) {
	return lCp_model;
      }

      // check <baseClassName>
      Json::Value lC_attribute = aC_config["baseClassName"];
      if (!lC_attribute) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Missing <baseClassName>");
	return lCp_model;
      }

      // check <className>
      if (!(lC_attribute = aC_config["className"]) ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Missing <className>");
	return lCp_model;
      }

      if (!lC_attribute.isString()) { // <className> should be a string
	return lCp_model;
      }

      std::string lC_className = lC_attribute.asString();
      
      // attempt to create the model from the factory
      if ( (lCp_model = createModel(lC_className.c_str()) )
	     != NULL ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Successfully created the model!");

	NetworkModel* lCp_networkModel = lCp_model->typeIsNetwork();

	if (lCp_model->typeIsNetwork()) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"This is a NetworkModel");

	  // check for network model components
	  Json::Value lC_modelsAttribute = aC_config["models"];
	  if ( !lC_modelsAttribute ) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Missing network model components");
	    return lCp_model;
	  }
	  if (!lC_modelsAttribute.isObject()) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "attribute <models> is not an object");
	    return lCp_model;
	  }

	  // check if this is a Digraph
	  DIGRAPH* lCp_digraph =
	    dynamic_cast<DIGRAPH*>(lCp_model);
	  if (lCp_digraph) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is a Digraph");
	    lCp_model = DigraphBuilder::build_digraph_from_json(aC_config,
								lCp_digraph);
	    if (!lCp_model)
	      return lCp_model;	// should be NULL
	    
	  }	// if (lCp_digraph)
	}	// if (lCp_networkModel)
	else {		// This is an ATOMIC model
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"This is an ATOMIC model");

	  ModelWrapperBase* lCp_modelWrapper =
	    dynamic_cast<ModelWrapperBase*>(lCp_model);

	  if (lCp_modelWrapper) { // if this is a ModelWrapper
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is a ModelWrapper");
	    Json::Value lC_wrappedModel = aC_config["wrappedModel"];

	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "wrappedModel=>"
			  << lC_wrappedModel);

	    if (!lC_wrappedModel.isObject()) {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Missing <wrappedModel>: "
			    << "deleting model wrapper");
	      delete lCp_modelWrapper;
	      return NULL;
	    }

	    if (!lC_wrappedModel["className"].isString()) {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Missing wrapped model <className>");
	      return lCp_model;
	    }

	    lC_className = lC_wrappedModel["className"].asString();

	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Attempting to create wrapped model of type <"
			  << lC_className << ">");
	    DEVS* lCp_wrappedModel =
	      createModelFromJSON(lC_wrappedModel);
	    
	    if (lCp_wrappedModel) { // if the wrappedModel exists
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Adding the wrappedModel...");
	      lCp_modelWrapper->setWrappedModel(lCp_wrappedModel);
	    }
	    else {
	      LOG4CXX_ERROR(ModelHomeI::getLogger(),
			    "The wrappedModel is missing!");
	    }

	    // inject input (model properties file)
	    // note: must be done before initializing the simulator
	    adevs::Bag<efscape::impl::IO_Type> xb;
	    efscape::impl::IO_Type e;
	    e.port = "properties_in";
	    
	    std::ostringstream lC_buffer_out;
	    lC_buffer_out << aC_config;
	    e.value = lC_buffer_out.str();
	    
	    xb.insert(e);

	    inject_events(-0.1, xb, lCp_model);

	  }
	} // else [this is an ATOMIC model
      }	  // if (!aC_config)
      
      return lCp_model;
      
    } // createModelFromJSON(const Json::Value&)

    // utility function for loading info from a JSON file
    boost::property_tree::ptree loadInfoFromJSON(std::string aC_path) {
      // path is relative
      std::string lC_FileName =
	ModelHomeI::getHomeDir() + std::string("/") + aC_path;

      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "model info path = <" << lC_FileName << "> ***");

      boost::property_tree::ptree pt;
      std::string lC_JsonStr = "{}";
      try {
	boost::property_tree::read_json( lC_FileName.c_str(), pt );

	std::ostringstream lC_buffer;
	boost::property_tree::write_json(lC_buffer, pt, false);
	lC_JsonStr = lC_buffer.str();
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "JSON=>" << lC_JsonStr);
      }
      catch (...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "unknown exception occurred parsing model info JSON file.");
      }

      return pt;
    }

    //
    // DigraphBuilder
    //
    Json::Value DigraphBuilder::convert_to_json() const {
      Json::Value lC_config;
      lC_config["baseClassName"] = "efscape::impl::DEVS";
      lC_config["className"] = "efscape::impl::DIGRAPH";
      
      Json::Value lC_models;
      Json::Value lC1_couplings;

      for (std::map<std::string, Json::Value>::const_iterator it = mCC_models.begin();
	   it != mCC_models.end(); it++) {
	lC_models[it->first] = it->second;
      }
      lC_config["models"] = lC_models;

      std::istringstream lC_buffer_in("[]");
      lC_buffer_in >> lC1_couplings;

      for (int i = 0; i < mC1_couplings.size(); i++) {
	Json::Value lC_coupling = mC1_couplings[i].convert_to_json();
	lC1_couplings.append(lC_coupling);
      }
      lC_config["couplings"] = lC1_couplings;

      return lC_config;
    }

    DEVS* DigraphBuilder::build_digraph_from_json(const Json::Value& aCr_value,
						  DIGRAPH* aCp_digraph)
    {
      Json::Value lC_modelsAttribute = aCr_value["models"];
      Json::Value lC1_couplings = aCr_value["couplings"];

      if (!lC_modelsAttribute.isObject()) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "DIGRAPH attribute <models> is either missing or is not an object");
	delete aCp_digraph;
	return NULL;
      }

      if (!lC1_couplings.isArray()) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "DIGRAPH attribute <couplings> is either missing or is not an array");
	delete aCp_digraph;
	return NULL;
      }

      // add models
      Json::Value::Members lC_memberNames =
	lC_modelsAttribute.getMemberNames();
      std::map<std::string, DEVS*> lCC_modelMap;
      lCC_modelMap["this"] = aCp_digraph;
      
      for (int i = 0; i < lC_memberNames.size(); i++) {
	DEVS* lCp_subModel =
	  createModelFromJSON(lC_modelsAttribute[ lC_memberNames[i] ] );
	if (lCp_subModel) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Adding model <"
			<< lC_memberNames[i]
			<< ">");
	  lCC_modelMap[ lC_memberNames[i] ] = lCp_subModel;
	  aCp_digraph->add(lCp_subModel);
	}
	else {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Missing model <"
			<< lC_memberNames[i]
			<< ">");
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Deleting incomplete Digraph model...");
	  delete aCp_digraph;
	  return NULL;
	}
      }

      // add couplings
      for (int i = 0; i < lC1_couplings.size(); i++) {
	Json::Value lC_couplingValue = lC1_couplings[i];
	struct coupling dgc;
	dgc.convert_from_json(lC_couplingValue);

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Adding model coupling ("
		      << dgc.from.model << "," << dgc.from.port << ")"
		      << "=>("
		      << dgc.to.model << "," << dgc.to.port << ")");

	// the component models should exist -- not checking
	if (lCC_modelMap.find(dgc.from.model) == lCC_modelMap.end() ) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"model <" << dgc.from.model << "> not found");
	  continue;
	}
	DEVS* fromModel = lCC_modelMap[dgc.from.model];

	if (lCC_modelMap.find(dgc.from.model) == lCC_modelMap.end()) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"model <" << dgc.from.model << "> not found");
	  continue;
	}
	DEVS* toModel = lCC_modelMap[dgc.to.model];

	aCp_digraph->couple(fromModel, dgc.from.port,
			    toModel, dgc.to.port);
      }

      return aCp_digraph;
    }

  } // namespace impl

} // namespace efscape
