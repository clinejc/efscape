// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelFactory.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELFACTORY_IPP
#define EFSCAPE_IMPL_MODELFACTORY_IPP

#include <efscape/impl/ModelFactory.hpp>

#include <efscape/impl/ModelHomeI.hh>
#include <efscape/utils/type.hpp>
#include <efscape/utils/boost_utils.ipp>

// boost serialization definitions
#include <boost/serialization/version.hpp>
#include <boost/serialization/type_info_implementation.hpp>

#include <iostream>

namespace efscape {

  namespace impl {

    /**
     * default constructor
     *
     * @tparam BaseType base type of factory
     */
    template <class BaseType>
    ModelFactoryTmpl<BaseType>::ModelFactoryTmpl() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
    		    "Creating ModelFactory...");
    }

    /**
     * destructor
     *
     * @tparam BaseType base type of factory
     */
    template <class BaseType>
    ModelFactoryTmpl<BaseType>::~ModelFactoryTmpl() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
    		    "Shutting down ModelFactory...");
    }

    /**
     * Returns a model object of type <acp_name>.
     *
     * @tparam BaseType base type of factory
     * @param acp_name model type name
     * @returns handle to new model object
     */
    template <class BaseType>
    BaseType* ModelFactoryTmpl<BaseType>::CreateModel(const char* acp_name)
    {
      try {
	// return mCF_factories[acp_name]();
	return efscape::utils::createObject<BaseType>(mCF_factories,
						      acp_name);
      }
      catch(...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "ModelFactoryTmpl<>::CreateModel(" << acp_name
		      << "): class not found");
      }
      return 0;
    }

    /**
     * Lists all models in the repository .
     *
     * @tparam BaseType base type of factory
     * @param aC1r_ModelSet selected subset of models
     */
    template <class BaseType>
    void
    ModelFactoryTmpl<BaseType>::ListModels(std::set<std::string>&
					   aCr_ModelNames)
    {
      std::map< std::string, ModelInfo >::iterator iModel;
      for (iModel = mCC_ModelInfoMap.begin(); iModel != mCC_ModelInfoMap.end();
	   iModel++) {
	boost::scoped_ptr<BaseType> lCp_model( CreateModel(iModel->first.c_str()) );
	if (lCp_model == NULL) {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"Unable to create model<" << iModel->first << ">");
	  continue;
	}
	aCr_ModelNames.insert(iModel->first);
      }	// for (iModel = ...

    }	// ModelFactoryTmpl<>::ListModels(std::set<std::string>)

    /**
     * Lists models in the repository that match the specified criteria.
     *
     * @tparam BaseType base type of factory
     * @param aC1r_ModelSet selected subset of models
     * @param aCr_ModelMatch selection criteria
     */
    template <class BaseType>
    void
    ModelFactoryTmpl<BaseType>::ListModels(std::set<std::string>&
					   aCr_ModelNames,
					   bool(*aFp_ModelMatch)
					   (const BaseType*) )
    {
      std::map< std::string, ModelInfo >::iterator iModel;
      for (iModel = mCC_ModelInfoMap.begin(); iModel != mCC_ModelInfoMap.end();
	   iModel++) {
	BaseType* lCp_model = CreateModel(iModel->first.c_str());
	if (lCp_model == NULL) {
	  LOG4CXX_ERROR(ModelHomeI::getLogger(),
			"Unable to create model<" << iModel->first << ">");
	  continue;
	}
	if ( (*aFp_ModelMatch)(lCp_model))
	  aCr_ModelNames.insert(iModel->first);
      }	// for (iModel = ...

    }	// ModelFactoryTmpl<>::ListModels(std::set<std::string>,...)

    /**
     * Returns model info for model <acp_classname>.
     *
     * @tparam BaseType base type of factory
     * @param acp_classname model class name
     * @returns model info for model <acp_classname>
     */
    template <class BaseType>
    ModelInfo
    ModelFactoryTmpl<BaseType>::getModelInfo(const char* acp_classname)
    {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Attempting to get info about model<"
		    << acp_classname
		    << ">");
      std::map< std::string, ModelInfo >::iterator iModel;
      if ( (iModel = mCC_ModelInfoMap.find(acp_classname) ) !=
	   mCC_ModelInfoMap.end() )
	return iModel->second;
      
      return ModelInfo();

    } // ModelFactoryTmpl<>::getModelInfo(const char*)

    /**
     * Registers a model in the factory repository and model info repository.
     *
     * @tparam BaseType base model type
     * @tparam DerivedType derived model type
     * @param aC_info class info in a JSON string
     * @returns whether registration was successful 
     */
    template <class BaseType> template<class DerivedType>
    bool
    ModelFactoryTmpl<BaseType>::RegisterModel(std::string aC_info)
    {
      std::string lC_properties = "{}";
      std::string lC_library_name = "efscapeimpl";
      RegisterModel(aC_info,
		    lC_properties,
		    lC_library_name);
    }

    /**
     * Registers a model in the factory repository and model info repository.
     *
     * @tparam BaseType base model type
     * @tparam DerivedType derived model type
     * @param aC_info class info in a JSON string
     * @param aC_properties properties in a JSON string
     * @param aC_library_name library this class belongs to
     * @returns whether registration was successful 
     */
    template <class BaseType> template<class DerivedType>
    bool
    ModelFactoryTmpl<BaseType>::RegisterModel(std::string aC_info,
					      std::string aC_properties,
					      std::string
					      aC_library_name)
    {
      boost::scoped_ptr<DerivedType> lCp_proto( new DerivedType() );
      std::string lC_id = efscape::utils::type<DerivedType>(*lCp_proto);

      int li_version = boost::serialization::version<DerivedType>::value;
      
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Registering model class <" << lC_id
		    << "> of version " << li_version
		    << ": model info="
		    << aC_info
		    << "; default model properties ="
		    << aC_properties);
      
      // insert model info
      mCC_ModelInfoMap[lC_id] = ModelInfo(aC_info.c_str(),
					  aC_properties.c_str(),
					  li_version,
					  aC_library_name.c_str());

      // mCF_factories[lC_id] = boost::factory<DerivedType*>();
      
      bool lb_ModelRegistered =
	(mCF_factories.
	 insert( std::make_pair(lC_id,
				boost::factory<DerivedType*>()) ) ).second;
      if (lb_ModelRegistered) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Model class <" << lC_id << "> successfully registered!");

	std::set<std::string> lC1_ModelNames;
	this->ListModels(lC1_ModelNames);
	std::set<std::string>::iterator iter;
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Available models in the repository");
	int li_cnt = 0;
	for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++)
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"=>" << ++li_cnt << ") " << *iter);
      }
      else {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Model class <" << lC_id << "> registration failed!");
      }

      // register model
      return ( lb_ModelRegistered );
    }

    /**
     * Registers a model builder in the builder factory repository.
     *
     * @tparam BaseType base model type
     * @tparam DerivedType derived model builder type
     * @param aC_name builder name
     * @returns whether registration was successful 
     */
    template <class BaseType> template<class DerivedType>
    bool
    ModelFactoryTmpl<BaseType>::registerBuilder(std::string aC_name) {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Registering model builder <" << aC_name << ">...");

      return (mCF_builder_factories.
	      insert( std::make_pair(aC_name,
				     boost::factory<DerivedType*>()) ) ).second;
    }
    
    /**
     * Creates a model builder object from the builder factory repository.
     *
     * @tparam BaseType base model type
     * @param 
     * @returns handle to model_builder
     */
    template <class BaseType>
    typename ModelFactoryTmpl<BaseType>::model_builder*
    ModelFactoryTmpl<BaseType>::createBuilder(std::string aC_name) {
      return efscape::utils::createObject<ModelBuilder>(mCF_builder_factories,
							aC_name);
    }


  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELFACTORY_IPP
