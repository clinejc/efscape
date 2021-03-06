// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.hpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
#define EFSCAPE_IMPL_MODELHOME_I_HPP

#include <efscape/impl/efscapelib.hpp>
#include <efscape/utils/CommandOpt.hpp>
#include <efscape/utils/Factory.hpp>
#include <boost/mpi/communicator.hpp>

#include <log4cxx/logger.h>
#include <boost/dll.hpp>	// boost dynamic library
#include <boost/function.hpp>
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
     * @version 3.3.0 created 24 Dec 2006, revised 20 Aug 2017
     */
    class ModelHomeI
    {
    public:

      ModelHomeI();
      virtual ~ModelHomeI();

      // methods for creating models
      DEVSPtr createModel(std::string aC_classname);
      DEVSPtr createModelFromXML(std::wstring aC_buffer)
	throw(std::logic_error);
      DEVSPtr createModelFromJSON(std::string aC_JSONstring)
	throw(std::logic_error);
      DEVSPtr createModelFromParameters(std::string aC_ParameterString)
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

      void setCommunicator( boost::mpi::communicator* aCp_world ) {
	mCp_world = aCp_world;
      }

      boost::mpi::communicator* getCommunicator() {
	return mCp_world;
      }
      
    private:

      /** path of resources (on server) */
      static std::string mSC_HomeDir;

      /** smart handle to program log */
      static log4cxx::LoggerPtr mSCp_logger;

      /** handle to world */
      boost::mpi::communicator* mCp_world;

      std::unique_ptr<model_factory> mCp_ModelFactory;
      std::unique_ptr< command_factory > mCp_CommandFactory;
      std::map< std::string, std::shared_ptr<boost::dll::shared_library> > mCCp_libraries;

    };				// class ModelHomeI definition

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_I_HPP
