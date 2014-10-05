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
      mCp_factory.reset(new FactoryType);
    }

    /**
     * destructor
     *
     * @tparam BaseType base type of factory
     */
    template <class BaseType>
    ModelFactoryTmpl<BaseType>::~ModelFactoryTmpl() {}

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
	return mCp_factory->CreateObject(acp_name);
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
     * @param acp_info class info
     * @param acp_library_name library this class belongs to
     * @returns whether registration was successful 
     */
    template <class BaseType> template<class DerivedType>
    bool
    ModelFactoryTmpl<BaseType>::RegisterModel(const char* acp_info)
    {
      const char* lcp_library_name = "efscapeimpl";
      RegisterModel((const char*)acp_info,(const char*)lcp_library_name);
    }

    /**
     * Registers a model in the factory repository and model info repository.
     *
     * @tparam BaseType base model type
     * @tparam DerivedType derived model type
     * @param acp_info class info
     * @param acp_library_name library this class belongs to
     * @returns whether registration was successful 
     */
    template <class BaseType> template<class DerivedType>
    bool
    ModelFactoryTmpl<BaseType>::RegisterModel(const char* acp_info,
					      const char*
					      acp_library_name)
    {
      boost::scoped_ptr<DerivedType> lCp_proto( new DerivedType() );
      std::string lC_id = efscape::utils::type<DerivedType>(*lCp_proto);

      int li_version = boost::serialization::version<DerivedType>::value;
      
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Registering model class <" << lC_id
		    << "> of version " << li_version << "...");

      // insert model info
      mCC_ModelInfoMap[lC_id] = ModelInfo(li_version,
					  acp_info,
					  acp_library_name);

      bool lb_ModelRegistered =
	mCp_factory->Register(lC_id, createModel<BaseType,DerivedType>);

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

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELFACTORY_IPP
