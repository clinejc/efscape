// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/adevs_json.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

namespace efscape {

  namespace impl {

   /**
     * Creates a model from a model configuration stored in a JSON
     * string.
     *
     * @param aCr_JSONstring model configuration embedded in a JSON string
     * @returns handle to model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    createModelFromJSON(const std::string& aCr_JSONstring)
	throw(std::logic_error)
    {
      std::stringstream lC_OutStream;
      if (!lC_OutStream) {
	std::string lC_ErrorMsg = "Unable to open string stream";

	throw std::logic_error(lC_ErrorMsg.c_str());
      }

      // write buffer to the temporary file
      lC_OutStream << aCr_JSONstring;

      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		     "Received JSON string =>"
		     << aCr_JSONstring);

      // read in the JSON data via a property tree extracted from the temp file
      boost::property_tree::ptree pt;
      boost::property_tree::read_json( lC_OutStream, pt );

      return createModelFromJSON(pt);
      
    } // createModelFromJSON(const std::string&)

    // utility function for building a model from JSON
    DEVS* createModelFromJSON(const boost::property_tree::ptree& aC_pt) {
      // This function supports the following model type configurations:
      // 1. ModelWrapper (contains a "wrappedModel")
      // 2. NETWORK (contains a set of "models")
      //    a. DIGRAPH (contains an array of "couplings")
      // 3. ATOMIC
      //
      // It also supports the following non-model types
      // * ClockI ("time")
      // * arrays of scalars
      // * matrices
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
	}
	else {			// parse info from child node
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"property access ok!");

	  // check if this is a child node
	  if (!rowPair.second.empty() && rowPair.second.data().empty()) {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is a node");
	    // if this is a "time" object
	    if (rowPair.first == "time") {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Found time info");
	    }
	    // else if this is a wrappedModel
	    else if (rowPair.first == "wrappedModel") {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Found wrapped model");
	      DEVS* lCp_wrappedModel =
		createModelFromJSON(rowPair.second);
	    }
	  }
	  else {		// this should be a leaf
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "This is a leaf");
	  }
	}
      }
	
      return 0;
      
      std::string lC_modelName;
      boost::optional<std::string> lC_modelNameOpt =
	aC_pt.get_optional<std::string>("modelName");
      if (lC_modelNameOpt) {
	lC_modelName =
	  ((std::string)lC_modelNameOpt.get());
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "property <modelName> found");
      }
      else
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "property <modelName> not found!");

      adevs::Devs<IO_Type>* lCp_model = NULL;
      if ( (lCp_model = createModel(lC_modelName.c_str()) ) != NULL ) {
	// inject input (model properties file)
	// note: must be done before initializing the simulator
	adevs::Bag<efscape::impl::IO_Type> xb;
	efscape::impl::IO_Type e;
	e.port = "properties_in";
	e.value = aC_pt;
	xb.insert(e);

	inject_events(-0.1, xb, lCp_model);
      }

      return lCp_model;
      
      } // createModelFromJSON(boost::property_tree::ptree*)

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
