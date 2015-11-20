// __COPYRIGHT_START__
// Package Name : efscape
// File Name : BuildModel.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/BuildModel.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>

#include <efscape/impl/AdevsModel.hh>

#include <typeinfo>

namespace po = boost::program_options;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace efscape {

  namespace impl {

    // class variables
    const char* BuildModel::mScp_program_name = "efbuilder";
    const char* BuildModel::mScp_program_version =
      "version 0.0.3 (2014-08-31)";

    /** default constructor */
    BuildModel::BuildModel() :
      mCp_RootModel(0),
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

      if (mC_ClassName == "") {
	mC_ClassName = "efscape::impl::AdevsModel"; // default root model is of class efscape::impl::AdevsModel
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
	mCp_model.reset( Singleton<ModelHomeI>::Instance().CreateModel( mC_ClassName.c_str() ) );

	if (mCp_model.get() == 0) {
	  std::string lC_message = "Unable to create model <" + mC_ClassName
	    + ">";
	  throw std::logic_error(lC_message.c_str());
	}
      }
      else {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Root model has already been created");
      }

      EntityI* lCp_EntityI = dynamic_cast<EntityI*>( mCp_model.get() );
      if (mC_Name != "" && lCp_EntityI != 0) {
	lCp_EntityI->name( mC_Name.c_str() );
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Set model name to <" << mC_Name << ">...");
      }

      // attempt to narrow the cast to class <efscape::impl::AdevsModel>
      AdevsModel* lCp_RootModel = dynamic_cast<AdevsModel*>( lCp_EntityI );

      if (lCp_RootModel == 0) {
	std::string lC_message = "Unable to cast model <" + mC_ClassName
	  + "> as a <efscape::impl::AdevsModel>...";
	throw std::logic_error(lC_message.c_str());
      }

      mCp_RootModel = lCp_RootModel;

      // set clock and resource path if this is the root model
      if (lCp_RootModel) {
	lCp_RootModel->setClockIPtr(mCp_ClockI);
	if (mC_variable_map.count("ResourcePath"))
	  mC_ResourcePath = mC_variable_map["ResourcePath"].as<std::string>();
	lCp_RootModel->setResourcePath(mC_ResourcePath.c_str());
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
      if (debug_on())
	std::cout << "initializing model...\n";

      try {
	if (!efscape::impl::initializeModel(mCp_model.get() ) ) {
	  throw std::logic_error("model initialization failed");
	}

	// create a simulator and compute initialize model state
	boost::scoped_ptr<adevs::Simulator<IO_Type> > lCp_simulator;
	lCp_simulator.reset( new adevs::Simulator<IO_Type>(mCp_model.get() ) );

	// compute the initial state of the model
	while ( lCp_simulator->nextEventTime() == 0) {
	  lCp_simulator->execNextEvent();
	}

      }
      catch(std::logic_error lC_excp) {
	lC_message
	  += std::string
	  (": Exception encounted during attempt to initialize model <")
	  + "<" + mC_Name + ">: <"
	  + lC_excp.what() + ">\n";
	throw std::logic_error(lC_message.c_str());
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
	if (debug_on())
	  std::cout << "Saving model state to <" << lC_out_file << ">...\n";
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
