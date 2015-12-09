// __COPYRIGHT_START__
// __COPYRIGHT_END__
// serialization implementation
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "CreateSimpleSim.hh"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>

#include "efscape/impl/AdevsModel.hh"
#include "simplesim/SimpleGenerator.hh"
#include "simplesim/SimpleObserver.hh"
#include "simplesim/BasicModel.hh"

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

    mCp_model.reset( new efscape::impl::AdevsModel );

    efscape::impl::BuildModel::createModel(); // invoke parent method

    // create digraph
    efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH;

    std::cout << "Creating digraph...\n";

    // create generator
    simplesim::SimpleGenerator* lCp_generator =
      new simplesim::SimpleGenerator;
    lCp_digraph->add(lCp_generator);

    // create  observer
    simplesim::SimpleObserver* lCp_observer = new simplesim::SimpleObserver;
    lCp_digraph->add(lCp_observer);

    // couple models
    lCp_digraph->couple(lCp_generator,
			simplesim::SimpleGenerator_strings::f_out,
			lCp_observer,
			simplesim::SimpleObserver_strings::m_input);

    // add model
    mCp_RootModel->setWrappedModel(lCp_digraph);

    std::cout << "Done creating the model!\n";

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
