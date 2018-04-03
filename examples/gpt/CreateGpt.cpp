// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CreateGpt.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include "CreateGpt.hpp"

#include "genr.hpp"
#include "proc.hpp"
#include "transd.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/utils/type.hpp>
#include <efscape/impl/adevs_json.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>

namespace fs = boost::filesystem;

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

      // build model using adevs JSON model builder functions
      // create digraph
      efscape::impl::DigraphBuilder lC_digraphBuilder;
      Json::Value lC_modelConfig;

      // create genr model
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
            "Creating a genr model and adding it to the digraph...");

      lC_modelConfig["modelTypeName"] =
	efscape::utils::type<gpt::genr>();
      lC_modelConfig["properties"]["genr_period"] = 1.0;

      lC_digraphBuilder.add("gnr", lC_modelConfig);

      // create  transd model
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Creating an transd model and adding it to the digraph...");

      lC_modelConfig = Json::Value();
      lC_modelConfig["modelTypeName"] =
	efscape::utils::type<gpt::transd>();
      lC_modelConfig["properties"]["observ_time"] = 10.0;

      lC_digraphBuilder.add("trnsd", lC_modelConfig);

      // create proc model
      lC_modelConfig = Json::Value();
      lC_modelConfig["modelTypeName"] =
	efscape::utils::type<gpt::proc>();
      lC_modelConfig["properties"]["processing_period"] = 2.0;

      lC_digraphBuilder.add("prc", lC_modelConfig);
      
      // couple models
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Coupling the observer to the generator...");


      lC_digraphBuilder.coupling("gnr", "out",
				 "trnsd", "arriv");
      lC_digraphBuilder.coupling("gnr", "out",
				 "prc", "in");
      lC_digraphBuilder.coupling("prc", "out",
				 "trnsd", "solved");
      lC_digraphBuilder.coupling("trnsd", "out",
				 "gnr", "stop");

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Adding a digraph...");

      Json::Value lC_config = lC_digraphBuilder.convert_to_json();

      // save two different versions of the model configuration respective JSON
      // files:
      //   1. <out_file>-builder.json: a JSON model builder specification
      //   2. <out_file>.json: a strict serialization of the model to JSON
      std::string lC_out_file = out_file();
      if (lC_out_file == "") {
        lC_out_file ="gpt";
      }

      // builder file
      fs::path lC_out_file_path =
        fs::path(lC_out_file.c_str() );
      fs::path lC_builder_file_path =
        lC_out_file_path.parent_path().string() +
        lC_out_file_path.stem().string() + "-builder.json";

      std::ofstream lC_builder_file(lC_builder_file_path.string());
      lC_builder_file << lC_config;

      // load model
      std::shared_ptr<efscape::impl::DEVS> lCp_model;
      lCp_model.reset( efscape::impl::buildModelFromJSON(lC_config) );

      // serialize model to snapshot file prior to run
      fs::path lC_archive_file_path =
        lC_out_file_path.parent_path().string() +
        lC_out_file_path.stem().string() + ".json";

      std::ofstream lC_archive_file(lC_archive_file_path.string());
      {
	efscape::impl::saveAdevsToJSON(lCp_model, lC_archive_file);
      }

      if (lCp_model == nullptr) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Unable to build model from JSON");
	return EXIT_FAILURE;
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
