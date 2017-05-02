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
#include <simplesim/SimpleGenerator.hh>
#include <simplesim/SimpleObserver.hh>
#include <simplesim/BasicModel.hh>

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
   
    // create digraph
    efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH;

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating digraph...");
    
    // create generator
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating a generator and adding it to the digraph...");
    simplesim::SimpleGenerator* lCp_generator =
      new simplesim::SimpleGenerator;
    lCp_digraph->add(lCp_generator);
   
    // create  observer
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Creating an observer and adding it to the digraph...");
    simplesim::SimpleObserver* lCp_observer = new simplesim::SimpleObserver;
    lCp_digraph->add(lCp_observer);

    // couple models
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Coupling the observer to the generator...");
    lCp_digraph->couple(lCp_digraph, "clock_in",
			lCp_generator, "clock_in");
    lCp_digraph->couple(lCp_digraph, "properties_in",
			lCp_generator, "properties_in");
    lCp_digraph->couple(lCp_generator,
    			simplesim::SimpleGenerator_strings::f_out,
    			lCp_observer,
    			simplesim::SimpleObserver_strings::m_input);
    lCp_digraph->couple(lCp_observer,
			simplesim::SimpleObserver_strings::f_output,
			lCp_digraph,
			simplesim::SimpleObserver_strings::f_output);
    
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
