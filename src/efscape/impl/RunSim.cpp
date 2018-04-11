// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RunSim.cpp
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

#include <efscape/impl/RunSim.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

// #include <efscape/impl/AdevsModel.hpp>
#include <efscape/impl/SimRunner.hpp>

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
	// set logging level
	if (debug_on()) {
	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
	}
	else {
	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getError());
	}

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Loading libraries");
	Singleton<ModelHomeI>::Instance().LoadLibraries();

	// processing argument (should be only 1 input file)
	std::shared_ptr<DEVS> lCp_model;

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
	    lCp_model = DEVSPtr( loadAdevsFromXML(lC_ParmName.c_str()) );
	} else if (p.extension().string() == ".json") {
	  // try to load the parameter file
	  std::ifstream parmFile(lC_ParmName.c_str());

	  // if file can be opened
	  if ( parmFile ) {
	    std::ostringstream buf;
	    char ch;
	    while (buf && parmFile.get( ch ))
	      buf.put( ch );
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  buf.str() );

	    lCp_model =
	      Singleton<ModelHomeI>::Instance().
	      createModelFromJSON(buf.str());
	  }
	}

	if (lCp_model == nullptr) {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"Unable to create model from parameter file <"
			<< lC_ParmName << ">");
	  return EXIT_FAILURE;
	}

      	// create simulator
      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Creating simulator...");
      	adevs::Simulator<IO_Type> lCp_simulator(lCp_model.get() );

      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Attempt to create simulation model successful!"
      		      << "...Initializing simulation...");

      // 	if (!initializeModel(lCp_model.get()) ) // initialize model
      // 	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			// "Unable to initialize model using InitObject interface...");
      //
      // 	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      // 		      "Running simulation...");

	// initialize the simulation clock
	double ld_timeMax = adevs_inf<double>();
	ClockIPtr lCp_clock;
	// AdevsModel* lCp_RootModel = // note: root model derived from model
	//   dynamic_cast<AdevsModel*>(lCp_model.get());
	SimRunner* lCp_SimRunner = // note: alternative root model
	  dynamic_cast<SimRunner*>(lCp_model.get());

	// if (lCp_RootModel) {
	//   lCp_clock = lCp_RootModel->getClockIPtr();
	// }
	/*else*/ if (lCp_SimRunner) {
	  lCp_clock = lCp_SimRunner->getClockIPtr();
	}

	if (lCp_clock.get() != NULL) {
	  ld_timeMax = lCp_clock->timeMax();
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Simulator clock set for time interval ["
			<< lCp_clock->time() << ","
			<< lCp_clock->timeMax() << "], time delta = "
			<< lCp_clock->timeDelta() << ", units = "
			<< lCp_clock->units() << ", time units = "
			<< lCp_clock->timeUnits());

      	}

      	// simulate model until time max
	double ld_time = 0.;
      	while ( (ld_time = lCp_simulator.nextEventTime())
      		<= ld_timeMax ) {
      	  lCp_simulator.execNextEvent();
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

      if (files() < 1 || files() > 2) {
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
