// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildSim.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/BuildSim.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <cereal/archives/json.hpp>

#include <iostream>

namespace po = boost::program_options;

namespace efscape {
  namespace impl {

    // class variables
    const char* BuildSim::mScp_program_name = "efbuilder";
    const char* BuildSim::mScp_program_version =
      "version 0.0.1 (2017-07-08)";

    BuildSim::BuildSim() :
      efscape::utils::CommandOpt(),
      mC_modelName("") {}

    const char* BuildSim::program_name() {
      return BuildSim::mScp_program_name;
    }

    const char* BuildSim::ProgramName() {
      return BuildSim::mScp_program_name;
    }

    const char* BuildSim::program_version() {
      return BuildSim::mScp_program_version;
    }

    int BuildSim::execute()
    {
      int li_status = 0;

      try {
      	// set logging level
      	if (debug_on()) {
      	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
      	}
      	else {
      	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getError());
      	}

      	// load libraries
      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Loading libraries");
      	Singleton<ModelHomeI>::Instance().LoadLibraries();

      	// check model type name
      	if (mC_modelTypeName == "") {
	  std::cerr << "*** Error: modelTypeName not specified! ***"
		    << std::endl;
	  usage();

      	  return EXIT_FAILURE;
      	}

      	// attempt to load model
      	DEVSPtr lCp_model;

      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Retrieving the model from the factory");
        	lCp_model.reset(
  	  Singleton<ModelHomeI>::Instance()
  	  .getModelFactory()
  	  .createObject( mC_modelTypeName, Json::Value() ) );

      	if (lCp_model == nullptr) {
      	  std::string lC_message = "Unable to create model <" + mC_modelName
      	    + ">";
      	  throw std::logic_error(lC_message.c_str());
      	}

	// retrieve metadata


	// run model
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Running simulation of model...\n");
      	// efscape::impl::runSim(lCp_model.get());

      }
      catch(std::logic_error lC_excp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Exception encounted during attempt to load model libraries: <"
		      << lC_excp.what() << ">");
      	return EXIT_FAILURE;
      }

      return li_status;
    }

    int BuildSim::parse_options(int argc, char *argv[])
    {
      mC_extended_description.add_options()
	("modelTypeName", po::value<std::string>(), "model type name");

      mC_extended_description.add_options()
	("modelName", po::value<std::string>(), "model name");

      int li_status =
	efscape::utils::CommandOpt::parse_options(argc,argv);	// parent method
      if (li_status != 0) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Illegal options!");
	return li_status;
      }


      if (mC_variable_map.count("modelTypeName")) {
	mC_modelName = mC_variable_map["modelTypeName"].as<std::string>();
      }
      if (mC_variable_map.count("modelName")) {
	mC_modelName = mC_variable_map["modelName"].as<std::string>();
	std::cout << "model name = <" << mC_modelName << ">\n";
      }

      return li_status;
    }

    void BuildSim::usage(int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << " --modelTypeName model_type_name [--modelName model_name]\n\t\t"
		<< program_name() << " param_name\n\t\t"
		<< program_name() << " -d -o output_name param_name\n\n";

      exit( exit_value );
    }

  } // namespace impl
} // namespace efscape
