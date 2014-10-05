// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Client.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
//=============================================================================

#include <Ice/Ice.h>
#include <efscape/efscape.h>
#include <fstream>
#include <sstream>
#include <cfloat>
#include <stdexcept>

/**
 * Catches exceptions to handle known on the exit.
 */
void singleton_terminate(void) {
  static bool tried_throw = false;

  try {
    if (!tried_throw++) throw;
        std::cout << "no active exception" << std::endl;
  }
  catch (const std::logic_error& err) {
    std::cout << "Caught logic error: " << err.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception occurred." << std::endl;
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

  // attempt to retrieve the root directory of efscape ICE configuration
  std::string lC_EfscapeIcePath = "."; // default location
  char* lcp_env_variable =	// get EFSCAPE_HOME
    getenv("EFSCAPE_HOME");

  if ( lcp_env_variable != 0 )
    lC_EfscapeIcePath = lcp_env_variable;

  std::string lC_config = lC_EfscapeIcePath
    + "/config.client";
  std::cout
    << "efscape ICE server config=<" << lC_config << ">\n";

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
  //   - two parameters: the model class name and a text parameter file
  if (argc < 2 || argc > 3) {
    std::cerr << argv[0] << " usage: " << argv[0]
	      << " <parmfile>\n";
    return EXIT_FAILURE;
  } else if (argc == 2) {
    lC_ParmName = argv[1];
  } else {			// argv == 3
    lC_ModelName = argv[1];
    lC_ParmName = argv[2];
  }

  try {

    // get handle to model home (factory)
    efscape::ModelHomePrx lCp_ModelHome
      = efscape::ModelHomePrx::checkedCast(communicator()
					   ->propertyToProxy("ModelHome.Proxy")
					   ->ice_twoway()
					   ->ice_timeout(-1)
					   ->ice_secure(false));
    if (!lCp_ModelHome)
      throw "Invalid proxy";

    std::cout << "ModelHome accessed successfully!\n";

    efscape::ModelPrx lCp_Model;

    // processing argument
    std::string lC_ParmName = argv[1];

    // try to load the parameter file
    std::ifstream parmFile(lC_ParmName.c_str());

    // if file can be opened
    std::wostringstream buf;
    if ( parmFile ) {
      // convert file to string
      char ch;
      while (buf && parmFile.get( ch ))
	buf.put( (wchar_t)ch );
    }
    if (lC_ModelName == "") {	// loading model from xml file
      lCp_Model = lCp_ModelHome->createFromXML(buf.str());
    } else {
      lCp_Model = lCp_ModelHome->createWithConfig(lC_ModelName,
						  buf.str());
    }

    if (!lCp_Model) {
      throw "Invalid Model proxy";
    }

    std::cout << "Model created!\n";

    // get a simulator for the model
    efscape::SimulatorPrx lCp_Simulator =
      lCp_ModelHome->createSim(lCp_Model);

    if (!lCp_Simulator)
      throw "Invalid Simulator proxy";
      
    std::cout << "Simulator created!\n";

    double ld_time = 0.0;

    if (lCp_Simulator->start()) {

      // get initial output from the model
      efscape::Message lC_message;
      efscape::ClockPrx lCp_Clock;

      lC_message = lCp_Model->outputFunction();

      // get a handle to the simulation model clock
      for (int i = 0; i < lC_message.size(); i++) {
	std::cout << "message " << i << ": value on port <"
		  << lC_message[i]->port << ">\n";
	if (lC_message[i]->port == "clock_out") {
	  lCp_Clock =
	    efscape::ClockPrx::checkedCast(lC_message[i]->value);
	}
      }

      // if (!lCp_Clock)
      // 	throw "Invalid Clock proxy";

      // std::cout << "\tstart time:\t" << lCp_Clock->timeCurrent() << "\n"
      // 		<< "\tend time:\t" << lCp_Clock->timeMax() << "\n"
      // 		<< "\ttime delta:\t" << lCp_Clock->timeDelta() << "\n";

      while ( (ld_time = lCp_Simulator->nextEventTime()) < DBL_MAX/*lCp_Clock->timeMax()*/) {
	lCp_Simulator->execNextEvent();
	efscape::Message lC_message;
	lC_message = lCp_Model->outputFunction();
	if (lC_message.size() > 0) {
	  std::cout << "time step = " << ld_time
		    << ", message size = "
		    << lC_message.size() << std::endl;
	  // for (int i = 0; i < lC_message.size(); i++) {
	  //   ::efscape::data::DataFramePrx lCp_DataFrame =
	  //     ::efscape::data::DataFramePrx::checkedCast(lC_message[i]->value);
	  //   if (lCp_DataFrame)
	  //     std::cout << "observer <" << lCp_DataFrame->getName()
	  // 		<< ">, number of rows = "
	  // 		<< lCp_DataFrame->numRows() << "\n";
	  // }
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

  Ice::collectGarbage();

  return li_status;
}
