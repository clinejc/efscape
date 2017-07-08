// __COPYRIGHT_START__
// __COPYRIGHT_END__
// serialization implementation
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "CreateSimpleSim.hh"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/impl/SimRunner.hpp>
#include <efscape/impl/adevs_json.hpp>
#include <efscape/utils/type.hpp>
#include <simplesim/SimpleGenerator.hh>
#include <simplesim/SimpleObserver.hh>
#include <simplesim/BasicModel.hh>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <json/json.h>

#include <sstream>

using namespace boost::gregorian;
using namespace boost::posix_time;
namespace fs = boost::filesystem;

namespace simplesim {

  // class variables
  const char* CreateSimpleSim::mScp_program_name = "createSimpleSim";
  const char* CreateSimpleSim::mScp_program_version =
    "version 0.0.2 (2014-08-31)";

  /** default constructor */
  CreateSimpleSim::CreateSimpleSim() :
    efscape::impl::BuildModel()
  {
  }

  /** destructor */
  CreateSimpleSim::~CreateSimpleSim() {
  }

  /**
   * Returns the program name
   *
   * @returns the program name
   */
  const char* CreateSimpleSim::program_name() {
    return CreateSimpleSim::mScp_program_name;
  }

  /**
   * Returns the program name (class version)
   *
   * @returns the program name
   */
  const char* CreateSimpleSim::ProgramName() {
    return CreateSimpleSim::mScp_program_name;
  }

  /**
   * Returns the program version.
   *
   * @returns the program version
   */
  const char* CreateSimpleSim::program_version() {
    return CreateSimpleSim::mScp_program_version;
  }

  /**
   * Executes the CreateSimpleSim command
   *
   * @returns exit state
   */
  int CreateSimpleSim::execute()
  {
    int li_status = 
      efscape::impl::BuildModel::execute(); // invoke parent method

    return li_status;

  } // CreateSimpleSim::execute()

  /**
   * Parses the command line arguements and initializes the command
   * configuration.
   *
   * @param argc number of command line arguments
   * @param argv vector of command line arguments
   * @returns exit status
   */
  int CreateSimpleSim::parse_options(int argc, char *argv[]) {

    md_TimeDelta = 1.0;
    md_TimeMax = 100.;

    int li_status = efscape::impl::BuildModel::parse_options(argc,argv);

    if (mC_Name == "")		// set default model name
      mC_Name = "simplesim";

    if (files() > 0) {
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
  void CreateSimpleSim::usage( int exit_value )
  {
    efscape::impl::BuildModel::usage(exit_value);
  }

  /**
   * Sets the model clock configuration (overrides parent method).
   */
  void CreateSimpleSim::setClock()
  {
    // set default time parameters
    mCp_ClockI->base_date( ptime( date(1996,Feb,1), hours(0) ) );
    mCp_ClockI->units(hours(24));
    mCp_ClockI->timeDelta() = 1.0;
    mCp_ClockI->timeMax() = 30.0;
  }

  /**
   * Creates a model based on the current command state.
   *
   * @throws std::logic_error
   */
  void CreateSimpleSim::createModel() 
    throw(std::logic_error)
  {
    std::string lC_message;

    Json::Value lC_config;

    //
    lC_config["sim"]["name"] = "simplesim";
    lC_config["sim"]["type"] = "simplesim";

    lC_config["ports"]["inputs"] = Json::Value(Json::objectValue);
    lC_config["ports"]["outputs"] = Json::Value(Json::objectValue);
    
    // set info
    lC_config["info"] = "Creates a simple simulation with a digraph containing a simple generator connected to a simple observer";

    // set root model type
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Specifying root model type...");

    lC_config["class"]["name"] = efscape::utils::type<efscape::impl::SimRunner>();

    // Specify time and add time object
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Simulation time units = " << mCp_ClockI->timeUnits() );

    lC_config["time"] = efscape::impl::convert_to_json(*mCp_ClockI);
    
    // create digraph
    efscape::impl::DigraphBuilder lC_digraphBuilder;
    Json::Value lC_modelConfig;
        
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Building a digraph...");
    
    // create generator
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating a generator and adding it to the digraph...");
    
    lC_modelConfig["class"]["name"] =
      efscape::utils::type<simplesim::SimpleGenerator>();

    lC_digraphBuilder.add("generator", lC_modelConfig);
    
    // create  observer
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating an observer and adding it to the digraph...");

    lC_modelConfig["class"]["name"] =
      efscape::utils::type<simplesim::SimpleObserver>();

    lC_digraphBuilder.add("observer", lC_modelConfig);

    // couple models
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Coupling the observer to the generator...");

   lC_digraphBuilder.coupling( "this", "clock_in",
			       "generator", "clock_in" );
   lC_digraphBuilder.coupling( "this", "property_in",
			       "generator", "property_in");
   lC_digraphBuilder.coupling("generator", "out",
			      "observer", "input");
   lC_digraphBuilder.coupling("observer", "output",
			      "this", "output");

   LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		 "Adding a digraph...");
 
   lC_config["wrappedModel"] = lC_digraphBuilder.convert_to_json();
        
    // save model configuration to a JSON file
    std::string lC_out_file = out_file();
    if (lC_out_file == "") {
      if (mC_Name != "")
	lC_out_file = mC_Name;
      else
	lC_out_file = mC_ClassName;

    lC_out_file += ".json";	// add file extension
    }
    else {
      fs::path lC_out_file_path =
	fs::path(lC_out_file.c_str() );
      lC_out_file =
	lC_out_file_path.parent_path().string() +
	lC_out_file_path.stem().string() + ".json";
    }

    std::ofstream lC_outfile(lC_out_file);
    lC_outfile << lC_config;

    efscape::impl::DEVS* lCp_devs =
      efscape::impl::createModelFromJSON(/*lC_pt*/lC_config);

    if (lCp_devs) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Successfully create a DEVS model"
		    << " from a JSON configuration file!");
    }
    else {
       LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		     "Unable to create a DEVS model"
		     << " from a JSON configuration file.");
    }

    mCp_model.reset( lCp_devs );
    
    // simulate model until infinity
    efscape::impl::runSim(lCp_devs);
    
    efscape::impl::BuildModel::createModel(); // invoke parent method

  } // CreateSimpleSim::createModel()

  //---------------
  // register class
  //---------------
  /**
   * Creates a new CreateSimpleSim.
   *
   * @returns a handle to a new CreateSimpleSim object
   */
  efscape::utils::CommandOpt* create_CreateSimpleSim() {
    return (efscape::utils::CommandOpt*)new CreateSimpleSim;
  }

  const bool registered =
		 efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().getCommandFactory().
		 registerType<CreateSimpleSim>( CreateSimpleSim::ProgramName() );
}
