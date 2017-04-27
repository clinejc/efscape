// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/ModelHomeI.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Includes for loading dynamic libraries
#include <efscape/utils/DynamicLibrary.hpp>
#include <ltdl.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <efscape/utils/boost_utils.ipp>


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

      LOG4CXX_DEBUG(getLogger(),
		    "Received JSON string =>"
		    << aCr_JSONstring);

      // read in the JSON data via a property tree extracted from the temp file
      boost::property_tree::ptree pt;
      boost::property_tree::read_json( lC_OutStream, pt );

      BOOST_FOREACH( boost::property_tree::ptree::value_type const& rowPair,
		     pt.get_child( "" ) ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "=> property "
		      << "<" << rowPair.first << ">="
		      << "<" << rowPair.second.get_value<std::string>() << ">");
	boost::optional<std::string> lC_propOpt =
	  pt.get_optional<std::string>(rowPair.first);
	if (!lC_propOpt) {
	  LOG4CXX_DEBUG(getLogger(),
			"property access failed!");
	}
	else
	  LOG4CXX_DEBUG(getLogger(),
			"property access ok!");
      }

      std::string lC_modelName;
      boost::optional<std::string> lC_modelNameOpt =
	pt.get_optional<std::string>("modelName");
      if (lC_modelNameOpt) {
	lC_modelName =
	  ((std::string)lC_modelNameOpt.get());
	LOG4CXX_DEBUG(getLogger(),
		      "property <modelName> found");
      }
      else
	LOG4CXX_ERROR(getLogger(),
		      "property <modelName> not found!");

      adevs::Devs<IO_Type>* lCp_model = NULL;
      if ( (lCp_model = createModel(lC_modelName.c_str()) ) != NULL ) {
	// inject input (model properties file)
	// note: must be done before initializing the simulator
	adevs::Bag<efscape::impl::IO_Type> xb;
	efscape::impl::IO_Type e;
	e.port = "properties_in";
	e.value = pt;
	xb.insert(e);

	inject_events(-0.1, xb, lCp_model);
      }

      return lCp_model;
      
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
      DynamicLibrary* lCp_lib = NULL;
      try {
      	lCp_lib = new DynamicLibrary(acp_libname);
      }
      catch(const std::string& aCr_ErrorMsg) {
      	std::cerr << aCr_ErrorMsg << std::endl;
      }

      if (lCp_lib->handle() != 0) {
      	std::string lC_msg = "ModelHomeI::LoadLibrary(";
      	lC_msg += std::string(acp_libname);
      	lC_msg += "): opened library";
      	LOG4CXX_DEBUG(getLogger(),
      		      lC_msg);
      }

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
      std::string lC_registry_path;

      if ( lcp_env_variable == NULL) {	// if $EFSCAPE_REGISTRY is not set...
	std::string error_msg = std::string("ModelHomeI::LoadLibrary():")
	  + " *** $EFSCAPE_REGISTRY not set ***\n";
	throw std::logic_error(error_msg);
      }

      lC_registry_path = lcp_env_variable;

      //---------------------------------
      // set the shared library extension
      //---------------------------------
      std::string lC_soext = "la";

      //----------------------------------------------------
      // open the efscape simulation model library directory
      //----------------------------------------------------
      fs::path lC_registry_path2 =
	fs::path( lcp_env_variable );

      // verify that this is a directory
      if ( !fs::is_directory(lC_registry_path2) ) {
	std::string error_msg = 
	  "ModelHomeI::LoadLibraries(): can't open <"
	  + lC_registry_path + std::string(">");

	throw std::logic_error(error_msg.c_str());
      }

      //-----------------------------------------------
      // process all the files in the library directory
      //-----------------------------------------------
      fs::directory_iterator end_iter;
      for ( fs::directory_iterator dir_itr( lC_registry_path2 );
          dir_itr != end_iter;
      	    ++dir_itr ) {

        if ( fs::is_directory( dir_itr->status() ) )
      	  continue;		// skip directories

        if ( !fs::is_regular_file( dir_itr->status() ) )
      	  continue;		// skip if not a regular file

      	std::string lC_filename = dir_itr->path().string();//filename().c_str();

      	//------------------------
      	// find the file extension
      	//------------------------
      	std::string lC_file_extension;
      	std::string::size_type pos = 0;
      	std::string::size_type startpos = 0;

      	while ( (pos=lC_filename.find_first_of(".",pos)) != std::string::npos) {
      	  ++pos;
      	  startpos = pos;
      	}

      	if ( startpos != 0 || startpos != std::string::npos) {
      	  lC_file_extension = lC_filename.substr(startpos,lC_filename.length());
      	}
      	else
      	  continue;

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
      for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++)
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "=>" << ++li_cnt << ") " << *iter);

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

    // utility function for loading info from a JSON file
    boost::property_tree::ptree loadInfoFromJSON(std::string aC_path) {
      // path is relative
      std::string lC_FileName =
	ModelHomeI::getHomeDir() + std::string("/") + aC_path;

      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "model info path = <" << lC_FileName << "> ***");

      boost::property_tree::ptree pt;
      std::string lC_JsonStr = "{}";
      try {
	boost::property_tree::read_json( lC_FileName.c_str(), pt );

	std::ostringstream lC_buffer;
	boost::property_tree::write_json(lC_buffer, pt, false);
	lC_JsonStr = lC_buffer.str();
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "JSON=>" << lC_JsonStr);
      }
      catch (...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "unknown exception occurred parsing model info JSON file.");
      }

      return pt;
    }


  } // namespace impl

} // namespace efscape
