// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.cc
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
#include <efscape/impl/ModelHomeI.hh>

#include <efscape/impl/ModelFactory.hpp>
#include <efscape/impl/ModelBuilder.hh>
#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <cstdlib>

// Includes for loading dynamic libraries
#include <efscape/utils/DynamicLibrary.hh>
#include <ltdl.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

// ---------------------------------------------------------------------------
//  beginning of Xerces-c C++ XML Includes
// ---------------------------------------------------------------------------
#include <xercesc/dom/DOM.hpp>
#include <efscape/utils/xerces_utils.hpp>
#include <efscape/utils/xerces_strings.hpp>


#include <efscape/impl/EfscapeBuilder.hh>

// using namespace repast;
using namespace xercesc;
using namespace efscape::utils;
namespace fs = boost::filesystem;

namespace efscape {

  namespace impl {

    // template class ModelFactoryTmpl<DEVS>;

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
      mCp_ModelFactory.reset( new ModelFactory );
      mCp_CommandFactory.reset( new CommandFactory );

      std::cout << "Created EFSCAPE model respository...\n";
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
    ModelHomeI::CreateModel(const char* acp_classname)
    {
      std::string lC_LogMsg = "ModelHomeI::CreateModel("
	+ std::string(acp_classname) + "): ";

      // attempt to retrieve model from the model factory
      DEVS* lCp_ModelI = GetModelFactory().CreateModel(acp_classname);
      if (lCp_ModelI == 0) {
	LOG4CXX_ERROR(getLogger(),
		      lC_LogMsg << "model not found");

	std::set<std::string> lC1_ModelNames;
	GetModelFactory().ListModels(lC1_ModelNames);
	std::set<std::string>::iterator iter;
	std::cout << "*** Available models: ***\n";
	for (iter = lC1_ModelNames.begin(); iter != lC1_ModelNames.end(); iter++)
	  std::cout << "\t" << *iter << std::endl;

	delete lCp_ModelI;
	return 0;
      }

      LOG4CXX_DEBUG(getLogger(),
      		    lC_LogMsg << "model found...");

      return dynamic_cast<adevs::Devs<IO_Type>* >(lCp_ModelI);

    } // ModelHomeI::CreateModel(const char*)

