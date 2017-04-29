// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildModel.cc
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/BuildModel.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/impl/AdevsModel.hh>
#include <efscape/impl/SimRunner.hpp>

#include <typeinfo>

namespace po = boost::program_options;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace efscape {

  namespace impl {

    // class variables
    const char* BuildModel::mScp_program_name = "efbuilder";
    const char* BuildModel::mScp_program_version =
      "version 0.9.1 (2017-04-28)";

    /** default constructor */
    BuildModel::BuildModel() :
      md_TimeDelta(1.)
    {
    }

    /** destructor */
    BuildModel::~BuildModel() {
    }

    /**
     * Returns the program name
     *
     * @returns the program name
     */
    const char* BuildModel::program_name() {
      return BuildModel::mScp_program_name;
    }

    /**
     * Returns the program name (class version)
     *
     * @returns the program name
     */
    const char* BuildModel::ProgramName() {
      return BuildModel::mScp_program_name;
    }

    /**
     * Returns the program version.
     *
     * @returns the program version
     */
    const char* BuildModel::program_version() {
      return BuildModel::mScp_program_version;
    }

    /**
     * Executes the BuildModel command
     *
     * @returns exit state
     */
    int BuildModel::execute()
    {
      try {
	if (debug_on()) {
	  std::cout << "Setting logger level to Debug...\n";
	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());
	}
	else {
	  std::cout << "Setting logger level to Error...\n";
	  ModelHomeI::getLogger()->setLevel(log4cxx::Level::getError());
	}

	mCp_ClockI.reset(new ClockI);

	Singleton<ModelHomeI>::Instance().LoadLibraries();	
      }
      catch(std::logic_error lC_excp) {
	std::cerr
	  << "Exception encounted during attempt to load model libraries: <"
	  << lC_excp.what() << ">\n";
	return EXIT_FAILURE;
      }

      setClock();		// set time parameters

      if (mC_ClassName != "")	{// non-interactive mode
	try {
	  createModel();
	  saveConfig();
	}
	catch(std::logic_error lC_excp) {
	  std::cerr << "Exception encounted during attempt to create model <"
		    << "<" << mC_ClassName << ">: <"
		    << lC_excp.what() << ">\n";
	  return EXIT_FAILURE;
	}
	catch (...) {
	  std::cerr << "Exception encounted during attempt to create model <"
		    << "<" << mC_ClassName << ">\n";
	  return EXIT_FAILURE;
	}
      }
      else {			// interactive mode
	// prompt for input
      }

      return EXIT_SUCCESS;

    } // BuildModel::execute()

    /**
     * Parses the command line arguements and initializes the command
     * configuration.
     *
     * @param argc number of command line arguments
     * @param argv vector of command line arguments
     * @returns exit status
     */
    int BuildModel::parse_options(int argc, char *argv[]) {

      // set default class name for root model
      bool lb_using_AdevsModel = false;
      if (mC_ClassName == "") {
	if (lb_using_AdevsModel) {
	  mC_ClassName = "efscape::impl::AdevsModel"; // default root model is of class efscape::impl::AdevsModel
	}
	else {
	  mC_ClassName = "efscape::impl::SimRunner"; // default root model is of class efscape::impl::SimRunner
	}
      }

      mC_description.add_options()
	("ClassName", po::value<std::string>(&mC_ClassName)->default_value(mC_ClassName),
	 "root model class name")
	("Name", po::value<std::string>(&mC_Name)->default_value(mC_Name),"root model name")
	("ResourcePath", po::value<std::string>(&mC_ResourcePath)->default_value(mC_ResourcePath),"local file system path")
	("TimeDelta", po::value<double>(&md_TimeDelta)->default_value(md_TimeDelta),"simulation time step")
	("TimeMax", po::value<double>(&md_TimeMax)->default_value(md_TimeMax),"maximum simulation time")
	("StartDate", po::value<ptime>(&mC_StartDate)->default_value(mC_StartDate),"simulation start time")
	("Time Units", po::value<time_duration>(&mC_TimeUnits)->default_value(mC_TimeUnits),"simulation start time")
	;

      int li_status = CommandOpt::parse_options(argc,argv);	// parent method

      if (mC_variable_map.count("ClassName"))
	mC_ClassName = mC_variable_map["ClassName"].as<std::string>();
      if (mC_variable_map.count("Name"))
	mC_Name = mC_variable_map["Name"].as<std::string>();
      if (mC_variable_map.count("ResourcePath"))
	mC_ResourcePath = mC_variable_map["ResourcePath"].as<std::string>();

      return li_status;
    }

    /**
     * Prints out usage message for this command/program
     *
     * @args  exit_value exit value
     */
    void BuildModel::usage( int exit_value )
    {
      std::cerr << "usage:\n"
		<< program_name() << " "
		<< "where [] indicates optional option:\n\n"
		<< mC_description
		<< "examples:\n\t\t"
		<< program_name() << " param_name\n\t\t"
		<< program_name() << " -d -o output_name param_name\n\n";
 
      exit( exit_value );
    }

