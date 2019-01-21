// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.cpp
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
#include <efscape/impl/ModelHomeI.hpp> // class declaration

#include <log4cxx/propertyconfigurator.h> // logging

// Include for handling JSON
#include <json/json.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <sstream>
#include <cstdlib>

#include <locale>
#include <codecvt>

namespace fs = boost::filesystem;

namespace efscape {

  namespace impl {

    // instantiate class data
    std::string ModelHomeI::mSC_HomeDir = ".";
    log4cxx::LoggerPtr
    ModelHomeI::mSCp_logger(log4cxx::Logger::getLogger("org.efscape.impl"));

    /** constructor */
    ModelHomeI::ModelHomeI()
    {
      //------------------------------------------
      // set the home directory for Efscape models
      //------------------------------------------
      std::string lC_EfscapeHome = "."; // default location
      char* lcp_env_variable =	// get EFSCAPE_HOME
	std::getenv("EFSCAPE_HOME");

      if ( lcp_env_variable != 0 )
	lC_EfscapeHome = lcp_env_variable;

      std::string lC_logger_config = lC_EfscapeHome
	+ "/log4j.properties";

      log4cxx::PropertyConfigurator::configure(lC_logger_config.c_str());

      LOG4CXX_DEBUG(getLogger(), "Entering application.");

      // set resource path to model resources
      setHomeDir(lC_EfscapeHome.c_str());

      // initialize factories
      mCp_ModelFactory.reset( new model_factory );
      mCp_CommandFactory.reset( new command_factory );

      LOG4CXX_DEBUG(getLogger(), "Created EFSCAPE model respository...");

    } // ModelHomeI::ModelHomeI()

    /** destructor */
    ModelHomeI::~ModelHomeI() {
      LOG4CXX_DEBUG(getLogger(),
      		    "Shutting down EFSCAPE model repository...");
    }

    /**
     * Creates a model from the specified model class name.
     *
     * @param aC_classname model class name
     * @returns smart pointer to model
     */
    DEVSPtr
    ModelHomeI::createModel(std::string aC_classname)
    {
      std::string lC_LogMsg = "ModelHomeI::createModel("
	+ std::string(aC_classname) + "): ";

      // attempt to retrieve model from the model factory
      DEVS* lCp_ModelI = getModelFactory().createObject(aC_classname);
      if (lCp_ModelI == 0) {
	LOG4CXX_ERROR(getLogger(),
		      lC_LogMsg << "model not found");
	return 0;
      }

      LOG4CXX_DEBUG(getLogger(),
      		    lC_LogMsg << "model found...");

      return DEVSPtr(lCp_ModelI);

    } // ModelHomeI::createModel(const char*)

    /**
     * Creates a model from XML serialization stored in a string
     *
     * @param aC_buffer model serialization embedded in a XML string
     * @returns smart pointer to model
     * @throws std::logic_error
     */
    DEVSPtr
    ModelHomeI::createModelFromXML(std::wstring aC_buffer)
	throw(std::logic_error)
    {
      LOG4CXX_DEBUG(getLogger(),
		    "attempting to create model from a JSON configuration...");

      // Load XML contents into a stringstream buffer

      // First need to convert from wstring to string
      // 29 Aug 2017 -- see https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string

      //setup converter
      using convert_type = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_type, wchar_t> converter;

      //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
      std::string converted_str = converter.to_bytes( aC_buffer );

      std::stringstream lC_buffer(converted_str);
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Received XML string =>"
		    << lC_buffer.str());

      return DEVSPtr( loadAdevsFromXML(lC_buffer) );

    } // ModelHomeI::createModelFromXML(std::wstring)

    /**
     * Creates a model from JSON serialization stored in a string
     *
     * @param aC_JSONstring model serialization embedded in a JSON string
     * @returns smart pointer to model
     * @throws std::logic_error
     */
    DEVSPtr
    ModelHomeI::createModelFromJSON(std::string aC_jsonString)
	throw(std::logic_error)
    {
      LOG4CXX_DEBUG(getLogger(),
		    "attempting to create model from a JSON configuration...");

      // Load JSON contents into a stringstream buffer
      std::stringstream lC_buffer(aC_jsonString);
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Received JSON string =>"
		    << lC_buffer.str());

