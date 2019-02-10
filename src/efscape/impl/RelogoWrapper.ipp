// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RelogoWrapper.ipp
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

#include <efscape/impl/RelogoWrapper.hpp> // class definition

// definitions for efscape models
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/utils/type.hpp>

// Repast HPC definitions
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/Properties.h>
#include <repast_hpc/logger.h>

// other definitions
#include <log4cxx/logger.h>

#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

namespace efscape
{
namespace impl
{

// instantiate class data members
template <typename ObserverType, typename PatchType>
const PortType RelogoWrapper<ObserverType, PatchType>::setup_in =
    "setup_in";

/**
     * default constructor
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::RelogoWrapper() :
  ATOMIC(),
  mC_modelProps(Json::nullValue)
{
} // RelogoWrapper<ObserverType, PatchType>::Relogo()

/**
     * constructor with argument in JSON format
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param aC_modelProps model properties in JSON format
      */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::RelogoWrapper(Json::Value aC_modelProps) :
  ATOMIC(),
  mC_modelProps(aC_modelProps)
{
} // RelogoWrapper<ObserverType, PatchType>::Relogo(Json::Value)

/**
     * destructor
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::~RelogoWrapper()
{
  std::string lC_id =
    utils::type<RelogoWrapper<ObserverType, PatchType>>(*this);
  
  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "Deleting RelogoWrapper=<"
                    << lC_id << ">...");
}
  
/**
     * Setup function.
     *
     * Initilizes the wrapped Repast HPC model
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param aC_propsFile Repast HPC config file name
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::setup(std::string aC_propsFile)
{

  std::string lC_id =
    utils::type<RelogoWrapper<ObserverType, PatchType>>(*this);
  
  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		lC_id << "::setup("
		<< aC_propsFile
		<< ")...");

  // 1) Get config file
  std::string lC_config_file = aC_propsFile;

  if ( lC_config_file == "" ||
       !fs::exists(fs::path(lC_config_file)) ) {
    // get the default config file
    // attempt to retrieve the root directory of efscape ICE configuration
    std::string lC_EfscapeIcePath = "."; // default location
    char *lcp_env_variable =             // get EFSCAPE_HOME
      getenv("EFSCAPE_HOME");

    if (lcp_env_variable != 0)
      lC_EfscapeIcePath = lcp_env_variable;

    lC_config_file = lC_EfscapeIcePath + std::string("/config.props");
  }
  
  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "===> 1) Attempting to load configuration file ***");

  // 2) Intialize RepastProcess
  boost::mpi::communicator* lCp_world =
    Singleton<ModelHomeI>::Instance().
    getCommunicator();
  repast::RepastProcess::init(lC_config_file,
			      lCp_world);

  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "===> 2) Initialized the RepastProcess!");

  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "===> Completed configuration of <"
                    << lC_id << ">");

  // 3) Copy JSON attributes to repast Properties
  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "===> 3) Copying JSON attributes to repast Properties");

  repast::Properties lC_props;
  if (!mC_modelProps.isObject())
  {
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                  "mC_modelProps is not an object:"
		  << " using default properties");
    
    // Retrieve default parameters
    Json::Value lC_info =
      Singleton<ModelHomeI>::Instance().
      getModelFactory().getProperties(lC_id);
    
    mC_modelProps = lC_info["properties"];
  }

  for (Json::Value::const_iterator it = mC_modelProps.begin();
       it != mC_modelProps.end();
       ++it) {
    std::string lC_key = it.key().asString();
    std::string lC_value = it->asString();
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		  "Setting property <"
		  << lC_key
		  << "> = <"
		  << lC_value
		  << ">");

    lC_props.putProperty(lC_key,
			 lC_value);
  }

  // 4) Create Repast model
  lCp_world =
    repast::RepastProcess::instance()->getCommunicator();
  mCp_model.reset(new repast::relogo::SimulationRunnerPlus<ObserverType, PatchType>(lCp_world));
  mCp_model->setup(lC_props); // initialize repast hpc model

  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "===> 4) Just reset and setup the wrapped model ***");
  
} // RelogoWrapper<ObserverType, PatchType>::setup(std::string)

//--------------------------------------------
// begin implementation of devs::adevs methods
//--------------------------------------------
/**
     * Internal transition function.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::delta_int()
{
  // execute next event
  repast::ScheduleRunner &runner =
    repast::RepastProcess::instance()->getScheduleRunner();
  runner.execNextEvent();
  if (!runner.isRunning()) // end simulation if done
    runner.end();
} // RelogoWrapper<ObserverType, PatchType>::delta_int()

/**
     * External transition function.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param e time
     * @param xb bag of events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::delta_ext(double e,
                                                       const adevs::Bag<IO_Type> &xb)
{
  // Attempt to "consume" input
  // adevs::Bag<IO_Type>::const_iterator i = xb.begin();
  for (auto i : xb)
  {
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                  "Relogo input on port <"
                      << i.port << ">");

    if (i.port == setup_in)
    { // event on <properties_in> port
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                    "Found port <"
                        << setup_in
                        << ">: initializing repast model..");
      
      std::string lC_configFile = "";
      
      try {
	lC_configFile =
	  boost::any_cast<std::string>( i.value );
      }
      catch (const boost::bad_any_cast &) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Unable to translate output");
      }
      setup(lC_configFile);
    }
  }
}

/**
     * Confluent transition function.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param xb bag of input events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::delta_conf(const adevs::Bag<IO_Type> &xb)
{
  delta_ext(0.0, xb);
  delta_int();
}

/**
     * Output function.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param yb bag of output events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::output_func(adevs::Bag<IO_Type> &yb)
{
  //---------------------------------------------------------------------------
  // Check if the simulation is still running.
  // If not, get reference to Repast HPC ScheduleRunner and shut the simulation
  // down -- this should complete all data recording
  //--------------------------------------------------------------------------- 
  repast::ScheduleRunner &runner =
      repast::RepastProcess::instance()->getScheduleRunner();
  
  if (!runner.isRunning()) {
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		  "Shutting repast::RepastProcess down...");
    repast::RepastProcess::instance()->done();
  }

  // Before proceeding, check if the wrapped model exists
  if (mCp_model.get() == nullptr) return;

  // copy the model properties to a JSON::Value object
  const repast::Properties &lCr_properties = mCp_model->getProperties();
  repast::Properties::key_iterator iter = lCr_properties.keys_begin();

  Json::Value lC_parameters;
  for (; iter != lCr_properties.keys_end(); iter++)
  {
    lC_parameters[*iter] = lCr_properties.getProperty(*iter);
  }

  // output properties map
  IO_Type y("properties_out",
                           lC_parameters);
  yb.insert(y);

  // get model output and direct output to output ports
  Json::Value lC_output =
      mCp_model->getObserver()->outputFunction();

  if (lC_output.isObject()) {
    Json::Value::Members lC_memberNames =
      lC_output.getMemberNames();
    for (int i = 0; i < lC_memberNames.size(); i++) {
      y = IO_Type( lC_memberNames[i],
				  lC_output[ lC_memberNames[i] ] );
      yb.insert(y);
    }
  }
}

/**
     * Time advance function.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @return time advance
     */
template <typename ObserverType, typename PatchType>
double RelogoWrapper<ObserverType, PatchType>::ta()
{
  repast::ScheduleRunner &runner =
      repast::RepastProcess::instance()->getScheduleRunner();

  if (!runner.isRunning())
    return DBL_MAX;

  if (runner.schedule().getNextTick() < 0)
    return DBL_MAX;

  double ld_time = runner.currentTick();
  if (ld_time < 0.)
    ld_time = 0.;

  return (runner.schedule().getNextTick() - ld_time);
}

/**
     * Output value garbage collection.
     *
     * @tparameter ObserverType wrapped Repast HPC Relogo Observer type
     * @tparameter PatchType wrapped Repast HPC Relogo Patch type
     * @param g bag of events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::gc_output(adevs::Bag<IO_Type> &g)
{
}

//-----------------------------------------------
// end of class RelogoWrapper implementation
//-----------------------------------------------

} // namespace impl
} // namespace efscape
