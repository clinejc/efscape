// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelFactory.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELFACTORY_HPP
#define EFSCAPE_IMPL_MODELFACTORY_HPP

// model implementation definition
#include <efscape/impl/adevs_config.hh>

#include <efscape/impl/ModelBuilder.hh>
#include <set>
#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

namespace efscape {

  namespace impl {

    /**
     * A simple structure for holding information about a model
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.2 created 03 Mar 2009, revised 05 June 2015
     */
    struct ModelInfo {

      ModelInfo() :
	info("{}"),
	properties("{}"),
	version(1),
	library_name("") {}

      ModelInfo(const char* acp_info,
		const char* acp_properties,
		int ai_version,
		const char* acp_library_name = "efscape") :
	info(acp_info),
	properties(acp_properties),
	version(ai_version),
	library_name(acp_library_name) {}

      // data members
      std::string info;
      std::string properties;
      int version;
      std::string library_name;
    };

    /**
     * This utility function loads information from a JSON file.
     * 
     * @return JSON string
     */
    std::string loadInfoFromJSON(const char* acp_path);
    
    /**
     * Implements a factory for simulation models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.1.0 created 10 Sep 2009, revised 18 Nov 2015
     * @tparam BaseType base type of factory
     */
    template <class BaseType>
    class ModelFactoryTmpl
    {
    public:

      typedef boost::function< BaseType*() > model_factory;
      typedef std::map< std::string, model_factory > model_factory_map;

      typedef ModelBuilderTmpl<BaseType> model_builder;
      typedef boost::function< model_builder*() > builder_factory;
      typedef std::map< std::string, builder_factory > builder_factory_map;
      
      ModelFactoryTmpl();
      ~ModelFactoryTmpl();

      BaseType* CreateModel(const char* acp_classname);

      // methods for registering models and listing model info
      template <class DerivedType>
      bool RegisterModel(std::string aC_info);
      template <class DerivedType>
      bool RegisterModel(std::string aC_info  = std::string("{}"),
			 std::string acp_properties = std::string("{}"),
			 std::string acp_library_name = std::string("efscapeimpl"));
      void ListModels(std::set<std::string>& aC1r_ModelNames);
      void ListModels(std::set<std::string>& aC1r_ModelNames,
		      bool(*aFp_ModelMatch)(const BaseType*));

      ModelInfo getModelInfo(const char* acp_classname);

      // methods supporting the underlying model builder factory
      template <class DerivedType>
      bool registerBuilder(std::string aC_name);
      model_builder* createBuilder(std::string aC_name);
      
    protected:

      /** model factory */
      model_factory_map mCF_factories;

      /** model info map */
      std::map< std::string, ModelInfo > mCC_ModelInfoMap;

      /** model builder factories */
      builder_factory_map mCF_builder_factories;

    };				// class ModelFactoryTmpl<>

    // Model Factories for specific base model types
    typedef ModelFactoryTmpl<DEVS> ModelFactory;

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELFACTORY_HPP
