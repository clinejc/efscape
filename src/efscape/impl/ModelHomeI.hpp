// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
#define EFSCAPE_IMPL_MODELHOME_I_HPP

#include <efscape/impl/adevs_config.hpp>
#include <efscape/utils/CommandOpt.hpp>
#include <boost/function.hpp>
#include <log4cxx/logger.h>

// boost serialization definitions
#include <boost/serialization/version.hpp>
#include <boost/serialization/type_info_implementation.hpp>

#include <boost/dll.hpp>	// boost dynamic library

#include <efscape/utils/Factory.hpp>
#include <efscape/utils/type.hpp>
#include <string>
#include <map>
#include <stdexcept>

namespace efscape {

  namespace impl {

    // typedefs
    typedef efscape::utils::Factory< std::string, DEVS > model_factory;
    typedef efscape::utils::Factory< std::string, efscape::utils::CommandOpt > command_factory;
    
    /**
     * Implements the local server-side ModelHome interface, a factory for
     * simulation models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 3.2.0 created 24 Dec 2006, revised 23 Apr 2017
     */
    class ModelHomeI
    {
    public:

      ModelHomeI();
      virtual ~ModelHomeI();

      // methods for creating models
      adevs::Devs<IO_Type>* createModel(const char* acp_classname);
      adevs::Devs<IO_Type>* createModelFromXML(const char* acp_filename)
	throw(std::logic_error);
      adevs::Devs<IO_Type>* createModelFromXML(const std::wstring& aCr_buffer)
	throw(std::logic_error);
      adevs::Devs<IO_Type>* createModelFromJSON(const std::string& aCr_JSONstring)
	throw(std::logic_error);

      model_factory& getModelFactory();
      command_factory& getCommandFactory();

      /**
       * Returns the output path on the server.
       *
       * @returns output path
       */
      static std::string getHomeDir() { return mSC_HomeDir; }

      /**
       * Sets the path of resources on the server.
       *
       * @param acp_path resource path
       */
      void setHomeDir(const char* acp_path) { mSC_HomeDir = acp_path;}

      void LoadLibrary(const char* acp_libname)
	throw(std::logic_error);

      void LoadLibraries()
	throw(std::logic_error);

      static log4cxx::LoggerPtr& getLogger();

    private:

      /** path of resources (on server) */
      static std::string mSC_HomeDir;

      /** smart handle to program log */
      static log4cxx::LoggerPtr mSCp_logger;

      std::unique_ptr<model_factory> mCp_ModelFactory;
      std::unique_ptr< command_factory > mCp_CommandFactory;
      std::map< std::string, std::shared_ptr<boost::dll::shared_library> > mCCp_libraries;

    };				// class ModelHomeI definition

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
