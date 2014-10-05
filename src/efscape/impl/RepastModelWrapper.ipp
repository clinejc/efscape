// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RepastModelWrapper.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/RepastModelWrapper.hpp>

// #include <repast_hpc/Properties.h>
// #include <repast_hpc/Schedule.h>
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/logger.h>
#include <log4cxx/logger.h>
#include <efscape/impl/ModelHomeI.hh>
#include <iostream>

#include <efscape/utils/type.hpp>

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
      std::cout << "Creating <"
		<< lC_id << ">...\n";

      // 1) Create Repast model
      mCp_model.reset( new ModelType() ); // create model
      std::cout << "\t*** 1) Just reset the wrapped model ***\n";

      // 2) Get default config file

      // attempt to retrieve the root directory of efscape ICE configuration
      std::string lC_EfscapeIcePath = "."; // default location
      char* lcp_env_variable =	// get EFSCAPE_HOME
	getenv("EFSCAPE_HOME");

      if ( lcp_env_variable != 0 )
	lC_EfscapeIcePath = lcp_env_variable;

      std::string lC_config_file = lC_EfscapeIcePath
	+ std::string("/config.props");


      std::cout << "\t*** 2) Attempting to load configuration file ***\n";

      // 3) Intialize RepastProcess
      repast::RepastProcess::init(lC_config_file);

      std::cout << "\t*** 3) Initialized the RepastProcess!\n";

      std::cout << "\t*** Completed configuration of <"
		<< lC_id << ">\n";

    } // RepastModelWrapper<ModelType>::RepastModel()

    /**
     * destructor
     *
     * @tparameter ModelType wrapped Repast HPC model class
     */
    template <class ModelType>
    RepastModelWrapper<ModelType>::~RepastModelWrapper() {
      std::string lC_id = efscape::utils::type< RepastModelWrapper<ModelType> >(*this);
      std::cout << "Deleting RepastModelWrapper=<"
		<< lC_id << ">...\n";

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
	std::cout << "RepastModel input on port <"
		  << (*i).port << ">\n";
	if ( (*i).port == properties_in) { // event on <properties_in> port
	  std::cout << "Found port=" << properties_in << std::endl;
	  try {
	    // 1) try to extract the properties file name
	    std::string lC_propsFile =
	      boost::any_cast<std::string>( (*i).value );

	    // 2) if time < 0., need to initialize the model
	    if ( e < 0.) {
	      // 2014-09-26 added by jcline: (re-)sets clock to 0.
	      repast::RepastProcess::instance()->getScheduleRunner().init();

	      boost::mpi::communicator* world =
		repast::RepastProcess::instance()->getCommunicator();

	      // Load properties and initialize
	      repast::Properties lC_props(lC_propsFile,
					  world);
	      mCp_model->setup(lC_props);
	    }
	  }
	  catch(const boost::bad_any_cast &) {
	    std::cout << "Unable to cast output\n";
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <std::string>");
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

    //------------------------------------------
    // end of class RepastModelWrapper implementation
    //------------------------------------------

  } // namespace impl
}   // namespace efscape
