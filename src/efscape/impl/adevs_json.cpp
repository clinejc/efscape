// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/adevs_json.hpp>

#include <efscape/impl/ModelHomeI.hpp>

// boost definitions
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <sstream>

namespace efscape {

  namespace impl {

    // utility function for building a model from JSON
    DEVS* createModelFromJSON(const boost::property_tree::ptree& aC_pt) {
      // This function supports the following model type configurations:
      // 1. ATOMIC
      //    a. ModelWrapper (contains a "wrappedModel")
      // 2. NETWORK (contains a set of "models")
      //    a. DIGRAPH (contains an array of "couplings")

      // DEVS model info
      bool lb_isa_model = false;
      std::string lC_modelType;
      DEVS* lCp_model = NULL;

      // ModelWrapper info
      DEVS* lCp_wrappedModel = NULL;

      // NetworkModel info
      std::map<std::string, DEVS*> lCC_modelMap;

      // DIGRAPH info
      std::vector<DigraphCoupling> lC1_digraphCouplings;

      // loop over all the immediate child nodes
      BOOST_FOREACH( boost::property_tree::ptree::value_type const& rowPair,
		     aC_pt.get_child( "" ) ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "=> property "
		      << "<" << rowPair.first << ">="
		      << "<" << rowPair.second.get_value<std::string>() << ">");
	boost::optional<std::string> lC_propOpt =
	  aC_pt.get_optional<std::string>(rowPair.first);
	if (!lC_propOpt) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"property access failed!");
	  continue;
	}
	
