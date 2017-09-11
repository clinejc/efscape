// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RepastModelWrapper.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/RepastModelWrapper.hpp> // class definition

// definitions for efscape models
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/utils/type.hpp>

// Repast HPC definitions
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/logger.h>

// boost definitions
#include <boost/foreach.hpp>

// other definitions
#include <log4cxx/logger.h>
#include <iostream>

namespace efscape {
  namespace impl {

    // instantiate class data members
    template <class ModelType>
    const efscape::impl::PortType RepastModelWrapper<ModelType>::properties_in =
      "properties_in";

    /**
     * default constructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
    template <class ModelType>
    RepastModelWrapper<ModelType>::RepastModelWrapper() :
      ATOMIC()
    {
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

    } // RepastModelWrapper<ModelType>::RepastModel()

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
	if ( (*i).port == properties_in) { // event on <properties_in> port
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"Found port=" << properties_in);
	  try {
	    // 1) try to extract the properties file name
	    boost::property_tree::ptree pt =
	      boost::any_cast<boost::property_tree::ptree>( (*i).value );

	    // 2) if time < 0., need to initialize the model
	    if ( e < 0.) {
	      // 2014-09-26 added by jcline: (re-)sets clock to 0.
	      repast::RepastProcess::instance()->getScheduleRunner().init();

	      // 2015-05-27 currently ignoring the 'world' variable
	      boost::mpi::communicator* world =
		repast::RepastProcess::instance()->getCommunicator();

	      // Load properties and initialize
	      repast::Properties lC_props;

	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "Loading properties...");
	      BOOST_FOREACH( boost::property_tree::ptree::value_type const& rowPair,
			     pt.get_child( "" ) ) {
		LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			      "=> property "
			      << "<" << rowPair.first << ">="
			      << "<" << rowPair.second.get_value<std::string>() << ">");
		lC_props.putProperty(rowPair.first,
				     rowPair.second.get_value<std::string>() );
	      }
	      
	      mCp_model->setup(lC_props);
	    }
	  }
	  catch(const boost::bad_any_cast &) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <boost::property_tree>");
	  }
	} // 'if ( (*i).port == properties_in)'

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
      // copy the model properties to a boost::property_tree::ptree
      const repast::Properties& lCr_properties = mCp_model->getProperties();
      repast::Properties::key_iterator iter = lCr_properties.keys_begin();
      boost::property_tree::ptree lC_ptree;
      for ( ; iter != lCr_properties.keys_end(); iter++) {
	lC_ptree.put( *iter,
		      lCr_properties.getProperty(*iter) );
      }

      // output properties map
      efscape::impl::IO_Type y("properties_out",
			       lC_ptree);
      yb.insert(y);

      // get model output
      repast::ScheduleRunner& runner =
	repast::RepastProcess::instance()->getScheduleRunner();

      // std::map< std::string, boost::any > lC_output =
      // 	mCp_model->getOutput();
      // std::map< std::string, boost::any >::iterator iter2 =
      // 	lC_output.begin();
      // for ( ; iter2 != lC_output.end(); iter2++) {
      // 	y.port = iter2->first;
      // 	y.value = iter2->second;
      // 	yb.insert(y);
      // }

      boost::property_tree::ptree lC_output =
	mCp_model->outputFunction();
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		    lC_output.get_child("")) {
        y.port = v.first;
	y.value = v.second;
	yb.insert(y);
      }
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
