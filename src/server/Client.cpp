// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Client.cpp
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
//=============================================================================

#include <Ice/Ice.h>
#include <efscape/ModelHome.h>
#include <efscape/Model.h>
#include <efscape/Simulator.h>
#include <fstream>
#include <sstream>
#include <cfloat>
#include <stdexcept>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <json/json.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

// Create logger
log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.efscape.client"));

/**
 * Catches exceptions to handle known on the exit.
 */
void singleton_terminate(void) {
  // static bool tried_throw = false;
  static int tried_throw = 0;

  try {
    // if (!tried_throw++) throw;
    if ( (tried_throw++) > 0) throw;

        std::cout << "no active exception" << std::endl;
  }
  catch (const std::logic_error& err) {
    std::cerr << "Caught logic error: " << err.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception occurred." << std::endl;
  }
  exit(0);			// ensure a clean exit
}

namespace {
  static const bool SET_TERMINATE =
    std::set_terminate(singleton_terminate);
}

class EfscapeClient : public Ice::Application
{
public:

  virtual int run(int, char*[]);
};

int
main(int argc, char* argv[])
{
  EfscapeClient app;

  log4cxx::BasicConfigurator::configure();

  // attempt to retrieve the root directory of efscape ICE configuration
  std::string lC_EfscapeIcePath = "."; // default location
  char* lcp_env_variable =	// get EFSCAPE_HOME
    getenv("EFSCAPE_HOME");

  if ( lcp_env_variable != 0 )
    lC_EfscapeIcePath = lcp_env_variable;

  std::string lC_config = lC_EfscapeIcePath
    + "/config.client";
  LOG4CXX_DEBUG(logger,
		"efscape ICE server config=<" << lC_config << ">");

  return app.main(argc, argv, lC_config.c_str());
}

void menu();

