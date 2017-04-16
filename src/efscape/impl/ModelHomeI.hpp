// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.hpp
// Copyright (C) 2006-2009 by Jon C. Cline
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Bugs, comments, and questions can be sent to clinej@stanfordalumni.org
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
#define EFSCAPE_IMPL_MODELHOME_I_HPP

#include <efscape/impl/adevs_config.hh>
#include <efscape/impl/ModelBuilder.hh>
#include <efscape/utils/CommandOpt.hh>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <log4cxx/logger.h>

// boost serialization definitions
#include <boost/serialization/version.hpp>
#include <boost/serialization/type_info_implementation.hpp>

#include <efscape/utils/Factory.hpp>
#include <efscape/utils/type.hpp>
#include <string>
#include <map>
#include <stdexcept>

namespace efscape {

  namespace impl {

    // typedefs
    typedef efscape::utils::Factory< std::string, DEVS > model_factory;
    typedef efscape::utils::Factory< std::string, ModelBuilder > builder_factory;
    typedef efscape::utils::Factory< std::string, efscape::utils::CommandOpt > command_factory;
    
    /**
     * Implements the local server-side ModelHome interface, a factory for
     * simulation models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 3.1.0 created 24 Dec 2006, revised 16 Apr 2017
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
      builder_factory& getBuilderFactory();
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

      /*static*/ void LoadLibrary(const char* acp_libname)
	throw(std::logic_error);

      /*static*/ void LoadLibraries()
	throw(std::logic_error);

      static log4cxx::LoggerPtr& getLogger();

    private:

      /** path of resources (on server) */
      static std::string mSC_HomeDir;

      /** smart handle to program log */
      static log4cxx::LoggerPtr mSCp_logger;

      boost::scoped_ptr<model_factory> mCp_ModelFactory;
      boost::scoped_ptr<builder_factory> mCp_BuilderFactory;
      boost::scoped_ptr< command_factory > mCp_CommandFactory;

    };				// class ModelHomeI definition

    /**
     * This utility function loads information from a JSON file.
     * 
     * @param aC_path relative path of the JSON file
     * @return JSON property tree
     */
    boost::property_tree::ptree loadInfoFromJSON(std::string aC_path);
    
  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
