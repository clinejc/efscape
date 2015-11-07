// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelFactory.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelFactory.hpp>

#include <efscape/impl/ModelFactory.ipp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <iostream>

namespace efscape {

  namespace impl {

    // utility function for loading info from a JSON file
    std::string loadInfoFromJSON(const char* acp_path) {
      // path is relative
      std::string lC_FileName =
	ModelHomeI::getHomeDir() + std::string("/") + acp_path;

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

      return lC_JsonStr;
    }
    
    //
    // class ModelFactory implementation
    //
    /** constructor */
    ModelFactory::ModelFactory() {
      if ( ModelHomeI::getLogger() != NULL)
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Creating ModelFactory...");
    }

    /** destructor */
    ModelFactory::~ModelFactory() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Shutting down ModelFactory...");
    }

    //
    // class CellModelFactory implementation
    //
    /** constructor */
    CellModelFactory::CellModelFactory() {
      if ( ModelHomeI::getLogger() != NULL)
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Creating CellModelFactory...");
    }

    /** destructor */
    CellModelFactory::~CellModelFactory() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Shutting down CellModelFactory...");
    }

  } // namespace impl

} // namespace efscape
