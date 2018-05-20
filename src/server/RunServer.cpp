// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RunServer.cpp
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

#include "RunServer.hpp"

#include <Ice/Ice.h> // need Ice definitions

// ModelHome servant definition
#include "ModelHomeI.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

namespace efscape {

  namespace server {

    // class variables
    const char* RunServer::mScp_program_name = "efserver";
    const char* RunServer::mScp_program_version =
      "version 1.1.0 (2018/05/12)";

    /** default constructor */
    RunServer::RunServer() {
    }

    /** destructor */
    RunServer::~RunServer() {}

    /**
     * Returns the program name
     *
     * @returns the program name
     */
    const char* RunServer::program_name() {
      return RunServer::mScp_program_name;
    }

    /**
     * Returns the program name (class version)
     *
     * @returns the program name
     */
    const char* RunServer::ProgramName() {
      return RunServer::mScp_program_name;
    }

    /**
     * Returns the program version.
     *
     * @returns the program version
     */
    const char* RunServer::program_version() {
      return RunServer::mScp_program_version;
    }

    /**
     * Executes the RunServer command
     *
     * @returns exit state
     */
    int RunServer::execute() {

      //----------------------------------------------
      // create a root model from the model repository
      //----------------------------------------------
      try {

	if (debug_on()) {
	  // note (2008.06.05):
	  // The default debug setting is not working.
	  impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
	}
	else {
	  impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getError());
	}

	LOG4CXX_DEBUG(impl::ModelHomeI::getLogger(),
		      "Loading libraries");
	impl::Singleton<impl::ModelHomeI>::Instance().LoadLibraries();

	//----------------------
	// launch the ICE server
	//----------------------
	// attempt to retrieve the root directory of efscape ICE configuration
	std::string lC_EfscapeIcePath = "."; // default location
	char* lcp_env_variable =	// get EFSCAPE_HOME
	  getenv("EFSCAPE_HOME");

	if ( lcp_env_variable != 0 )
	  lC_EfscapeIcePath = lcp_env_variable;

	std::string lC_ice_config = lC_EfscapeIcePath
	  + "/config.server";
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "efscape ICE server config=<" << lC_ice_config << ">");

	// provide command-line arguments
	int argc = 1;
	char** argv = new char*[argc*sizeof(char*)];
	argv[0] = new char[std::string(program_name()).size()+1];
	strcpy(argv[0], program_name());

	this->main(argc, argv, lC_ice_config.c_str());

	// delete argv (no longer needed)
	for (int i = 0; i < argc; i++) {
	  delete [] argv[i];
	}
	delete [] argv;

      }
      catch(std::logic_error lC_excp) {
      	LOG4CXX_ERROR(impl::ModelHomeI::getLogger(),
      		      lC_excp.what());
      	return EXIT_FAILURE;
      }
      catch(std::exception& lC_excp) {
      	LOG4CXX_ERROR(impl::ModelHomeI::getLogger(),
      		      lC_excp.what());
      	return EXIT_FAILURE;
      }
      catch (...) {
      	LOG4CXX_ERROR(impl::ModelHomeI::getLogger(),
      		      "Error occurred parsing argument <"
      		      << (*this)[0] << ">");
      }
      return EXIT_SUCCESS;

    } // RunServer::execute()

    /**
     * Parses the command line arguements and initializes the command
     * configuration.
     *
     * @param argc number of command line arguments
     * @param argv vector of command line arguments
     * @returns exit status
     */
    int RunServer::parse_options(int argc, char *argv[]) {

      int li_status = CommandOpt::parse_options(argc,argv);	// parent method
      if (li_status != 0)
	return li_status;

      if (files() > 1) {
	usage();
	return 1;
      }

      return li_status;
    }

    /**
     * Prints out usage message for this command/program
     *
     * @args  exit_value exit value
     */
    void RunServer::usage( int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "[-d] [-h] [-v] \n\t"
		<< "[[-i] input_files]\n\t"
		<< "[-o output_file]\n\t"
		<< "[class_name] param_name\n\n"
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << "\n\t\t"
		<< program_name() << " -d -o logfile_name\n\n";

      exit( exit_value );
    }

    /**
     * Implements the run method for ModelHomeServer.
     *
     * @param argc argument count
     * @param argv argument vector
     */
    int RunServer::run(int argc, char* argv[])
    {
      auto adapter = communicator()->createObjectAdapter("ModelHome");
  auto servant = make_shared<ModelHomeI>();
  adapter->add(servant, Ice::stringToIdentity("ModelHome"));
  adapter->activate();
  communicator()->waitForShutdown();

      return EXIT_SUCCESS;
    }

    //================================
    // end of RunServer implementation
    //================================

    const bool registered =
		   impl::Singleton<impl::ModelHomeI>::Instance().getCommandFactory().
		   registerType<RunServer>( RunServer::ProgramName() );

  } // namespace server

}   // namespace efscape