// __COPYRIGHT_START__
// Package Name : efscape
// File Name : AdevsModel.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/AdevsModel.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>

#include <efscape/utils/type.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <sstream>

using namespace boost::posix_time;
using namespace boost::filesystem;

namespace efscape {

  namespace impl {

    // instantiate class data members
    int AdevsModel::mSi_nextJobID = 0;

    /** constructor */
    AdevsModel::AdevsModel() :
      EntityI(),
      mC_ResourcePath("."),
      mC_WorkDir("."),
      mi_JobID(0)
    {
      name("efscape");	// root model name (name of main library)

      mi_JobID = ++mSi_nextJobID;

    } // AdevsModel::AdevsModel()

    /** destructor */
    AdevsModel::~AdevsModel()
    {
      LOG4CXX_DEBUG(getLogger(),
		    "Deleting root model<" << name() << ">...");

      DEVS* lCp_WrappedModel = getWrappedModel();
      if (lCp_WrappedModel != NULL)
	delete lCp_WrappedModel;
    }

    /**
     * Initializes the model. The primary model is paired with a simulator.
     * Derived classes are responsible for generating the model hierarchy (pre-
     * initialization) and supplying additional listeners (post-initialization).
     *
     * @throws std::logic_error
     */
    void AdevsModel::initialize()
      throw(std::logic_error)
    {
      std::string lC_message = "AdevsModel::initialize(): ";

      LOG4CXX_DEBUG(getLogger(),
		    lC_message << "initializing root model..."); 

      // verify that the wrapped model has been set properly
      adevs::Devs<IO_Type>* lCp_model = getWrappedModel();
      if (lCp_model == 0) {
	  lC_message += "main model does not exist";
	  throw std::logic_error(lC_message);
      }

      // create output directory in $EFSCAPE_HOME/jobs/scenario-id
      std::ostringstream lC_OstringJobID;
      lC_OstringJobID << this->name() << "-"
		      << std::setw(4) << std::setfill('0') << mi_JobID;
      std::string lC_ModelWorkDir =
	ModelHomeI::getHomeDir() + "/jobs/"
	+ lC_OstringJobID.str();

      path p = complete(path(lC_ModelWorkDir.c_str()));
      if ( !exists(p) ) {
	try {
	  create_directory(p);
	}
	catch(std::exception& e) {
	  lC_message += e.what();
	  throw std::logic_error(lC_message);
	}
      }
      this->setWorkDir(lC_ModelWorkDir.c_str());

      if (!initializeModel(lCp_model)) {
	lC_message += "unable to initialize wrapped model";
	throw (std::logic_error(lC_message));
      }

    } // AdevsModel::initialize()

    //-------------------------
    // accessor/mutator methods
    //-------------------------

    /** @returns handle to clock */
    ClockIPtr& AdevsModel::getClockIPtr() { return mCp_ClockI; }

    /**
     * Sets the handle to a clock.
     *
     * @param aCp_clock handle to clock
     */
    void AdevsModel::setClockIPtr(const ClockIPtr& aCp_clock) {
      mCp_ClockI = aCp_clock;
    }

    /**
     * Returns smart handle to logger
     *
     * @returns smart handle to logger
     */
    log4cxx::LoggerPtr AdevsModel::getLogger() const
    {
      std::ostringstream lC_OstringJobID;
      lC_OstringJobID << this->name() << "-"
		      << std::setw(4) << std::setfill('0') << mi_JobID;

      std::string lC_LoggerName =
	ModelHomeI::getLogger()->getName() + "." + lC_OstringJobID.str();

      return log4cxx::Logger::getLogger(lC_LoggerName.c_str());
    }

    /**
     * Returns the resource path on the server.
     *
     * @returns resource path
     */
    std::string AdevsModel::getResourcePath() const { return mC_ResourcePath; }

    /**
     * Sets the path of resources on the server.
     *
     * @param acp_path resource path
     */
    void AdevsModel::setResourcePath(const char* acp_path) { mC_ResourcePath = acp_path;}

    /**
     * Returns the model output path on the server.
     *
     * @returns model output path
     */
    std::string AdevsModel::getWorkDir() const { return mC_WorkDir; }

    /**
     * Sets the model output path on the server.
     *
     * @param acp_WorkDir new output path
     */
    void AdevsModel::setWorkDir(const char* acp_WorkDir) { mC_WorkDir = acp_WorkDir; }

    // class AdevsModel implementation

    /**
     * Utility function that returns the root AdevsModel of the adevs hierarchy
     * containing this model.
     *
     * @param aCp_model
     * @returns handle to root model
     */
    const AdevsModel* getAdevsModel(adevs::Devs<IO_Type>* aCp_model) {
      return dynamic_cast<const AdevsModel*>(getRootModel(aCp_model));
    }

  } // namespace impl

} // namespace efscape
