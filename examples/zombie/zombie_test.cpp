// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // date_time definitions
using namespace boost::gregorian;

namespace fs = boost::filesystem;

/**
 * main
 *
 * @param argc number of arguments
 * @param argv vector of arguments
 */
int main(int argc, char** argv)
{
  int li_status = 0;

  // Set-up mpi environment
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  efscape::impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());

  try {
    // 1) Load model libraries
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Loading libraries");
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().LoadLibraries();

    // 2) Retrieve the "zombie_test" command
    std::unique_ptr<efscape::utils::CommandOpt> lCp_command;
    std::string lC_programName = "zombie_test";
    lCp_command.reset(efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    		    getCommandFactory().createObject(lC_programName) );

    if (lCp_command == nullptr) {
      LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
    		  "Command <"
    		  <<  lC_programName << "> not found!");
      return EXIT_FAILURE;
    }
    
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Command <"
		  << lC_programName
		  << "> successfully retrieved!");
    
    if ( (li_status = lCp_command->parse_options(argc, argv)) == 0 ) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
    		  "Invoking command <"
    		  << lC_programName
    		  << ">...");

      li_status = lCp_command->execute(); // execute command
    }
  }
  catch(std::logic_error lC_excp) {
    std::cerr
      << "Exception encounted during attempt to load model libraries: <"
      << lC_excp.what() << ">\n";
    return EXIT_FAILURE;
  }

  return li_status;
}
