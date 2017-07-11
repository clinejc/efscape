// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CreateGpt.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include "CreateGpt.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <iostream>

namespace gpt {

  // class variables
  const char* CreateGpt::mScp_program_name = "createGpt";
  const char* CreateGpt::mScp_program_version =
    "version 0.0.1 (2017-07-08)";

  CreateGpt::CreateGpt() :
    efscape::utils::CommandOpt() {}

  const char* CreateGpt::program_name() {
    return CreateGpt::mScp_program_name;
  }
  
  const char* CreateGpt::ProgramName() {
    return CreateGpt::mScp_program_name;
  }

  const char* CreateGpt::program_version() {
    return CreateGpt::mScp_program_version;
  }

  int CreateGpt::execute()
  {
    int li_status = 0;

    try {
      // set logging level
      if (debug_on()) {
	efscape::impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
      }
      else {
	efscape::impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getError());
      }

      // load libraries
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Loading libraries");
      efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().LoadLibraries();

      // load model
      std::unique_ptr<efscape::impl::DEVS> lCp_model;
      std::string lC_modelName = "gpt::gpt_coupled_model";
      
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Retrieving the model from the factory");
      lCp_model.reset( efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().createModel( lC_modelName.c_str() ) );

      if (lCp_model.get() == 0) {
	std::string lC_message = "Unable to create model <" + lC_modelName
	  + ">";
	throw std::logic_error(lC_message.c_str());
      }

      // run model
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

  int CreateGpt::parse_options(int argc, char *argv[]) {
    // parent method
    int li_status = efscape::utils::CommandOpt::parse_options(argc,argv);	

    return li_status;
  }

  void CreateGpt::usage(int exit_value )
  {
  }

  //---------------
  // register class
  //---------------
 const bool registered =
		 efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().getCommandFactory().
		 registerType<CreateGpt>( CreateGpt::ProgramName() );
  
} // namespace gpt
