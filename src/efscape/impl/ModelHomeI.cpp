// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/ModelHomeI.hpp>

// Includes for loading dynamic libraries
#include <efscape/utils/DynamicLibrary.hpp>

// Include for handling JSON
#include <efscape/impl/adevs_json.hpp>
#include <efscape/utils/boost_utils.ipp>
#include <boost/property_tree/json_parser.hpp>
#include <json/json.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <sstream>
#include <cstdlib>

using namespace efscape::utils;
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
      LOG4CXX_INFO(mSCp_logger, "Entering application.");

      //------------------------------------------
      // set the home directory for Efscape models
      //------------------------------------------
      std::string lC_EfscapeHome = "."; // default location
      char* lcp_env_variable =	// get EFSCAPE_HOME
	getenv("EFSCAPE_HOME");

      if ( lcp_env_variable != 0 )
	lC_EfscapeHome = lcp_env_variable;

      // set resource path to model resources
      setHomeDir(lC_EfscapeHome.c_str());

      // initialize factories
      mCp_ModelFactory.reset( new model_factory );
      mCp_CommandFactory.reset( new command_factory );

      LOG4CXX_INFO(mSCp_logger, "Created EFSCAPE model respository...");
      
    } // ModelHomeI::ModelHomeI()

    /** destructor */
    ModelHomeI::~ModelHomeI() {
      LOG4CXX_DEBUG(getLogger(),
      		    "Shutting down EFSCAPE model repository...");
    }

    /**
     * Creates a model from the specified model class name.
     *
     * @param acp_classname model class name
     * @returns handle to model
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::createModel(const char* acp_classname)
    {
      std::string lC_LogMsg = "ModelHomeI::createModel("
	+ std::string(acp_classname) + "): ";

      // attempt to retrieve model from the model factory
      DEVS* lCp_ModelI = getModelFactory().createObject(acp_classname);
      if (lCp_ModelI == 0) {
	LOG4CXX_ERROR(getLogger(),
		      lC_LogMsg << "model not found");

	std::set<std::string> lC1_ModelNames =
	  getModelFactory().getTypeIDs();
	std::set<std::string>::iterator iter;
	LOG4CXX_DEBUG(getLogger(),
		      "*** Available models: ***");
	for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++)
	  LOG4CXX_DEBUG(getLogger(),
			"=> modelName=" << *iter);
	return 0;
      }

      LOG4CXX_DEBUG(getLogger(),
      		    lC_LogMsg << "model found...");

      return dynamic_cast<adevs::Devs<IO_Type>* >(lCp_ModelI);

    } // ModelHomeI::createModel(const char*)

    /**
     * Creates a model from a model configuration stored in a string
     * buffer.
     *
     * @param aCr_buffer buffer containing the model configuration
     * @returns handle to model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::createModelFromXML(const std::wstring& aCr_buffer)
      throw(std::logic_error)
    {
      char lcp_FileName[] = "/tmp/fileXXXXXX";
      int fd;			// file descriptor
      
      if ( (fd = mkstemp(lcp_FileName) ) == -1)
	throw std::logic_error("Unable to open temporary file");

      std::wofstream lC_OutFile(lcp_FileName);
      if (!lC_OutFile) {
	std::string lC_ErrorMsg = "Unable to open temporary file name <"
	  + std::string(lcp_FileName) + ">";

	throw std::logic_error(lC_ErrorMsg.c_str());
      }

      // write buffer to the temporary file
      lC_OutFile << aCr_buffer;
      lC_OutFile.close();

      try {
	return this->createModelFromXML(lcp_FileName);
      }
      catch(std::logic_error excp) {
	throw excp;
      }

    } // ModelHomeI::createModelFromXML(const std::wstring&)

    /**
     * Creates a model from a model configuration stored in an XML file.
     *
     * @param acp_filename name of configuration file
     * @returns handle to root model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::createModelFromXML(const char* acp_filename)
      throw(std::logic_error)
    {
      std::string lC_message = "ModelHomeI::createModelFromXML("
	+ std::string(acp_filename) + "):";

      // attempt to load adevs model serialized in XML format
      adevs::Devs<IO_Type>* lCp_model =
	loadAdevs(acp_filename); // create root model
      if (!lCp_model) {
	lC_message += " unable to create model from parameter file";
	throw std::logic_error(lC_message.c_str());
      }

      return lCp_model;

    } // ModelHomeI::createModelFromXML(...)

    /**
     * Creates a model from a model configuration stored in a JSON
     * string.
     *
     * @param aCr_JSONstring model configuration embedded in a JSON string
     * @returns handle to model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::createModelFromJSON(const std::string& aCr_JSONstring)
	throw(std::logic_error)
    {
      LOG4CXX_DEBUG(getLogger(),
		    "Attempting to create model from a JSON configuration...");
      std::stringstream lC_OutStream;
      if (!lC_OutStream) {
	std::string lC_ErrorMsg = "Unable to open string stream";

	throw std::logic_error(lC_ErrorMsg.c_str());
      }

      // write buffer to the temporary file
      lC_OutStream << aCr_JSONstring;

      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		     "Received JSON string =>"
		     << aCr_JSONstring);

      // read in the JSON data
      istringstream lC_buffer_in(aCr_JSONstring.c_str());

      Json::Value lC_config;
      lC_buffer_in >> lC_config;

      return efscape::impl::createModelFromJSON(lC_config);
      
    } // ModelHomeI::createModelFromJSON(const char*)
    
    /**
     * Loads the specified library.
     *
     * @param acp_libname name of library to be loaded
     * @throws std::logic_error
     */
    void ModelHomeI::LoadLibrary(const char* acp_libname)
      throw(std::logic_error)
    {
      boost::shared_ptr< boost::dll::shared_library >
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
     * @returns reference to logger
     */
    log4cxx::LoggerPtr& ModelHomeI::getLogger() {
      return mSCp_logger;
    }

    //---------------------------------------
    // end of class ModelHomeI implementation
    //---------------------------------------

  } // namespace impl

} // namespace efscape
