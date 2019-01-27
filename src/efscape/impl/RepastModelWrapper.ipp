// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RepastModelWrapper.ipp
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

#include <efscape/impl/RepastModelWrapper.hpp> // class definition

// definitions for efscape models
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/utils/type.hpp>

// Repast HPC definitions
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/logger.h>

// other definitions
#include <json/json.h>
#include <log4cxx/logger.h>

namespace efscape {
  namespace impl {

    // instantiate class data members
    template <class ModelType>
    const efscape::impl::PortType RepastModelWrapper<ModelType>::setup_in =
      "setup_in";

    /**
     * default constructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
    template <class ModelType>
    RepastModelWrapper<ModelType>::RepastModelWrapper() :
      ATOMIC()
    {
      init();
    } // RepastModelWrapper<ModelType>::RepastModel()

    /**
     * constructor with argument in JSON format
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param aC_args JSON containing model attributes
      */
    template <class ModelType>
    RepastModelWrapper<ModelType>::RepastModelWrapper(Json::Value aC_args) :
      ATOMIC()
    {
      init(aC_args);
    } // RepastModelWrapper<ModelType>::RepastModel(Json::Value)

    /**
     * Init function.
     *
     * Initilizes the wrapped Repast HPC model
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param aC_args
     */
    template <class ModelType>
    void RepastModelWrapper<ModelType>::init(Json::Value aC_args) {
      
      std::string lC_id = efscape::utils::type< RepastModelWrapper<ModelType> >(*this);
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Creating <"
		    << lC_id << ">...");

      // 1) Create Repast model
      mCp_model.reset( new ModelType() ); // create model
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "===> 1) Just reset the wrapped model ***");

      // 2) Get default config file

      // attempt to retrieve the root directory of efscape ICE configuration
      std::string lC_EfscapeIcePath = "."; // default location
      char* lcp_env_variable =	// get EFSCAPE_HOME
	getenv("EFSCAPE_HOME");

      if ( lcp_env_variable != 0 )
	lC_EfscapeIcePath = lcp_env_variable;

      std::string lC_config_file = lC_EfscapeIcePath
	+ std::string("/config.props");

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "===> 2) Attempting to load configuration file ***");

      // 3) Intialize RepastProcess
      repast::RepastProcess::init(lC_config_file);

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "===> 3) Initialized the RepastProcess!");

      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "===> Completed configuration of <"
		    << lC_id << ">");
      
      // 4) Copy JSON attributes to repast Properties
      repast::Properties lC_props;
      if (aC_args.isObject()) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "aC_args is an object")
      	for (Json::Value::const_iterator it=aC_args.begin(); it!=aC_args.end();
      	     ++it) {
	  std::string lC_key = it.key().asString();
	  std::string lC_value = it->asString();
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"Setting property <"
			<< lC_key
			<< "> = <"
			<< lC_value
			<< ">");

      	  lC_props.putProperty( lC_key,
      	  		        lC_value );
      	}
      } else {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "aC_args is not an object");
      }

      // 5) set properties
      mCp_props.reset( new repast::Properties(lC_props) );
    }

    /**
     * destructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
    template <class ModelType>
    RepastModelWrapper<ModelType>::~RepastModelWrapper() {
      std::string lC_id = efscape::utils::type< RepastModelWrapper<ModelType> >(*this);
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Deleting RepastModelWrapper=<"
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
    template <class ModelType>
    void RepastModelWrapper<ModelType>::delta_int() {
      // execute next event
      repast::RepastProcess::instance()->getScheduleRunner().execNextEvent();
    } // RepastModelWrapper<ModelType>::delta_int()

    /**
     * External transition function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param e time
     * @param xb bag of events
     */
    template <class ModelType>
    void RepastModelWrapper<ModelType>::delta_ext(double e,
				const adevs::Bag<IO_Type>& xb)
    {
      // Attempt to "consume" input
      adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();

      for (; i != xb.end(); i++) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "RepastModel input on port <"
		      << (*i).port << ">");
	
	if ( (*i).port == setup_in) { // event on <properties_in> port
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
    template <class ModelType>
    void RepastModelWrapper<ModelType>::delta_conf(const adevs::Bag<IO_Type>& xb)
    {
      delta_ext(0.0,xb);
      delta_int();
    }

    /**
     * Output function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param yb bag of output events
     */
    template <class ModelType>
    void RepastModelWrapper<ModelType>::output_func(adevs::Bag<IO_Type>& yb) {
      // copy the model properties to a JSON::Value object
      const repast::Properties& lCr_properties = mCp_model->getProperties();
      repast::Properties::key_iterator iter = lCr_properties.keys_begin();

      Json::Value lC_parameters;
      for ( ; iter != lCr_properties.keys_end(); iter++) {
	lC_parameters[*iter] = lCr_properties.getProperty(*iter);
      }

      // output properties map
      efscape::impl::IO_Type y("properties_out",
			       lC_parameters);
      yb.insert(y);

      // get model output
      repast::ScheduleRunner& runner =
	repast::RepastProcess::instance()->getScheduleRunner();

     Json::Value lC_output =
	mCp_model->outputFunction();
    }

    /**
     * Time advance function.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @return time advance
     */
    template <class ModelType>
    double RepastModelWrapper<ModelType>::ta() {
      repast::ScheduleRunner& runner =
	repast::RepastProcess::instance()->getScheduleRunner();

      if ( !runner.isRunning() )
	return DBL_MAX;

      if ( runner.schedule().getNextTick() < 0)
	return DBL_MAX;

      double ld_time = runner.currentTick();
      if (ld_time < 0.)
	ld_time = 0.;

      return ( runner.schedule().getNextTick() - ld_time );
    }

    /**
     * Output value garbage collection.
     *
     * @tparameter ModelType wrapped Repast HPC model class
     * @param g bag of events
     */
    template <class ModelType>
    void RepastModelWrapper<ModelType>::gc_output(adevs::Bag<IO_Type>& g) {
    }

    //-----------------------------------------------
    // end of class RepastModelWrapper implementation
    //-----------------------------------------------

  } // namespace impl
}   // namespace efscape
