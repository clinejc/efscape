// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RunSim.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/RunSim.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>

#include <efscape/impl/AdevsModel.hh>

#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>

namespace efscape {

  namespace impl {

    // class variables
    const char* RunSim::mScp_program_name = "efdriver";
    const char* RunSim::mScp_program_version =
      "version 0.0.4 (2015/06/01)";

    /** default constructor */
    RunSim::RunSim() {
    }

    /** destructor */
    RunSim::~RunSim() {}

    /**
     * Returns the program name
     *
     * @returns the program name
     */
    const char* RunSim::program_name() {
      return RunSim::mScp_program_name;
    }

    /**
     * Returns the program name (class version)
     *
     * @returns the program name
     */
    const char* RunSim::ProgramName() {
      return RunSim::mScp_program_name;
    }

    /**
     * Returns the program version.
     *
     * @returns the program version
     */
    const char* RunSim::program_version() {
      return RunSim::mScp_program_version;
    }

    /**
     * Executes the RunSim command
     *
     * @returns exit state
     */
    int RunSim::execute() {

      //----------------------------------------------
      // create a root model from the model repository
      //----------------------------------------------
      try {
	ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
	if (mC_variable_map.count("debug")) {
	  LOG4CXX_INFO(ModelHomeI::getLogger(),
		       "debug is set");
	}
	else
	  LOG4CXX_INFO(ModelHomeI::getLogger(),
			"debug is not set");

	if (debug_on()) {
	  LOG4CXX_INFO(ModelHomeI::getLogger(),
		       "Setting logger to Debug...");

	  // note (2008.06.05):
	  // The default debug setting is not working.
	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
	}
	else {
	  LOG4CXX_INFO(ModelHomeI::getLogger(),
		       "logger level = INFO");
	}

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Loading libraries");
	Singleton<ModelHomeI>::Instance().LoadLibraries();	

	// processing argument
	boost::scoped_ptr<DEVS> lCp_model;
	if (files() == 1) {
	  std::string lC_ParmName = (*this)[0];
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Running with a single parameter <"
			<< lC_ParmName << ">");

	  // attempt to create model from parameter file
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Loading file <" << lC_ParmName << ">");

	  boost::filesystem::path p =
	    boost::filesystem::path(lC_ParmName.c_str());
	  if (p.extension().string() == ".xml") {
	    lCp_model.reset( Singleton<ModelHomeI>::Instance().
			     CreateModelFromXML(lC_ParmName.c_str()) );
	  }
	  else if (p.extension().string() == ".json") {
	    // try to load the parameter file
	    std::ifstream parmFile(lC_ParmName.c_str());

	    // if file can be opened
	    if ( parmFile ) {
	      std::ostringstream buf;
	      char ch;
	      while (buf && parmFile.get( ch ))
		buf.put( ch );
	      std::cout << buf << std::endl;

	      lCp_model.reset( Singleton<ModelHomeI>::Instance().
			       CreateModelFromJSON(buf.str()) );
	    }
	  }
	  else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Missing parameters: exiting now ...");
	    return EXIT_FAILURE;
	  }

	  if (lCp_model.get() == 0) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to create model from parameter file <"
			  << lC_ParmName << ">");
	    return EXIT_FAILURE;
	  }
	} // if (files() == 1)
	else {
	}

      	// create simulator
      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Creating simulator...");
      	boost::scoped_ptr<adevs::Simulator<IO_Type> > lCp_simulator;
      	lCp_simulator.reset( new adevs::Simulator<IO_Type>(lCp_model.get()) );

      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Attempt to create simulation model successful!"
      		      << "...Initializing simulation...");

      	if (!initializeModel(lCp_model.get()) ) // initialize model
      	  throw std::logic_error("Unable to initialize model\n");

      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Running simulation...");

	// initialize the simulation clock
	double ld_timeMax = DBL_MAX;
	ClockIPtr lCp_clock;
	AdevsModel* lCp_RootModel = // note: root model derived from model
	  dynamic_cast<AdevsModel*>(lCp_model.get());
	if (lCp_RootModel) {
	  lCp_clock = lCp_RootModel->getClockIPtr();
	  ld_timeMax = lCp_clock->timeMax();
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Simulator clock set for time interval ["
			<< lCp_clock->time() << ","
			<< lCp_clock->timeMax() << "], time delta = "
			<< lCp_clock->timeDelta() );

      	  // set-up observers
      	  std::set< efscape::impl::ObserverPtr > lCCp_observers;
      	  lCp_RootModel->getObservers(lCCp_observers);
      	  std::set< efscape::impl::ObserverPtr >::iterator iObserver;
      	  for (iObserver = lCCp_observers.begin();
	       iObserver != lCCp_observers.end(); iObserver++)
      	    lCp_simulator->addEventListener( (*iObserver).get() );
      	}

      	// simulate model until time max
	double ld_time = 0.;
      	while ( (ld_time = lCp_simulator->nextEventTime())
      		<= ld_timeMax ) {
      	  lCp_simulator->execNextEvent();
      	}
      }
      catch(std::logic_error lC_excp) {
      	LOG4CXX_ERROR(ModelHomeI::getLogger(),
      		      lC_excp.what());
      	return EXIT_FAILURE;
      }
      catch(std::exception& lC_excp) {
      	LOG4CXX_ERROR(ModelHomeI::getLogger(),
      		      lC_excp.what());
      	return EXIT_FAILURE;
      }
      catch (...) {
      	LOG4CXX_ERROR(ModelHomeI::getLogger(),
      		      "Error occurred parsing argument <"
      		      << (*this)[0] << ">");
      }
  
      return EXIT_SUCCESS;

    } // RunSim::execute()

    /**
     * Parses the command line arguements and initializes the command
     * configuration.
     *
     * @param argc number of command line arguments
     * @param argv vector of command line arguments
     * @returns exit status
     */
    int RunSim::parse_options(int argc, char *argv[]) {

      int li_status = CommandOpt::parse_options(argc,argv);	// parent method
      if (li_status != 0)
	return li_status;

      if (files() < 1 && files() > 2) {
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
    void RunSim::usage( int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "[-d] [-h] [-v] \n\t"
		<< "[[-i] input_files] (currently ignored)\n\t"
		<< "[-o output_file]\n\t"
		<< "param_name\n\n"
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << " param_name\n\t\t"
		<< program_name() << " -d -o output_name param_name\n\n";
 
      exit( exit_value );
    }

  } // namespace impl
}   // namespace efscape
