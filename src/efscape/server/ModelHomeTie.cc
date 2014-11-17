// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/server/ModelHomeTie.hh>

#include <efscape/server/ModelTie.hh>
#include <efscape/server/SimulatorI.hh>
#include <efscape/server/JsonDatasetI.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>

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
	impl::Singleton<impl::ModelHomeI>::Instance().CreateModel(name.c_str());

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
	  impl::Singleton<impl::ModelHomeI>::Instance().CreateModelFromStr(configuration);

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
     * Create a new ::efscape::Model proxy from the specified XML configuration.
     *
     * @param name model class name
     * @param configuration model configuration
     * @param current method invocation
     * @returns ::efscape::Model proxy
     */
    ::efscape::ModelPrx
    ModelHomeTie::createWithConfig(const ::std::string& name,
				   const ::std::wstring& configuration,
				   const Ice::Current& current)
    {
      efscape::ModelPtr lCp_model = 0;

      try {
	// attempt create the model
	adevs::Devs<efscape::impl::IO_Type>* lCp_modelI =
	  impl::Singleton<impl::ModelHomeI>::Instance().CreateModelWithConfig(name.c_str(),
					       configuration);

	if (lCp_modelI) {
	  // tie the model
	  lCp_model = new ModelTie(lCp_modelI, name.c_str());
	  ModelTie* modelTest = new ModelTie(lCp_modelI, name.c_str());

	}
	else {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"efscape::server::ModelHomeTie(): model implementation not found!");
	}
      }
      catch (std::logic_error excp) {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "efscape::server::ModelHomeTie()"
		      << excp.what() );
      }

      if (lCp_model) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Model proxy for <" << lCp_model->getName() << "> created...");

	try {
	  efscape::ModelPrx lCp_ModelPrx =
	    efscape::ModelPrx::uncheckedCast(current.adapter
					     ->addWithUUID( lCp_model ) );
	  return lCp_ModelPrx;
	}
	catch (std::exception excp) {
	  std::cerr << "*** Failed to set Model Proxy: " << excp.what() << " ***\n";
	}
					   
      }

      LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		    "Model not found...");
      return 0;

    } // ModelHomeTie::createWithConfig(...)

    /**
     * Returns a list of available models.
     *
     * @param current method invocation
     * @returns list of available models
     */
    ::efscape::ModelList ModelHomeTie::getModelList(const Ice::Current& current)
    {
      ::efscape::ModelList lC1_ModelList;
      return lC1_ModelList;
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

    /**
     * Creates a JsonDataset object.
     *
     * @param data data for JsonDataset
     * @param schema schema for JsonDataset
     * @param current method invocation
     * @returns ::efscape::JsonDataset proxy
     */
    ::efscape::JsonDatasetPrx
    ModelHomeTie::createJsonDataset(const ::std::string& data,
				    const ::std::string& schema,
				    const Ice::Current& current)
    {
      JsonDatasetIPtr lCp_JsonDataset = new JsonDatasetI(data,schema);

      ::efscape::JsonDatasetPrx lCp_JsonDatasetPrx =
	  ::efscape::JsonDatasetPrx::uncheckedCast(current.adapter
						   ->addWithUUID( lCp_JsonDataset ) );

      return lCp_JsonDatasetPrx;
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
