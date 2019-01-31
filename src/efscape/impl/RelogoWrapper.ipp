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
#include <efscape/utils/type.hpp>

// Repast HPC definitions
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/logger.h>

// other definitions
#include <json/json.h>
#include <log4cxx/logger.h>

namespace efscape
{
namespace impl
{

// instantiate class data members
template <typename ObserverType, typename PatchType>
const efscape::impl::PortType RelogoWrapper<ObserverType, PatchType>::setup_in =
    "setup_in";

/**
     * default constructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::RelogoWrapper() : ATOMIC()
{
  init();
} // RelogoWrapper<ObserverType, PatchType>::Relogo()

/**
     * constructor with argument in JSON format
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param aC_args JSON containing model attributes
      */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::RelogoWrapper(Json::Value aC_args) : ATOMIC()
{
  init(aC_args);
} // RelogoWrapper<ObserverType, PatchType>::Relogo(Json::Value)

/**
     * Init function.
     *
     * Initilizes the wrapped Repast HPC model
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param aC_args
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::init(Json::Value aC_args)
{

  std::string lC_id = efscape::utils::type<RelogoWrapper<ObserverType, PatchType>>(*this);
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "Creating <"
                    << lC_id << ">...");

  // 1) Get default config file

  // attempt to retrieve the root directory of efscape ICE configuration
  std::string lC_EfscapeIcePath = "."; // default location
  char *lcp_env_variable =             // get EFSCAPE_HOME
      getenv("EFSCAPE_HOME");

  if (lcp_env_variable != 0)
    lC_EfscapeIcePath = lcp_env_variable;

  std::string lC_config_file = lC_EfscapeIcePath + std::string("/config.props");

  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "===> 1) Attempting to load configuration file ***");

  // 2) Intialize RepastProcess
  repast::RepastProcess::init(lC_config_file);

  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "===> 2) Initialized the RepastProcess!");

  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "===> Completed configuration of <"
                    << lC_id << ">");

  // 3) Copy JSON attributes to repast Properties
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "===> 3) Copying JSON attributes to repast Properties");

  repast::Properties lC_props;
  if (aC_args.isObject())
  {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                  "aC_args is an object")
    for (Json::Value::const_iterator it = aC_args.begin(); it != aC_args.end();
         ++it)
    {
      std::string lC_key = it.key().asString();
      std::string lC_value = it->asString();
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                    "Setting property <"
                        << lC_key
                        << "> = <"
                        << lC_value
                        << ">");

      lC_props.putProperty(lC_key,
                           lC_value);
    }
  }
  else
  {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                  "aC_args is not an object");
  }

  mCp_props.reset(new repast::Properties(lC_props));

  // 4) Create Repast model
  boost::mpi::communicator *world =
      repast::RepastProcess::instance()->getCommunicator();
  mCp_model.reset(new repast::relogo::SimulationRunnerPlus<ObserverType, PatchType>(world));

  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "===> 4) Just reset the wrapped model ***");
}

/**
     * destructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
template <typename ObserverType, typename PatchType>
RelogoWrapper<ObserverType, PatchType>::~RelogoWrapper()
{
  std::string lC_id = efscape::utils::type<RelogoWrapper<ObserverType, PatchType>>(*this);
  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                "Deleting RelogoWrapper=<"
                    << lC_id << ">...");

  // Shutdown Repast process
  repast::RepastProcess::instance()->done();
}

//--------------------------------------------
// begin implementation of devs::adevs methods
//--------------------------------------------
/**
     * Internal transition function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::delta_int()
{
  // execute next event
  repast::RepastProcess::instance()->getScheduleRunner().execNextEvent();
} // RelogoWrapper<ObserverType, PatchType>::delta_int()

/**
     * External transition function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param e time
     * @param xb bag of events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::delta_ext(double e,
                                                       const adevs::Bag<IO_Type> &xb)
{
  // Attempt to "consume" input
  adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();

  for (; i != xb.end(); i++)
  {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                  "Relogo input on port <"
                      << (*i).port << ">");

    if ((*i).port == setup_in)
    { // event on <properties_in> port
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
                    "Found port <"
                        << setup_in
                        << ">: initializing repast model..");
      mCp_model->setup(*mCp_props); // initialize repast hpc model
    }
  }
}

/**
     * Confluent transition function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
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
     * @tparameter ModelType wrapped Repast HPC model class
     * @param yb bag of output events
     */
template <typename ObserverType, typename PatchType>
void RelogoWrapper<ObserverType, PatchType>::output_func(adevs::Bag<IO_Type> &yb)
{
  // copy the model properties to a JSON::Value object
  const repast::Properties &lCr_properties = mCp_model->getProperties();
  repast::Properties::key_iterator iter = lCr_properties.keys_begin();

  Json::Value lC_parameters;
  for (; iter != lCr_properties.keys_end(); iter++)
  {
    lC_parameters[*iter] = lCr_properties.getProperty(*iter);
  }

  // output properties map
  efscape::impl::IO_Type y("properties_out",
                           lC_parameters);
  yb.insert(y);

  // get model output
  repast::ScheduleRunner &runner =
      repast::RepastProcess::instance()->getScheduleRunner();

  Json::Value lC_output =
      mCp_model->getObserver()->outputFunction();
}

/**
     * Time advance function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
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
     * @tparameter ModelType wrapped Repast HPC model class
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
