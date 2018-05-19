// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildSim.cpp
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

#include <efscape/impl/BuildSim.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <cereal/archives/json.hpp>

#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace po = boost::program_options;

namespace efscape {
  namespace impl {

    // class variables
    const char* BuildSim::mScp_program_name = "efbuilder";
    const char* BuildSim::mScp_program_version =
      "version 1.0.0 (2018-05-19)";

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

      	// check if model type name
      	if (mC_modelTypeName == "") {
	  std::cerr << "*** Error: modelTypeName not specified! ***"
		    << std::endl;
	  usage();

      	  return EXIT_FAILURE;
      	}
	std::set<std::string> lC1_ModelNames =
	  Singleton<ModelHomeI>::Instance().getModelFactory().getTypeIDs();
	if (lC1_ModelNames.find(mC_modelTypeName) == lC1_ModelNames.end()) {
	  std::cerr << "*** Error: modelTypeName <"
		    << mC_modelTypeName
		    << "> missing from model repository! ***"
		    << std::endl;
	  return EXIT_FAILURE;
	}

	// retrieve metadata
	Json::Value lC_ModelInfo =
	  Singleton<ModelHomeI>::Instance().getModelFactory().
	  getProperties(mC_modelTypeName.c_str());

	if (this->out_file() !="") {
	  std::ofstream lC_outFile(out_file().c_str());
	  lC_outFile << lC_ModelInfo << "\n";
	}
	else
	  std::cout << lC_ModelInfo << "\n";

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

      int li_status = EXIT_SUCCESS;
      try {
	li_status =
	  efscape::utils::CommandOpt::parse_options(argc,argv);	// parent method
      }
      catch(po::error e) {
	std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
      }

      if (mC_variable_map.count("modelTypeName")) {
	mC_modelTypeName = mC_variable_map["modelTypeName"].as<std::string>();
      }
      if (mC_variable_map.count("modelName")) {
	mC_modelName = mC_variable_map["modelName"].as<std::string>();
      }

      return li_status;
    }

    void BuildSim::usage(int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "where [] indicates optional option:\n\n"
		<< mC_description << "\n"
		<< "example:\n\t"
		<< program_name() << "[-d] --modelTypeName model_type_name [--modelName model_name] [-o output_name]\n\n"
		<< "note: input files are ignored\n\n";

      exit( exit_value );
    }

  } // namespace impl
} // namespace efscape
