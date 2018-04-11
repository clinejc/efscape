// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.cpp
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
#include <efscape/impl/adevs_json.hpp>

#include <efscape/impl/ModelHomeI.hpp>

namespace efscape {

  namespace impl {


    // utility function for building a model from JSON
    DEVS* buildModelFromJSON(Json::Value aC_config) {
      // This function supports the following model type configurations:
      // 1. ATOMIC
      //    a. ModelWrapper (contains a "wrappedModel")
      // 2. NETWORK (contains a set of "models")
      //    a. DIGRAPH (contains an array of "couplings")

      DEVS* lCp_model = NULL;
      if (!aC_config) {
	return lCp_model;
      }

      // check <modelTypeName>
      Json::Value lC_attribute = aC_config["modelTypeName"];
      if (!lC_attribute.isString()) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Missing <modelTypeName>");
	return lCp_model;
      }
      
      std::string lC_modelTypeName = lC_attribute.asString();
      
      // attempt to create the model from the factory
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Attempt to create a <"
		    << lC_modelTypeName
		    << "> model");
      if ( (lCp_model = createModel(lC_modelTypeName.c_str()) )
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

	    DEVSPtr lCp_wrappedModel( buildModelFromJSON(lC_wrappedModel ) );
	    
	    if (lCp_wrappedModel) { // if the wrappedModel exists
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Adding the wrappedModel...");
	      lCp_modelWrapper->setWrappedModel(lCp_wrappedModel);
	    }
	    else {
	      LOG4CXX_ERROR(ModelHomeI::getLogger(),
			    "The wrappedModel is missing!");
	    } // else missing a wrapped model
	  } // else not a wrapper model
	  
	  // inject input (model properties file)
	  // note: must be done before initializing the simulator
	  Json::Value lC_properties = aC_config["properties"];
	  if ( lC_properties.isObject() ) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Attempting to load properties for model <"
			  << lC_modelTypeName << ">");

	    adevs::Bag<efscape::impl::IO_Type> xb;
	    efscape::impl::IO_Type e;
	    e.port = "properties_in";
	    e.value = lC_properties;
	    
	    xb.insert(e);

	    inject_events(0.0, xb, lCp_model);
	  } else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "No properties for model <"
			  << lC_modelTypeName << ">");
	  }
	} // else [this is an ATOMIC model
      }	  // if (!aC_config)
      
      return lCp_model;
      
    } // buildModelFromJSON(const Json::Value&)

    //
    // DigraphBuilder
    //
    Json::Value DigraphBuilder::convert_to_json() const {
      Json::Value lC_config;
      
      lC_config["modelTypeName"] = "efscape::impl::DIGRAPH";
      
      Json::Value lC_models;
      Json::Value lC1_couplings;

      for (std::map<std::string, Json::Value>::const_iterator it = mCC_models.begin();
	   it != mCC_models.end(); it++) {
	lC_models[it->first] = it->second;
      }
      lC_config["models"] = lC_models;

      lC1_couplings = Json::Value(Json::arrayValue);

      for (int i = 0; i < mC1_couplings.size(); i++) {
	Json::Value lC_edge = mC1_couplings[i].convert_to_json();
	lC1_couplings.append(lC_edge);
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
	  buildModelFromJSON(lC_modelsAttribute[ lC_memberNames[i] ] );
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
	Json::Value lC_edgeValue = lC1_couplings[i];
	struct edge dgc;
	dgc.convert_from_json(lC_edgeValue);

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Adding model edge ("
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
