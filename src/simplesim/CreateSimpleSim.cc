// __COPYRIGHT_START__
// __COPYRIGHT_END__
// serialization implementation
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "CreateSimpleSim.hh"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/impl/AdevsModel.hpp>
#include <efscape/impl/SimRunner.hpp>
#include <efscape/impl/adevs_json.hpp>
#include <simplesim/SimpleGenerator.hh>
#include <simplesim/SimpleObserver.hh>
#include <simplesim/BasicModel.hh>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

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

    boost::property_tree::ptree lC_pt;
    boost::property_tree::ptree lC_child;

    // set info
    lC_pt.put("info",
	      "Creates a simple simulation with a digraph containing a simple generator connected to a simple observer");
    
    // set type
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Specifying root model type...");
    lC_pt.put("baseType", "efscape::impl::DEVS");
    lC_pt.put("type", "efscape::impl::SimRunner");

    // Specify time
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Simulation time units = " << mCp_ClockI->timeUnits() );
    efscape::impl::SimTime lC_simTime = {
      mCp_ClockI->timeDelta(),
      mCp_ClockI->timeMax(),
      mCp_ClockI->timeUnits()
    };
    std::string lC_time = picojson::convert::to_string(lC_simTime);
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "time: " << lC_time);    

    std::istringstream lC_time_in(lC_time.c_str());
    boost::property_tree::read_json( lC_time_in, lC_child );


    // add time object
    lC_pt.add_child("time", lC_child);
    
    // create digraph
    lC_pt.put("wrappedModel.baseType", "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.type", "efscape::impl::DIGRAPH");
    
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Added a digraph...");
    
    // create generator
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating a generator and adding it to the digraph...");
    lC_pt.put("wrappedModel.models.generator.baseType",
	      "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.models.generator.type",
	      "simplesim::SimpleGenerator");
   
    // create  observer
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating an observer and adding it to the digraph...");
    lC_pt.put("wrappedModel.models.observer.baseType",
	      "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.models.observer.type",
	      "simplesim::SimpleObserver");

    // couple models
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Coupling the observer to the generator...");

    // create an array of couplings
   std::vector<efscape::impl::DigraphCoupling> lC1_couplings;

   lC1_couplings.push_back( efscape::impl::DigraphCoupling("this", "clock_in", "generator", "clock_in") );
   lC1_couplings.push_back( efscape::impl::DigraphCoupling("this", "property_in", "generator", "property_in") );
    lC1_couplings.push_back(efscape::impl::DigraphCoupling("generator", "out", "observer", "input"));
    lC1_couplings.push_back(efscape::impl::DigraphCoupling("observer", "output", "this", "output"));

    boost::property_tree::ptree lC_coupling_pt;
    std::string lC_couplingConfig = picojson::convert::to_string(lC1_couplings);
    std::istringstream lC_coupling_in(lC_couplingConfig.c_str());
    boost::property_tree::read_json( lC_coupling_in, lC_coupling_pt );
    
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "coupling=" << lC_couplingConfig);
    
    lC_pt.add_child("wrappedModel.couplings", lC_coupling_pt);
    
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
    boost::property_tree::write_json( lC_out_file, lC_pt );

    efscape::impl::DEVS* lCp_devs =
      efscape::impl::createModelFromJSON(lC_pt);

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