int
EfscapeClient::run(int argc, char* argv[])
{
  int li_status = EXIT_SUCCESS;
  std::string lC_parmName = "";

  //----------------------------------------------------------------------------
  // server has a single optional command, the name of a model parameter:
  //     1. Model parameter for in JSON format (see model metadata)
  //     2. Cereal serialization JSON format
  //
  // If an input file is not specified, the user will be prompted to
  // select one of the available models, from which a valid parameter file
  // will be generated.
  //----------------------------------------------------------------------------
  if (argc > 2) {
    std::cerr << argv[0] << " usage: " << argv[0]
	      << " [parmfile]\n";
    return EXIT_FAILURE;
  } else if (argc == 2) {
    lC_parmName = argv[1];
  }

  try {

    // get handle to model home (factory)
    auto lCp_ModelHome =
      Ice::checkedCast<efscape::ModelHomePrx>(communicator()
					      ->propertyToProxy("ModelHome.Proxy")
					      ->ice_twoway()
					      ->ice_timeout(-1)
					      ->ice_secure(false));
    
    if (!lCp_ModelHome)
      throw "Invalid ModelHome proxy";

    LOG4CXX_DEBUG(logger,
		  "ModelHome accessed successfully!");

    // If no input file has been specified, display a list of all models
    // currently loaded, and allow the user to select a model and output
    // a default model parameter file.
    if (lC_parmName == "") {
      efscape::ModelNameList lC1_modelList =
	lCp_ModelHome->getModelList();

      std::cout << "** List of available models ***\n";
    
      for (int i = 0; i < lC1_modelList.size(); i++) {
	std::cout << i << ": "
		  << lC1_modelList[i]
		  << "\n";

      }

      // menu options
      std::cout << "\nEnter the number of the model (0 to exit)==> ";

      // process user input
      int li_userInput = 0;
      std::string lC_parmString = "";
      do {
	std::cin >> li_userInput;
	if (li_userInput > 0 && li_userInput <= lC1_modelList.size()) {
	  std::string lC_modelName = lC1_modelList[li_userInput - 1];
	  std::cout << "Selected model <"
		    << lC_modelName << ">\n";

	  lC_parmName =
	    boost::replace_all_copy(lC_modelName,
				    ":",
				    "_");
	  std::cout << "lC_parmName = <" << lC_parmName << ">\n";

	  lC_parmString =
	    lCp_ModelHome->getModelInfo(lC_modelName.c_str());
	  
	} else if ( li_userInput > lC1_modelList.size() ) {
	  std::cout << "Model index <" << li_userInput << "> out of bounds\n";
	  lC_parmName = "";
	  lC_parmString = "";
	}
      } while(li_userInput > 0);

      if (lC_parmName != "") {
	// Write JSON string into a buffer and read the buffer into a JSON
	// object so that an attribute may be added
	std::stringstream lC_buffer(lC_parmString);
	Json::Value lC_jsonParameters;
	lC_buffer >> lC_jsonParameters;

	// add a "modelName" attribute
	lC_jsonParameters["modelName"] = lC_parmName;

	// Write the JSON string back into the cleared buffer
	lC_buffer.clear();
	lC_buffer << lC_jsonParameters;
	lC_parmString = lC_buffer.str(); // update the parm string
	
	// save the model parameter JSON string to a file
	lC_parmName += ".json";
	std::ofstream parmFile(lC_parmName.c_str());
	parmFile << lC_parmString << std::endl;
      }
      return EXIT_SUCCESS;
    }

    // try to load the parameter file
    std::ifstream parmFile(lC_parmName.c_str());

    // if file can be opened
    std::shared_ptr<efscape::ModelPrx> lCp_Model;
    if ( parmFile ) {
      boost::filesystem::path p =
	boost::filesystem::path(lC_parmName.c_str());

      LOG4CXX_DEBUG(logger,
		    "Using input parameter file <"
		    << lC_parmName
		    << "> with file extension <"
		    << p.extension()
		    << ">");

      // convert file to a string      
      if (p.extension().string() == ".json") {
	std::ostringstream buf;
	char ch;
	while (buf && parmFile.get( ch ))
	  buf.put( ch );
	LOG4CXX_DEBUG(logger,
		      "JSON buffer=>");
	LOG4CXX_DEBUG(logger,
		      buf.str() );

	// first try to create model from JSON ser
	lCp_Model = lCp_ModelHome->createFromJSON(buf.str());
	if (lCp_Model == nullptr) {
	  lCp_Model = lCp_ModelHome->createFromParameters(buf.str());
	}	
      }
    } else {
      throw "Unable to open parameter file\n";
    }

    if (lCp_Model == nullptr) {
      throw "Invalid Model proxy";
    }

    LOG4CXX_DEBUG(logger,
		  "Model created!");

    // get a simulator for the model
    auto lCp_Simulator =
      lCp_ModelHome->createSim(lCp_Model);

    if (!lCp_Simulator)
      throw "Invalid Simulator proxy";
      
    LOG4CXX_DEBUG(logger,
		  "Simulator created!");

    double ld_time = 0.0;

    if (lCp_Simulator->start()) {

      // get initial output from the model
      efscape::Message lC_message =
	lCp_Model->outputFunction();

      // get a handle to the simulation model clock
      for (int i = 0; i < lC_message.size(); i++) {
	LOG4CXX_DEBUG(logger,
		      "message " << i << ": value on port <"
		      << lC_message[i].port << "> = "
		      << lC_message[i].valueToJson);

      }

      while ( (ld_time = lCp_Simulator->nextEventTime()) < DBL_MAX/*lCp_Clock->timeMax()*/) {
	lCp_Simulator->execNextEvent();
	efscape::Message lC_message =
	  lCp_Model->outputFunction();
	if (lC_message.size() > 0) {
	  LOG4CXX_DEBUG(logger,
			"time step = " << ld_time
			<< ", message size = "
			<< lC_message.size() );
	  for (int i = 0; i < lC_message.size(); i++) {
	    LOG4CXX_DEBUG(logger,
			  "message " << i << ": value on port <"
			  << lC_message[i].port << "> = "
			  << lC_message[i].valueToJson);
	  }
	}
      }

    }
    else {
      std::cerr << "Unable to start simulation\n";
      li_status = 1;
    }

    lCp_Simulator->destroy();
    lCp_Model->destroy();

  } catch (const Ice::Exception& excp) {
    std::cerr << excp << std::endl;
    li_status = 1;
  }

  // Ice::collectGarbage();

  return li_status;
}

void menu() {
  std::cout <<
    "models:\n";
}
