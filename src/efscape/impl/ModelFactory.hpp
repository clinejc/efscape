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

#include <loki/Factory.h>
#include <set>
#include <map>
#include <boost/scoped_ptr.hpp>

namespace efscape {

  namespace impl {

    // typedefs
    typedef std::string ModelKeyType;
    typedef ::Loki::Factory<DEVS, ModelKeyType> DevsFactory;
    typedef ::Loki::Factory<CellDevs, ModelKeyType> CellDevsFactory;

    /**
     * A simple structure for holding information about a model
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 03 Mar 2009, revised 03 Mar 2009
     */
    struct ModelInfo {

      ModelInfo() :
	version(1),
	info(""),
	library_name("") {}

      ModelInfo(int ai_version,
		const char* acp_info,
		const char* acp_library_name = "efscape") :
	version(ai_version),
	info(acp_info),
	library_name(acp_library_name) {}

      // data members
      int version;
      std::string info;
      std::string library_name;
    };

    /**
     * This template function create an adevs DEVS object of type T.
     *
     * @returns handle to new adevs DEVS object of derived type T
     */
    template < class BaseType, class DerivedType >
    inline BaseType* createModel() {
      return ( dynamic_cast< BaseType* >( new DerivedType) );
    }


    /**
     * Implements a factory for simulation models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.3 created 10 Sep 2009, revised 24 Sep 2014
     */
    template <class BaseType>
    class ModelFactoryTmpl
    {
    public:

      typedef ::Loki::Factory<BaseType,ModelKeyType> FactoryType;

      ModelFactoryTmpl();
      ~ModelFactoryTmpl();

      BaseType* CreateModel(const char* acp_classname);

      // methods for registering models and listing model info
      template <class DerivedType>
      bool RegisterModel(const char* acp_info);
      template <class DerivedType>
      bool RegisterModel(const char* acp_info  = "",
			 const char* acp_library_name = "efscapeimpl");
      void ListModels(std::set<std::string>& aC1r_ModelNames);
      void ListModels(std::set<std::string>& aC1r_ModelNames,
		      bool(*aFp_ModelMatch)(const BaseType*));

      ModelInfo getModelInfo(const char* acp_classname);

      /**
       * Returns reference to underlying factory.
       *
       * @returns reference to underlying factory
       */
      FactoryType& GetFactory() { return *mCp_factory; }

    protected:

      /** model factory */
      boost::scoped_ptr<FactoryType> mCp_factory;

      /** model info map */
      std::map< std::string, ModelInfo > mCC_ModelInfoMap;

    };				// class ModelFactoryTmpl<>


    /**
     * Implements a factory for simulation models based on adevs.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.4 created 07 Jan 2007, revised 10 Sep 2009
     */
    class ModelFactory : public ModelFactoryTmpl<DEVS>
    {
    public:

      ModelFactory();
      ~ModelFactory();

    };				// class ModelFactory definition

    /**
     * Implements a factory for simulation models based on adevs cell models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 11 Sep 2009
     */
    class CellModelFactory : public ModelFactoryTmpl<CellDevs>
    {
    public:

      CellModelFactory();
      ~CellModelFactory();

    };				// class CellModelFactory definition

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELFACTORY_HPP
