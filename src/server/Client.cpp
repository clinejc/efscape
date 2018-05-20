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

int
EfscapeClient::run(int argc, char* argv[])
{
  int li_status = EXIT_SUCCESS;

  std::string lC_ModelName = "";
  std::string lC_ParmName = "";
  
  // server requires either:
  //   - a single parameter: an xml file containing the parameters, or
  if (argc != 2) {
    std::cerr << argv[0] << " usage: " << argv[0]
	      << " <parmfile>\n";
    return EXIT_FAILURE;
  } else {
    lC_ParmName = argv[1];
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

    efscape::ModelNameList lC1_ModelNameList =
      lCp_ModelHome->getModelList();

    std::cout << "Accessing list of available models\n";
    
    for (int i = 0; i < lC1_ModelNameList.size(); i++) {
      std::cout << "mode #" << i << "=<"
		<< lC1_ModelNameList[i]
		<< ">\n";
      std::string lC_ModelInfo =
	lCp_ModelHome->getModelInfo(lC1_ModelNameList[i].c_str());
      std::cout << "\tmodel info=>\n"
		<< lC_ModelInfo
		<< "\n";
    }
    
    // try to load the parameter file
    std::ifstream parmFile(lC_ParmName.c_str());

    // if file can be opened
    std::shared_ptr<efscape::ModelPrx> lCp_Model;
    if ( parmFile ) {
      boost::filesystem::path p =
	boost::filesystem::path(lC_ParmName.c_str());

      LOG4CXX_DEBUG(logger,
		    "Using input parameter file <"
		    << lC_ParmName
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
