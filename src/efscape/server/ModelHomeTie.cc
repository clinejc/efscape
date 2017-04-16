// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/server/ModelHomeTie.hh>

#include <efscape/server/ModelTie.hh>
#include <efscape/server/SimulatorI.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <sstream>

namespace efscape {

  namespace server {

    /**
     * Create a new ::efscape::Model proxy from the specified model class name.
     *
     * @param name model class name
     * @param current method invocation
     * @returns ::efscape::Model proxy
     */
    ::efscape::ModelPrx
    ModelHomeTie::create(const ::std::string& name,
			 const Ice::Current& current)
    {
      // attempt create the model
      adevs::Devs<efscape::impl::IO_Type>* lCp_model =
	impl::Singleton<impl::ModelHomeI>::Instance().createModel(name.c_str());

      if (lCp_model) {
	// wrap the model
	ModelTie* lCp_ModelTie = new ModelTie(lCp_model);

	// activate the model
	efscape::ModelPtr lCp_model = lCp_ModelTie;
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Model <" << lCp_model->getName() << "> found...");
	efscape::ModelPrx lCp_ModelPrx =
	  efscape::ModelPrx::uncheckedCast(current.adapter
					     ->addWithUUID( lCp_model ) );
	return lCp_ModelPrx;
      }

      LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		    "Model not found...");
      return 0;

    } // ModelHomeTie::create(...)

    /**
     * Create a new ::efscape::Model proxy from the specified XML configuration.
     *
     * @param configuration model XML configuration
     * @param current method invocation
     * @returns ::efscape::Model proxy
     */
    ::efscape::ModelPrx
    ModelHomeTie::createFromXML(const ::std::wstring& configuration,
				const Ice::Current& current)
    {
      efscape::ModelPtr lCp_model = 0;

      try {
	// attempt create the model
	adevs::Devs<efscape::impl::IO_Type>* lCp_modelI =
	  impl::Singleton<impl::ModelHomeI>::Instance().createModelFromXML(configuration);

	if (lCp_modelI) {
	  // tie the model
	  lCp_model = new ModelTie(lCp_modelI);
	}
	else {
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"efscape::server::ModelHomeTie(): model implementation not found!");
	}
      }
      catch (std::logic_error exp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "efscape::server::ModelHomeTie()"
		      << exp.what() );
      }

      if (lCp_model) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Model <" << lCp_model->getName() << "> found...");

	efscape::ModelPrx lCp_ModelPrx =
	  efscape::ModelPrx::uncheckedCast(current.adapter
					   ->addWithUUID( lCp_model ) );
	return lCp_ModelPrx;
					   
      }

      LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		    "Model not found...");
      return 0;

    } // ModelHomeTie::createFromXML(...)

    /**
     * Create a new ::efscape::Model proxy from the specified JSON configuration.
     *
     * @param configuration model JSON configuration
     * @param current method invocation
     * @returns ::efscape::Model proxy
     */
    ::efscape::ModelPrx
    ModelHomeTie::createFromJSON(const ::std::string& configuration,
				const Ice::Current& current)
    {
      efscape::ModelPtr lCp_model = 0;

      try {
	// attempt create the model
	adevs::Devs<efscape::impl::IO_Type>* lCp_modelI =
	  impl::Singleton<impl::ModelHomeI>::Instance().createModelFromJSON(configuration);

	if (lCp_modelI) {
	  // tie the model
	  lCp_model = new ModelTie(lCp_modelI);
	}
	else {
	  LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			"efscape::server::ModelHomeTie(): model implementation not found!");
	}
      }
      catch (std::logic_error exp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "efscape::server::ModelHomeTie()"
		      << exp.what() );
      }

      if (lCp_model) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Model <" << lCp_model->getName() << "> found...");

	efscape::ModelPrx lCp_ModelPrx =
	  efscape::ModelPrx::uncheckedCast(current.adapter
					   ->addWithUUID( lCp_model ) );
	return lCp_ModelPrx;
					   
      }

      LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		    "Model not found...");
      return 0;

    } // ModelHomeTie::createFromJSON(...)

    /**
     * Returns a list of available models.
     *
     * @param current method invocation
     * @returns list of available models
     */
    ::efscape::ModelNameList ModelHomeTie::getModelList(const Ice::Current& current)
    {
      ::efscape::ModelNameList lC1_ModelNameList;

      // retrieve the list of available models
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "*** Available models: ***");
      std::set<std::string> lC1_ModelNames =
	impl::Singleton<impl::ModelHomeI>::Instance().
      	getModelFactory().getTypeIDs();
      std::set<std::string>::iterator iter;
      for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "=> modelName=" << *iter);
	lC1_ModelNameList.push_back(*iter);
      }
	
      return lC1_ModelNameList;
    }

    /**
     * Returns info about the specified model in a JSON string
     *
     * @param aC_name type of model
     * @param current method invocation
     * @returns info about the specified model in a JSON string
     */
    ::std::string ModelHomeTie::getModelInfo(const ::std::string& aC_name,
					     const Ice::Current& current)
    {
      boost::property_tree::ptree lC_ModelInfo =
	impl::Singleton<impl::ModelHomeI>::Instance().
	getModelFactory().getProperties(aC_name.c_str());

      std::string lC_JsonStr = "{}";
      try {
	std::ostringstream lC_buffer;
	boost::property_tree::write_json(lC_buffer,
					 lC_ModelInfo,
					 false);
	lC_JsonStr = lC_buffer.str();
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "JSON=>" << lC_JsonStr);
      }
      catch (...) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "unknown exception converting JSON to string.");
      }

      return lC_JsonStr;
    }

    /**
     * Creates a simulator object for a model.
     *
     * @param rootModel handle to root model
     * @param current method invocation
     * @returns ::efscape::Simulator proxy
     */
    ::efscape::SimulatorPrx
    ModelHomeTie::createSim(const ::efscape::ModelPrx& rootModel,
			  const Ice::Current& current)
    {
      SimulatorIPtr lCp_simulator = new SimulatorI(rootModel);

      ::efscape::SimulatorPrx lCp_SimulatorPrx =
	  ::efscape::SimulatorPrx::uncheckedCast(current.adapter
						 ->addWithUUID( lCp_simulator ) );
      return lCp_SimulatorPrx;
    }

    //--------------------------------------------------------------------------
    // implementation of local (server-side) methods
    //--------------------------------------------------------------------------

    /** constructor */
    ModelHomeTie::ModelHomeTie()
    {
      try {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Loading libraries");
	// impl::Singleton<impl::ModelHomeI>::Instance().LoadLibraries();
      }
      catch(std::logic_error lC_excp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      lC_excp.what());
      }
      catch (...) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "A generic exception occurred");
      }

    } // ModelHomeTie::ModelHomeTie()


    /** destructor */
    ModelHomeTie::~ModelHomeTie() {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Shutting down EFSCAPE ICE model repository...");
    }

  } // namespace server

} // namespace efscape
