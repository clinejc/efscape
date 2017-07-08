// __COPYRIGHT_START__
// Package Name : efscape
// File Name : driver.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
//=============================================================================
/***************************************************************************
 * @author Jon C. Cline <clinej@stanfordalumni.org>
 * @version 0.02 created 03 Feb 2008, revised 06 Jun 2008
 *
 * This program invokes a command that 
 *
 * ChangeLog:
 *   - 2014-09-01 Added singleton_terminate() function to handle exit
 *   - 2008.02.03 created driver
 ***************************************************************************/

#include <efscape/utils/CommandOpt.hpp>
#include <map>
#include <string>
#include <log4cxx/propertyconfigurator.h>
#include <boost/filesystem.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <boost/mpi/environment.hpp>

void fnExit(void) {
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		"Exiting the application...");
}

/**
 * Catches exceptions to handle known on the exit.
 */
void singleton_terminate(void) {
  // static bool tried_throw = false;
  static int tried_throw = 0;

  try {
    // if (!tried_throw++) throw;
    if ( (tried_throw++) > 0) throw;

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "no active exception");
  }
  catch (const std::runtime_error& err) {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Caught runtime error: " << err.what() );
  }
  catch (...) {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "unknown exception occurred.");
  }
  exit(0);			// ensure a clean exit
}

namespace {
  static const bool SET_TERMINATE =
    std::set_terminate(singleton_terminate);
}

/**
 * main body of the program
 *
 * @argc argument count
 * @argv vector of arguments (including program name)
 */
int main(int argc, char** argv) {

  std::cout << "Start...\n";

  // initialize MPI environment (use of MPI option)
  boost::mpi::environment env(argc, argv);

  //---------------------------------------
  // extract the program name from the path
  //---------------------------------------
  std::string lC_program_name = boost::filesystem::path(argv[0]).filename().string();

  //-----------------------------
  // program name == command name
  //-----------------------------
  int li_option_status = 0;
  int li_cmd_status = 0;
  std::unique_ptr<efscape::utils::CommandOpt> lCp_command;

  // attempt to retrieve the root directory of efscape ICE configuration
  std::string lC_EfscapeIcePath = "."; // default location
  char* lcp_env_variable =	// get EFSCAPE_HOME
    getenv("EFSCAPE_HOME");

  if ( lcp_env_variable != 0 )
    lC_EfscapeIcePath = lcp_env_variable;

  std::string lC_logger_config = lC_EfscapeIcePath
    + "/log4j.properties";

  try {
    lCp_command.reset( efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().getCommandFactory().
		       createObject(lC_program_name) );
  }
  catch(...) {
    std::cerr << "*** error *** Problem creating command object\n";
  }

  if ( lCp_command.get() != 0 ) {	// if the command is found, execute it
    if (lCp_command->debug_on())
      std::cout << "Invoking command <" << argv[0] << ">...\n";

    li_option_status = lCp_command->parse_options(argc,argv); // parse command options

    if (  li_option_status == 0)
      li_cmd_status = lCp_command->execute(); // execute command
  }
  else {
    std::cerr << "Command <" << lC_program_name << "> not found\n";
    li_cmd_status = li_option_status;
  }

  return li_cmd_status;
}
