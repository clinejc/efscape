// __COPYRIGHT_START__
// Package Name : efscape
// File Name : driver.cpp
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
#include <boost/mpi/communicator.hpp>

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

  // initialize MPI environment (use of MPI option)
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;
  efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    setCommunicator(&world);

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
