// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.hh
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
#ifndef EFSCAPE_IMPL_MODELHOME_I_HH
#define EFSCAPE_IMPL_MODELHOME_I_HH

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
     * @version 0.1.1 created 24 Dec 2006, revised 08 Dec 2015
     *
     * ChangeLog:
     *   - 2015-12-08 Refactored all factory interfaces (now three separate)
     *   - 2015-11-18 Added factory methods for CommandOpt-derived classes
     *   - 2015-05-31 Removed deprecated model creation methods
     *   - 2015-05-27 Added method for creating model from JSON config
     *   - 2014-09-13 Added methods for loading models with config files 
     *   - 2014-08-12 Added CommandFactory (moved from efscape::utils)
     *   - 2014-07-16 Merged all singletons into ModelHome*
     *   - 2009-09-11 Added class accessor method for a adevs cell factory.
     *   - 2009-03-11 revised
     *   - 2009-02-28 replaced ModelI* return with adevs
     *   - 2006-12-24 created Class
     */
    class ModelHomeI
    {
    public:

      ModelHomeI();
      virtual ~ModelHomeI();

      // methods for creating models
      adevs::Devs<IO_Type>* CreateModel(const char* acp_classname);
      adevs::Devs<IO_Type>* CreateModelFromXML(const char* acp_filename)
	throw(std::logic_error);
      adevs::Devs<IO_Type>* CreateModelFromXML(const std::wstring& aCr_buffer)
	throw(std::logic_error);
      adevs::Devs<IO_Type>* CreateModelFromJSON(const std::string& aCr_JSONstring)
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

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_I_HH
