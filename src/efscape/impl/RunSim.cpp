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

// Include for handling JSON
#include <json/json.h>

#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

namespace efscape {

  namespace impl {

    // class variables
    const char* RunSim::mScp_program_name = "efdriver";
    const char* RunSim::mScp_program_version =
      "version 1.1.0 (2019/01/22)";

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

	// processing argument (should be at most 1 input file)
	std::string lC_parmName = "";

	//----------------------------------------------------------------------
	// 1. If no input file has been specified, display a list of all models
	//    currently loaded, and allow the user to select a model and output
	//    a default model parameter file.
	//----------------------------------------------------------------------
	if (files() == 0) {
	  // Get a list of model type ids
	  std::set<std::string> lC1_modelTypes =
	    Singleton<ModelHomeI>::Instance().getModelFactory().getTypeIDs();

	  // Show list of model ids and copy list to a vector
	  std::set<std::string>::iterator iter;
	  std::vector<std::string> lC1_modelList; // listing of model ids
	  std::cout << "*** List of available models: ***\n";
	  for (iter = lC1_modelTypes.begin(); iter != lC1_modelTypes.end();
	       iter++) {
	    lC1_modelList.push_back(*iter);
	    std::cout << lC1_modelList.size()
		      << ":"
		      << *iter << std::endl;
	  }

	  // menu options
	  std::cout << "Enter the number of the model (0 to exit)==> ";

	  // process user input
	  int li_userInput = 0;
	  std::string lC_parmString = "";
	  do {
	    std::cin >> li_userInput;
	    if (li_userInput > 0 && li_userInput <= lC1_modelTypes.size()) {
	      std::string lC_modelName = lC1_modelList[li_userInput - 1];
	      std::cout << "Selected model <"
			<< lC_modelName << ">\n";

	      lC_parmName =
		boost::replace_all_copy(lC_modelName,
					":",
					"_");
	      std::cout << "lC_parmName = <" << lC_parmName << ">\n";

	      // get model info
	      Json::Value lC_info =
		Singleton<ModelHomeI>::Instance().getModelFactory().
		getProperties(lC_modelName);
	      if ( lC_info.isMember("modelName") ) {
		lC_parmName = lC_info["modelName"].asString();
	      } else {
		lC_parmName =
		  efscape::impl::cplusTypeName2Posix(lC_modelName);
		lC_info["modelName"] = lC_parmName;
	      }
	      
	      std::ostringstream lC_buffer_out;
	      lC_buffer_out << lC_info;
	      lC_parmString = lC_buffer_out.str();
	      std::cout << lC_parmString << std::endl;

	    } else if ( li_userInput > lC1_modelTypes.size() ) {
	      std::cout << "Model index <" << li_userInput << "> out of bounds\n";
	      lC_parmName = "";
	      lC_parmString = "";
	    }
	  } while(li_userInput > 0);

	  if (lC_parmName != "") {
	    // save the model parameter JSON string to a file
	    lC_parmName += ".json";
	    std::ofstream parmFile(lC_parmName.c_str());
	    parmFile << lC_parmString << std::endl;
	  }
	  return EXIT_SUCCESS;
	}

	//----------------------------------------------------------------------
	// 2. At this point, there should be a single command line argment,
	//    which should be a valid parameter file name.
	//    Attempt to create model from the specified parameter file
	//----------------------------------------------------------------------
	std::shared_ptr<DEVS> lCp_model;
	Json::Value lC_info;
	lC_parmName = (*this)[0];
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Running with a single parameter <"
		      << lC_parmName << ">");
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Loading file <" << lC_parmName << ">");

	fs::path p =
	  fs::path(lC_parmName.c_str());

	//----------------------------------------------------------------------
	// 2a. If this the input file is in JSON format:
	//     1. First attempt to load the input as a JSON parameter file
	//     2. If the first attempt fails, attempt to load the input as a
	//        a cereal serialization of the model
	//----------------------------------------------------------------------
	if (p.extension().string() == ".json") {
	  // try to load the parameter file
	  std::ifstream parmFile(lC_parmName.c_str());

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
	      createModelFromParameters(buf.str());
	    
	    if (lCp_model == nullptr) {
	      LOG4CXX_ERROR(ModelHomeI::getLogger(),
			    "Attempt to load input file <"
			    << lC_parmName
			    << "> failed: "
			    << "attempting to deserialize cereal JSON input");
	      lCp_model =
		Singleton<ModelHomeI>::Instance().
		createModelFromJSON(buf.str());
	    }

	    // load parameter into a JSON value to be used later
	    std::istringstream lC_buffer_in(buf.str());
	    lC_buffer_in >> lC_info;
	  }
	}
	//----------------------------------------------------------------------
	// 2b. Otherwise, this should be am C++ xml serialization file
	//----------------------------------------------------------------------
	else if (p.extension().string() == ".xml") {
	    lCp_model = DEVSPtr( loadAdevsFromXML(lC_parmName.c_str()) );
	}

	if (lCp_model == nullptr) {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"Unable to create model from parameter file <"
			<< lC_parmName << ">");
	  return EXIT_FAILURE;
	}

	//----------------------------------------------------------------------
	// 3. Run the simulation model
	//----------------------------------------------------------------------
	// initialize the model first
	adevs::Bag<IO_Type> xb;
	IO_Type x("setup_in",
		  "");
	xb.insert(x);
	inject_events(0., xb, lCp_model.get());
	
	
      	// create simulator
      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Creating simulator...");
	
      	adevs::Simulator<IO_Type> lCp_simulator(lCp_model.get() );

      	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
      		      "Attempt to create simulation model successful!"
      		      << "...Initializing simulation...");

	// initialize the simulation clock
	double ld_timeMax = adevs_inf<double>();
	ClockIPtr lCp_clock;

	SimRunner* lCp_SimRunner = // note: alternative root model
	  dynamic_cast<SimRunner*>(lCp_model.get());

	if (lCp_SimRunner) {
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
	  adevs::Bag<IO_Type> xb;
	  get_output(xb, lCp_model.get());
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

	//
	// adevs::Bag<IO_Type> xb;
	get_output(xb, lCp_model.get());;
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
     * Parses the command line arguments and initializes the command
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

      if (files() > 2) {
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
		<< "[-d] [-h] [-v]\n\t"
		<< "[[-i] [input_file]]\n\t"
		<< "[-o output_file]\n\t"
		<< "param_name\n\n"
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << " param_name\n\t\t"
		<< program_name() << " -d -o output_name param_name\n\n"
		<< "If an input file is not specified, the user will be prompted"
		<< " to select one of the available models, from which a valid"
		<< " parameter file will be generated.\n";

      exit( exit_value );
    }

  } // namespace impl
}   // namespace efscape