    /**
     * Creates a model from a model configuration stored in a string
     * buffer.
     *
     * @param aCr_buffer buffer containing the model configuration
     * @returns handle to model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::CreateModelFromStr(const std::wstring& aCr_buffer)
      throw(std::logic_error)
    {
      // create a temporary file to store the simulation configuration
      // char* lcp_FileName = NULL;
      // lcp_FileName = tmpnam(NULL);
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
	return this->CreateModelFromFile(lcp_FileName);
      }
      catch(std::logic_error excp) {
	throw excp;
      }

    } // ModelHomeI::CreateModelFromStr(const std::wstring&)

    /**
     * Creates a model from a model configuration stored in an XML file.
     *
     * @param acp_filename name of configuration file
     * @returns handle to root model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::CreateModelFromFile(const char* acp_filename)
      throw(std::logic_error)
    {
      std::string lC_message = "ModelHomeI::CreateModelFromFile("
	+ std::string(acp_filename) + "):";

      adevs::Devs<IO_Type>* lCp_model = NULL;
      boost::scoped_ptr<ModelBuilder> lCp_builder;

      //--------------------------------------------
      // parse the file with the xerces-c DOM parser
      //--------------------------------------------
      try {
	XercesInitializer lC_init;
	xercesc::DOMImplementation*  lCp_DOMimpl = 
	  DOMImplementationRegistry::getDOMImplementation(
                fromNative("LS").c_str( )
            );
        if (lCp_DOMimpl == 0) {
	  lC_message += "\n\tcouldn't create DOM implementation";
	  throw std::logic_error(lC_message.c_str());
        }

        // Construct a DOMLSParser to parse simulation parameter file
        DOMLSParser* lC_parser ( 
	  static_cast<xercesc::DOMImplementationLS*>(lCp_DOMimpl)->
	  createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0) );

	//---------------------------------------------------------------------
        // Parse simulation configuration xml file
	//  - top tag should be "boost_serialization
	//  - first child element tag should be "efscape"
	//  - first element tag <class_name> should be name of root model class
	//---------------------------------------------------------------------
        DOMDocument* lCp_doc = 
	  lC_parser->parseURI(acp_filename);

	DOMElement* lCp_SimConfig = lCp_doc->getDocumentElement();

	// 1) top tag is used to identify the appropriate parser/builder
	std::string lC_TagName = toNative(lCp_SimConfig->getTagName());

	lCp_builder.reset( TheBuilderFactory::Instance().
			   CreateObject(lC_TagName) );

	if (!lCp_builder.get()) {
	  lC_message += "\n\tbad document root <" + lC_TagName
	    + "> -- unable to local parser for this XML file";
	  throw std::logic_error(lC_message.c_str());
	}

	LOG4CXX_DEBUG(getLogger(),
		      lC_message
		      << " retrieved model builder <" << lC_TagName
		      << ">");
      }
      catch(const xercesc::XMLException &toCatch) {
	lC_message += std::string(" Error during Xerces-c Initialization.\n")
	  + "  Exception message:"
	  + ::toNative(toCatch.getMessage());

	throw std::logic_error(lC_message.c_str());
      }
      catch (const std::logic_error& excp) {
	throw excp;
      }

      lCp_model = lCp_builder->parse(acp_filename); // create root model
      if (!lCp_model) {
	lC_message += " unable to create model from parameter file";
	throw std::logic_error(lC_message.c_str());
      }

      return lCp_model;

    } // ModelHomeI::CreateModelFromFile(...)

    /**
     * Creates a model from a model configuration stored in an XML file.
     *
     * @param acp_classname model class name
     * @param acp_propsfile name of configuration file
     * @returns handle to root model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::CreateModelWithConfig(const char* acp_classname,
				      const char* acp_propsfile)
      throw(std::logic_error)
    {
      std::string lC_message = "ModelHomeI::CreateModelWithConfig("
	+ std::string(acp_classname)
	+ std::string(",")
	+ std::string(acp_propsfile) + "):";

      adevs::Devs<IO_Type>* lCp_model = NULL;
      if ( (lCp_model = CreateModel(acp_classname) ) != NULL ) {
	// inject input (model properties file)
	// note: must be done before initializing the simulator
	adevs::Bag<efscape::impl::IO_Type> xb;
	efscape::impl::IO_Type e;
	e.port = "properties_in";
	e.value = std::string(acp_propsfile);
	xb.insert(e);

	inject_events(-0.1, xb, lCp_model);
      }

      return lCp_model;

    } // ModelHomeI::CreateModelWithConfig(const char*,const char*)

    /**
     * Creates a model from a model configuration stored in an XML file.
     *
     * @param acp_classname model class name
     * @param aCr_propsfile string containing configuration file
     * @returns handle to root model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    ModelHomeI::CreateModelWithConfig(const char* acp_classname,
				      const std::wstring& aCr_propsfile)
      throw(std::logic_error)
    {
      // create a temporary file to store the simulation configuration
      // char* lcp_FileName = NULL;
      // lcp_FileName = tmpnam(NULL);
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
      lC_OutFile << aCr_propsfile;
      lC_OutFile.close();

      try {
	return this->CreateModelWithConfig(acp_classname,
					   lcp_FileName);
      }
      catch(std::logic_error excp) {
	throw excp;
      }

    } // ModelHomeI::CreateModelWithConfig(const char*, const std::wstring&)

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
 
    } // ModelHomeI::LoadLibraries()

    /**
     * Returns a reference to the ModelFactory
     *
     * @returns reference to ModelFactory (singleton)
     */
    ModelFactory& ModelHomeI::GetModelFactory() {
      if (mCp_ModelFactory.get() == NULL)
	mCp_ModelFactory.reset( new ModelFactory );
      return *mCp_ModelFactory;
    }

    /**
     * Returns a reference to the CommandFactory
     *
     * @returns reference to the CommandFactory (singleton)
     */
    CommandFactory& ModelHomeI::TheCommandFactory() {
      if (mCp_CommandFactory.get() == NULL)
	mCp_CommandFactory.reset( new CommandFactory );

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

  } // namespace impl

} // namespace efscape
