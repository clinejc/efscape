// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include "ZombieTest.hpp"

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

namespace zombie {
 // class variables
  const char* ZombieTest::mScp_program_name = "zombie_test";
  const char* ZombieTest::mScp_program_version =
    "version 0.0.1 (2019-01-29)";

  ZombieTest::ZombieTest() :
    efscape::utils::CommandOpt() {}

  const char* ZombieTest::program_name() {
    return ZombieTest::mScp_program_name;
  }

  const char* ZombieTest::ProgramName() {
    return ZombieTest::mScp_program_name;
  }

  const char* ZombieTest::program_version() {
    return ZombieTest::mScp_program_version;
  }

  int ZombieTest::execute()
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

     // 1) Retrieve the model default parameters
      std::shared_ptr<efscape::impl::DEVS> lCp_model;
      std::string lC_className = "efscape::impl::RelogoWrapper<ZombieObserver, repast::relogo::Patch>";
 
      // 3) Retrieve the model with parameters
      Json::Value lC_info =
	efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
	getModelFactory().getProperties(lC_className);

      std::ostringstream lC_buffer;
      lC_buffer << lC_info;

      lCp_model =
	efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
	createModelFromParameters(lC_buffer.str());
  
      if (lCp_model == nullptr) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Unable to retrieve a model object of type <"
		      <<  lC_className << ">!");
	return EXIT_FAILURE;
      }

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Attempt to create an object of type<"
		    << lC_className << "> with parameters was successful!");

      // Write todays date to a string
      date today = day_clock::local_day();
      date_facet* f = new date_facet("%Y-%m-%d");
      locale loc = locale(locale("en_US"), f);
      lC_buffer.clear();
      lC_buffer.str("");
      lC_buffer.seekp(0);
      lC_buffer.imbue(loc);
      lC_buffer << today;

      fs::path lC_path =
	fs::path(efscape::impl::ModelHomeI::getHomeDir()) /
	fs::path("sessions") /
	fs::path(lC_buffer.str());

      std::cout << lC_path.string() << std::endl;

      if (!fs::exists(lC_path)) {
	try {
	  fs::create_directory(lC_path);
	}
	catch(exception& e) {
	  std::cerr << e.what() << std::endl;
	}
      }

      if (chdir(lC_path.string().c_str())) {
	std::cout << "Changed directory" << std::endl;
      } else {
	std::cout << "chdir failed\n";
      }
  
      // initialize model
      adevs::Bag<efscape::impl::IO_Type> xb;
      efscape::impl::IO_Type x("setup_in",
			       0);
      xb.insert(x);
      efscape::impl::inject_events(0., xb, lCp_model.get());

      // test ta()
      efscape::impl::ATOMIC* lCp_atomic = lCp_model->typeIsAtomic();

      if (lCp_atomic != nullptr) {
	std::cout << "ta() = " << lCp_atomic->ta() << std::endl;
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

  int ZombieTest::parse_options(int argc, char *argv[]) {
    // parent method
    int li_status = efscape::utils::CommandOpt::parse_options(argc,argv);

    // Set-up mpi environment
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    return li_status;
  }

  void ZombieTest::usage(int exit_value )
  {
  }

  //---------------
  // register class
  //---------------
 const bool registered =
		 efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().getCommandFactory().
	   registerType<ZombieTest>( ZombieTest::ProgramName() );

}