    /**
     * Sets the model clock configuration.
     */
    void BuildModel::setClock()
    {
      mCp_ClockI->timeDelta() = md_TimeDelta;
      mCp_ClockI->timeMax() = md_TimeMax;
      mCp_ClockI->base_date(mC_StartDate);
      mCp_ClockI->units(mC_TimeUnits);

      // Override with command-line parameters
      if (mC_variable_map.count("TimeDelta")) {
	md_TimeDelta = mC_variable_map["TimeDelta"].as<double>();
	mCp_ClockI->timeDelta() = md_TimeDelta;
      }

      if (mC_variable_map.count("TimeMax")) {
	md_TimeMax = mC_variable_map["TimeMax"].as<double>();
	mCp_ClockI->timeMax() = md_TimeMax;
      }

      if (mC_variable_map.count("StartDate")) {
	mC_StartDate = mC_variable_map["StartDate"].as<ptime>();
	mCp_ClockI->base_date(mC_StartDate);
      }

      if (mC_variable_map.count("TimeUnits")) {
	mC_TimeUnits = mC_variable_map["TimeUnits"].as<time_duration>();
	mCp_ClockI->units(mC_TimeUnits);
      }
    }

    /**
     * Creates a model based on the current command state.
     *
     * @throws std::logic_error
     */
    void BuildModel::createModel() 
      throw(std::logic_error)
    {
      if (mCp_model.get() == NULL) {

	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Retrieving the model from the factory");
	mCp_model.reset( Singleton<ModelHomeI>::Instance().createModel( mC_ClassName.c_str() ) );

	if (mCp_model.get() == 0) {
	  std::string lC_message = "Unable to create model <" + mC_ClassName
	    + ">";
	  throw std::logic_error(lC_message.c_str());
	}
      }
      else {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Root model has already been created");
      }

      EntityI* lCp_EntityI = dynamic_cast<EntityI*>( mCp_model.get() );
      if (mC_Name != "" && lCp_EntityI != 0) {
      	lCp_EntityI->name( mC_Name.c_str() );
      	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
      		      "Set model name to <" << mC_Name << ">...");
      }

      // attempt to narrow the cast to a root model wrapper
      AdevsModel* lCp_AdevsModel =
	dynamic_cast<AdevsModel*>( mCp_model.get() );
      SimRunner* lCp_SimRunner =
	dynamic_cast<SimRunner*>( mCp_model.get() );

      if (lCp_AdevsModel == NULL) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Root model <" << mC_ClassName
		      << "> is not derived from <efscape::impl::AdevsModel>...");
      }
      else if (lCp_SimRunner == NULL) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Root model <" << mC_ClassName
		      << "> is not derived from <efscape::impl::SimRunner>...");
      }
      
      // set clock and resource path if this is the root model
      if (lCp_AdevsModel) {
      	lCp_AdevsModel->setClockIPtr(mCp_ClockI);
      	if (mC_variable_map.count("ResourcePath"))
      	  mC_ResourcePath = mC_variable_map["ResourcePath"].as<std::string>();
      	lCp_AdevsModel->setResourcePath(mC_ResourcePath.c_str());
      }

    } // BuildModel::createModel()

    /**
     * Saves the model configuration.
     *
     * @throws std::logic_error
     */
    void BuildModel::saveConfig() 
      throw(std::logic_error)
    {
      std::string lC_message = "BuildModel::saveConfig()";

      if (mCp_model == 0) {
	lC_message += ": Unable to save null model <" + mC_ClassName
	  + ">";
	throw std::logic_error(lC_message.c_str());
      }

      // first initialize the model
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "initializing model...");

      // attempt to narrow the cast to a root model wrapper (deja vu?)
      AdevsModel* lCp_AdevsModel =
	dynamic_cast<AdevsModel*>( mCp_model.get() );

      // use this information to determine how to initialize the model
      if (lCp_AdevsModel != NULL) {
	if (!efscape::impl::initializeModel(mCp_model.get() ) ) {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"model initialization failed");
	}
      }
      else {
	// inject clock
	adevs::Bag<efscape::impl::IO_Type> xb;
	efscape::impl::IO_Type e;
	e.port = "clock_in";
	e.value = mCp_ClockI;
	xb.insert(e);

	// inject properties
	boost::property_tree::ptree lC_properties;
	lC_properties.put("ResourcePath", mC_ResourcePath);
	e.port = "properties_in";
	e.value = lC_properties;
	xb.insert(e);

	inject_events(-0.1, xb, mCp_model.get());
      }

      // create a simulator and compute initialize model state
      boost::scoped_ptr<adevs::Simulator<IO_Type> > lCp_simulator;
      lCp_simulator.reset( new adevs::Simulator<IO_Type>(mCp_model.get() ) );

      // compute the initial state of the model
      while ( lCp_simulator->nextEventTime() == 0) {
	lCp_simulator->execNextEvent();
      }

      // save model configuration to xml file
      std::string lC_out_file = out_file();
      if (lC_out_file == "") {
	if (mC_Name != "")
	  lC_out_file = mC_Name;
	else
	  lC_out_file = mC_ClassName;

	lC_out_file += ".xml";	// add file extension
      }

      try {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Saving model state to <" << lC_out_file << ">...");
	saveAdevs(mCp_model.get(),lC_out_file.c_str());
      }
      catch (std::exception& excp) {
	std::string lC_message = "Error encountered saving model <"
	  + mC_ClassName + "> to file <" + lC_out_file + ">: " + excp.what();
	throw std::logic_error(lC_message.c_str());
      }

    } // BuildModel::saveConfig()

  } // namespace impl

} // namespace efscape
