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
      mC_ModelName("") {}

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
      	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
      		      "Loading libraries");
      	efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().LoadLibraries();

      	// check model name
      	if (mC_ModelName == "") {
      	  std::cout << "Missing model name!\n";
	  mC_ModelName = "gpt::gpt_coupled_model";
      	  // return EXIT_FAILURE;
      	}
	
      	// load model
      	std::unique_ptr<efscape::impl::DEVS> lCp_model;
      
      	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
      		      "Retrieving the model from the factory");
      	lCp_model.reset( efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModel( mC_ModelName.c_str() ) );

      	if (lCp_model.get() == 0) {
      	  std::string lC_message = "Unable to create model <" + mC_ModelName
      	    + ">";
      	  throw std::logic_error(lC_message.c_str());
      	}

      	// run model
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Running simulation of model...\n");
      	efscape::impl::runSim(lCp_model.get());
 
      }
      catch(std::logic_error lC_excp) {
      	std::cerr
      	  << "Exception encounted during attempt to load model libraries: <"
      	  << lC_excp.what() << ">\n";
      	return EXIT_FAILURE;
      }

      std::cout << "Hello World!\n";

      return li_status;
    }

    int BuildSim::parse_options(int argc, char *argv[])
    {
      mC_extended_description.add_options()
	("ModelName", po::value<std::string>(), "model name");

      int li_status =
	efscape::utils::CommandOpt::parse_options(argc,argv);	// parent method
      if (li_status != 0) {
	std::cout << "Illegal options!\n";
	return li_status;
      }


      if (mC_variable_map.count("ModelName")) {
	mC_ModelName = mC_variable_map["ModelName"].as<std::string>();
	std::cout << "model name = <" << mC_ModelName << ">\n";	
      }

      std::cout << "BuildSim::parse_options(...)\n";
      
      return li_status;
    }

    void BuildSim::usage(int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << " param_name\n\t\t"
		<< program_name() << " -d -o output_name param_name\n\n";
 
      exit( exit_value );
    }

  } // namespace impl
} // namespace efscape
