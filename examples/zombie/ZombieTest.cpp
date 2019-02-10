// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include "ZombieTest.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <repast_hpc/RepastProcess.h>
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

      //------------------------------------------------------------------------
      // 1) Retrieve the model default parameters
      //------------------------------------------------------------------------
      std::shared_ptr<efscape::impl::DEVS> lCp_model;
      std::string lC_className = "efscape::impl::RelogoWrapper<ZombieObserver, repast::relogo::Patch>";
 
     Json::Value lC_info =
	efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
	getModelFactory().getProperties(lC_className);

      std::ostringstream lC_buffer;
      lC_buffer << lC_info;

      //------------------------------------------------------------------------
      // 2) Retrieve the model with parameters
      //------------------------------------------------------------------------
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
 
      //------------------------------------------------------------------------
      // 3. Run the simulation model
      //----------------------------------------------------------------------
      // initialize the model first
      adevs::Bag<efscape::impl::IO_Type> xb;
      efscape::impl::IO_Type x("setup_in",
			       "");
      xb.insert(x);
      efscape::impl::inject_events(0., xb, lCp_model.get());

      // create simulator
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Creating simulator...");
	
      adevs::Simulator<efscape::impl::IO_Type> lCp_simulator(lCp_model.get() );

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Attempt to create simulation model successful!"
		    << "...Initializing simulation...");

      // initialize the simulation clock
      double ld_timeMax = adevs_inf<double>();
   	
      //
      // simulate model until time max
      //
	
      // select an output stream for simulation output 
      std::streambuf * buf;
      std::ofstream of;
      if (out_file() != "") {
	of.open(out_file());
	buf = of.rdbuf();
      } else {
	buf = std::cout.rdbuf();
      }
      std::ostream lC_out(buf);
	
      double ld_time = 0.;
      while ( (ld_time = lCp_simulator.nextEventTime())
	      < ld_timeMax ) {
	lCp_simulator.execNextEvent();

	// 
	adevs::Bag<efscape::impl::IO_Type> xb;
	efscape::impl::get_output(xb, lCp_model.get());
	// adevs::Bag<IO_Type>::iterator i = xb.begin();
	for (auto i : xb) {
	  try {
	    Json::Value lC_messages =
	      boost::any_cast<Json::Value>( i.value );
	    lC_out << lC_messages << std::endl;
	  }
	  catch(const boost::bad_any_cast &) {
	    ;
	  }
	}
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