      return loadAdevsFromJSON(lC_buffer);

    } // ModelHomeI::createModelFromJSON(std::string)

    /**
     * Creates a model from a model parameters stored in a JSON
     * string.
     *
     * @param aC_parameters model configuration embedded in a JSON string
     * @returns smart pointer to model
     * @throws std::logic_error
     */
    DEVSPtr
    ModelHomeI::createModelFromParameters(std::string aC_jsonString)
      throw(std::logic_error)
    {
      LOG4CXX_DEBUG(getLogger(),
		    "Attempting to create model from a JSON parameters...");

      // Load JSON contents into a stringstream buffer
      std::stringstream lC_buffer(aC_jsonString);
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Received JSON string =>"
		    << lC_buffer.str());

      // Load JSON from buffer into Json::Value
      Json::Value lC_parameters;
      lC_buffer >> lC_parameters;

      // retrieve <typeName>
      //
      Json::Value lC_modelTypeNameValue = lC_parameters["typeName"];
      if (!lC_modelTypeNameValue.isString()) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Missing <typeName>");
	return DEVSPtr();
      }

      std::string lC_modelTypeName = lC_modelTypeNameValue.asString();

      // retrieve the model properties
      //
      Json::Value lC_modelProperties = lC_parameters["properties"];

      // load and set wrapped model
      //      
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Attempting to create a model of type <"
		    << lC_modelTypeName
		    << "> from the factory");
      DEVSPtr lCp_model( getModelFactory()
			 .createObject( lC_modelTypeName, lC_modelProperties ) );

       if (lCp_model == nullptr) {
         LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Failed to retrieve a model of type <"
          << lC_modelTypeName
          << "> from the factory");
       }

      return lCp_model;

    } // ModelHomeI::createModelFromParameters(std:;string)

    /**
     * Loads the specified library.
     *
     * @param acp_libname name of library to be loaded
     * @throws std::logic_error
     */
    void ModelHomeI::LoadLibrary(const char* acp_libname)
      throw(std::logic_error)
    {
      std::shared_ptr< boost::dll::shared_library >
	lCp_library(new boost::dll::shared_library(acp_libname) );

      mCCp_libraries[std::string(acp_libname)] = lCp_library;

    } // ModelHomeI::LoadLibrary(const char*)

    /**
     * This function dynamically loads libraries in the simulator library
     * directory. All simulation libraries containing self-registering classes
     * (derived subclasses of class Simulator_impl and other SimApp module
     * implementations).
     *
     * @throws std::logic_error
     */
    void ModelHomeI::LoadLibraries()
      throw(std::logic_error)
    {

      //------------------------------
      // get the EFSCAPE registry path
      //------------------------------
      char* lcp_env_variable =	// get EFSCAPE registry path
	getenv("EFSCAPE_REGISTRY");

      if ( lcp_env_variable == NULL) {	// if $EFSCAPE_REGISTRY is not set...
	std::string error_msg = std::string("ModelHomeI::LoadLibrary():")
	  + " *** $EFSCAPE_REGISTRY not set ***\n";
	throw std::logic_error(error_msg);
      }

      //---------------------------------
      // set the shared library extension
      //---------------------------------
      // std::string lC_soext = ".la";
      std::string lC_soext = boost::dll::shared_library::suffix().string();
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "detected extension=" << lC_soext);

      //----------------------------------------------------
      // open the efscape simulation model library directory
      //----------------------------------------------------
      fs::path lC_registry_path =
	fs::path( lcp_env_variable );

      // verify that this is a directory
      if ( !fs::is_directory(lC_registry_path) ) {
	std::string error_msg =
	  "ModelHomeI::LoadLibraries(): can't open <"
	  + lC_registry_path.string() + std::string(">");

	throw std::logic_error(error_msg.c_str());
      }

      //-----------------------------------------------
      // process all the files in the library directory
      //-----------------------------------------------
      fs::directory_iterator end_iter;
      for ( fs::directory_iterator dir_itr( lC_registry_path );
          dir_itr != end_iter;
      	    ++dir_itr ) {

        if ( fs::is_directory( dir_itr->status() ) )
      	  continue;		// skip directories

        if ( !fs::is_regular_file( dir_itr->status() ) )
      	  continue;		// skip if not a regular file

      	std::string lC_filename = dir_itr->path().string();

      	//------------------------
      	// find the file extension
      	//------------------------
	std::string lC_file_extension = dir_itr->path().extension().string();

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "File extensions=<" << lC_file_extension << ">");

      	if (lC_file_extension != lC_soext) // skip if not a shared library
      	  continue;

      	LoadLibrary(lC_filename.c_str()); // attempt to load library
      }

      //-------------------------------------
      // List all the models currently loaded
      //-------------------------------------
      std::set<std::string> lC1_ModelNames =
	getModelFactory().getTypeIDs();
      std::set<std::string>::iterator iter;
      int li_cnt = 0;
      LOG4CXX_DEBUG(getLogger(),
		    "*** Available models: ***");
      for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end();
	   iter++) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "=>" << ++li_cnt << ") " << *iter);

	// get model info
	Json::Value lC_info =
	  getModelFactory().getProperties(*iter);

	std::ostringstream lC_buffer_out;
	lC_buffer_out << lC_info;

	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      lC_buffer_out.str());
      }

    } // ModelHomeI::LoadLibraries()

    /**
     * Returns a reference to the ModelFactory
     *
     * @returns reference to ModelFactory (singleton)
     */
    model_factory& ModelHomeI::getModelFactory() {
      if (mCp_ModelFactory.get() == NULL)
	mCp_ModelFactory.reset( new model_factory );
      return *mCp_ModelFactory;

    }

    /**
     * Returns a reference to the CommandFactory
     *
     * @returns reference to CommandFactory (singleton)
     */
    command_factory& ModelHomeI::getCommandFactory() {
      if (mCp_CommandFactory.get() == NULL)
	mCp_CommandFactory.reset( new command_factory );
      return *mCp_CommandFactory;
    }

    /**
     * Returns smart handle to logger.
     *
     * @returns smart handle to logger
     */
    log4cxx::LoggerPtr& ModelHomeI::getLogger() {
      return mSCp_logger;
    }

    //---------------------------------------
    // end of class ModelHomeI implementation
    //---------------------------------------

  } // namespace impl

} // namespace efscape
