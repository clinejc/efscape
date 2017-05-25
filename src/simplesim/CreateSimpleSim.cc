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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <sstream>

using namespace boost::gregorian;
using namespace boost::posix_time;

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

    bool lb_using_AdevsModel = false;
    if (lb_using_AdevsModel) {
      mCp_model.reset( new efscape::impl::AdevsModel );
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Using AdevsModel as root...");
    }
    else {
      mCp_model.reset( new efscape::impl::SimRunner );
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Using SimRunner as root...");

    }

    boost::property_tree::ptree lC_pt;
    boost::property_tree::ptree lC_child;

    // set type
    lC_pt.put("baseType", "efscape::impl::DEVS");
    lC_pt.put("type", "efscape::impl:SimRunner");

    // Specify time
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Simulation time units = " << mCp_ClockI->timeUnits() );
    efscape::impl::SimTime lC_simTime = {
      mCp_ClockI->timeDelta(),
      mCp_ClockI->timeMax(),
      mCp_ClockI->timeUnits()
    };
    std::string lC_time = picojson::convert::to_string(lC_simTime);
    std::istringstream lC_time_in(lC_time.c_str());
    boost::property_tree::read_json( lC_time_in, lC_child );

    // lC_child.put("units", "days since 1996-02-01T00:00:00");
    // lC_child.put("delta_t", 1);
    // lC_child.put("stopAt", 30);

    // add time object
    lC_pt.add_child("time", lC_child);
    
    // create digraph
    efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH;

    lC_pt.put("wrappedModel.baseType", "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.type", "efscape::impl:DIGRAPH");
    lC_pt.put("wrappedModel.name", "digraph");
    
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating digraph...");
    
    // create generator
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating a generator and adding it to the digraph...");
    simplesim::SimpleGenerator* lCp_generator =
      new simplesim::SimpleGenerator;
    lCp_digraph->add(lCp_generator);

    lC_pt.put("wrappedModel.models.generator.baseType",
	      "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.models.generator.type",
	      "simplesim::SimpleGenerator");
   
    // create  observer
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating an observer and adding it to the digraph...");
    simplesim::SimpleObserver* lCp_observer = new simplesim::SimpleObserver;
    lCp_digraph->add(lCp_observer);

    lC_pt.put("wrappedModel.models.observer.baseType",
	      "efscape::impl::DEVS");
    lC_pt.put("wrappedModel.models.observer.type",
	      "simplesim::SimpleObserver");

    // couple models
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Coupling the observer to the generator...");

    boost::property_tree::ptree lC_couplings;
    
    lCp_digraph->couple(lCp_digraph, "clock_in",
			lCp_generator, "clock_in");
    lC_child = boost::property_tree::ptree(); // reset  
    lC_child.put("from.model", "digraph");
    lC_child.put("from.port", "clock_in");
    lC_child.put("to.model", "generator");
    lC_child.put("to.port", "clock_in");
    lC_couplings.push_back(std::make_pair("", lC_child));

    lCp_digraph->couple(lCp_digraph, "properties_in",
			lCp_generator, "properties_in");
    lC_child = boost::property_tree::ptree(); // reset  
    lC_child.put("from.model", "digraph");
    lC_child.put("from.port", "property_in");
    lC_child.put("to.model", "generator");
    lC_child.put("to.port", "property_in");
    lC_couplings.push_back(std::make_pair("", lC_child));

    lCp_digraph->couple(lCp_generator,
    			simplesim::SimpleGenerator_strings::f_out,
    			lCp_observer,
    			simplesim::SimpleObserver_strings::m_input);
    lC_child = boost::property_tree::ptree(); // reset  
    lC_child.put("from.model", "generator");
    lC_child.put("from.port", "out");
    lC_child.put("to.model", "observer");
    lC_child.put("to.port", "input");
    lC_couplings.push_back(std::make_pair("", lC_child));

    lCp_digraph->couple(lCp_observer,
			"output",
			lCp_digraph,
			"output");
    lC_child = boost::property_tree::ptree(); // reset  
    lC_child.put("from.model", "observer");
    lC_child.put("from.port", "output");
    lC_child.put("to.model", "digraph");
    lC_child.put("to.port", "output");
    lC_couplings.push_back(std::make_pair("", lC_child));
    
    lC_pt.add_child("wrappedModel.couplings", lC_couplings);
    
    // write the properties out to JSON
    boost::property_tree::write_json( "test.json", lC_pt );

    efscape::impl::DEVS* lCp_devs =
      efscape::impl::createModelFromJSON(lC_pt);
    
    // add model
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Adding the digraph to the ModelWrapper...");

    // attempt to narrow the cast to a root model wrapper
    efscape::impl::AdevsModel* lCp_AdevsModel =
      dynamic_cast<efscape::impl::AdevsModel*>( mCp_model.get() );
    efscape::impl::SimRunner* lCp_SimRunner =
      dynamic_cast<efscape::impl::SimRunner*>( mCp_model.get() );

    if (lCp_AdevsModel != NULL) 
      lCp_AdevsModel->setWrappedModel(lCp_digraph);
    else if (lCp_SimRunner != NULL)
      lCp_SimRunner->/*efscape::impl::ModelWrapperBase::*/setWrappedModel(lCp_digraph);
    else
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Unable to attach SimpleSim model");

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Done creating the model!");

    efscape::impl::BuildModel::createModel(); // invoke parent method

    efscape::impl::DEVS* lCp_model =
      lCp_SimRunner->/*efscape::impl::ModelWrapperBase::*/getWrappedModel();
    if (lCp_model == NULL) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Added wrapped model missing!");
    }
    else {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Added wrapped model found!");
    }

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