	// parse info from this node
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "property <" << rowPair.first << "> access ok!");

	// if this has a leaf (an attribute)
	if (rowPair.second.empty() && !rowPair.second.data().empty() ) {
	  std::string lC_value = rowPair.second.get_value<std::string>();
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"This is a leaf (attribute) value = <"
			<< lC_value << ">");
	  if (rowPair.first == std::string("baseType") ) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
	  		  "Found <baseType>");
	    lb_isa_model =
	      (lC_value == std::string("efscape::impl::DEVS") ); 
	  }
	  else if (rowPair.first == std::string("type") ) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found <type>");
	    lC_modelType = lC_value;
	  }
	  else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Attribute <" << rowPair.first << "> ignored");
	  }
	}
	// else if this has a node (object)
	else if (!rowPair.second.empty() && rowPair.second.data().empty()) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"This is a node (object)");
	  
	  if (rowPair.first == std::string("wrappedModel") ) { // 1) wrappedModel
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Found <wrappedModel>");
	      lCp_wrappedModel = createModelFromJSON(rowPair.second);
	      if (lCp_wrappedModel) {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Successfully created <wrappedModel>!");
		
	      }
	      else {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Unable to create <wrappedModel>");
	      }
	  }
	  else if (rowPair.first == std::string("models") ) { // 2) collection of models
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found a set of models (for a network)");
	    BOOST_FOREACH( boost::property_tree::ptree::value_type const& modelPair, rowPair.second.get_child("") ) {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "=> model property "
			    << "<" << modelPair.first << ">="
			    << "<" << modelPair.second.get_value<std::string>() << ">");
	      DEVS* lCp_subModel = createModelFromJSON(modelPair.second);
	      if (lCp_subModel) {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Created submodel");
		// inject properties
		adevs::Bag<efscape::impl::IO_Type> xb;
		efscape::impl::IO_Type e;

		e.port = "properties_in";
		e.value = modelPair.second;
		xb.insert(e);

		inject_events(0.0, xb, lCp_subModel);

		lCC_modelMap[modelPair.first] = lCp_subModel;
	      }
	      else {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Unable to create submodel");
	      }
	    }
	  }
	  else if (rowPair.first == std::string("couplings") ) { // 3) digraph couplings
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found an array of couplings (for a digraph)");

	    BOOST_FOREACH( boost::property_tree::ptree::value_type const& couplingPair, rowPair.second.get_child("") ) {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "=> coupling property "
			    << "<" << couplingPair.first << ">="
			    << "<" << couplingPair.second.get_value<std::string>() << ">");
	      DigraphCoupling lC_coupling;
	      std::ostringstream lC_pair_out;
	      boost::property_tree::write_json( lC_pair_out,
						couplingPair.second );
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "coupling=" << lC_pair_out.str());
	      picojson::convert::from_string(lC_pair_out.str(),
					     lC_coupling);
	      lC1_digraphCouplings.push_back(lC_coupling);
	    }
	  }
	  else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Object ignored");
	  }
	}
	else {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Neither a proper leaf or proper node");
	}
      }	// ...aC_getChild("")...

      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "modelType=<" << lC_modelType << ">");

      // now build the model
      if (lb_isa_model) {
	if ( (lCp_model = createModel(lC_modelType.c_str()) )
	     != NULL ) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Successfully created the model!");

	  ModelWrapperBase* lCp_modelWrapper =
	    dynamic_cast<ModelWrapperBase*>(lCp_model);
	  NetworkModel* lCp_networkModel = lCp_model->typeIsNetwork();

	  if (lCp_networkModel) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is a NetworkModel");
	    DIGRAPH* lCp_digraph =
	      dynamic_cast<DIGRAPH*>(lCp_networkModel);
	    if (lCp_digraph) {
	      // add model components
	      for (std::map<std::string,DEVS*>::iterator it = lCC_modelMap.begin();
		   it != lCC_modelMap.end(); it++ ) {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Adding model <"
			      << it->first << ">...");
		lCp_digraph->add(it->second);
	      }	// for (std::map<...
	      
	      // add model couplings
	      lCC_modelMap["this"] = lCp_model; // add the parent
	      for (std::vector<DigraphCoupling>::iterator
		     it = lC1_digraphCouplings.begin();
		   it != lC1_digraphCouplings.end(); it++) {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Adding model coupling ("
			      << it->from.model << "," << it->from.port << ")"
			      << "=>("
			      << it->to.model << "," << it->to.port << ")");
		// the component models should exist -- not checking
		if (lCC_modelMap.find(it->to.model) == lCC_modelMap.end() ) {
		  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
				"model <" << it->to.model << "> not found");
		  continue;
		}
		DEVS* fromModel = lCC_modelMap[it->from.model];
		
		if (lCC_modelMap.find(it->from.model) == lCC_modelMap.end()) {
		  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
				 "model <" << it->from.model << "> not found");
		  continue;
		}
		DEVS* toModel = lCC_modelMap[it->to.model];

		lCp_digraph->couple(fromModel, it->from.port,
				    toModel, it->to.port);
	      }
	    }	// if (lCp_digraph)
	  }	// if (lCp_networkModel)
	  else {		// This is an ATOMIC model
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is an ATOMIC model");
	    if (lCp_modelWrapper) { // if this is a ModelWrapper
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "This is a ModelWrapper");
	      if (lCp_wrappedModel) { // if the wrappedModel exists
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "Adding the wrappedModel...");
		lCp_modelWrapper->setWrappedModel(lCp_wrappedModel);
	      }
	      else {
		LOG4CXX_ERROR(ModelHomeI::getLogger(),
			      "The wrappedModel is missing!");
	      }
	    }

	    // inject input (model properties file)
	    // note: must be done before initializing the simulator
	    adevs::Bag<efscape::impl::IO_Type> xb;
	    efscape::impl::IO_Type e;
	    e.port = "properties_in";
	    e.value = boost::property_tree::ptree(aC_pt);
	    xb.insert(e);

	    inject_events(-0.1, xb, lCp_model);
	    
	  } // else [is an ATOMIC model]

	} // if [this model is not NULL]
	else {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"Model <" << lC_modelType << "> not found!");
	}

      }	// if (lb_isa_model)
      else {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Missing <modelType>!")
      }

      return lCp_model;
      
    }	// createModelFromJSON(const boost::property_tree::ptree&)

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

  } // namespace impl

} // namespace efscape
