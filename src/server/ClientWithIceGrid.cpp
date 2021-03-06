// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Client.cpp
// Copyright (C) 2006-2019 Jon C. Cline
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
// RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
// CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
//=============================================================================

#include <Ice/Ice.h>
#include <IceGrid/IceGrid.h>
#include <efscape/Model.h>
#include <efscape/ModelHome.h>
#include <efscape/Simulator.h>
#include <json/json.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <cfloat>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>  // logging

namespace fs = boost::filesystem;

// Create logger
log4cxx::LoggerPtr gCp_logger(log4cxx::Logger::getLogger("org.efscape.client"));

/**
 * Helper function that converts a C++ type name string into a posix file
 * name compatible string by replacing or removing special characters.
 * Should work with simple types up to single parameter template classes.
 * (Note: copied from efscape/impl/efscapelib.?pp)
 *
 * @param aC_cplusTypeName c++ type name
 * @return posix file friendly string
 */
std::string cplusTypeName2Posix(std::string aC_cplusTypeName) {
  // 1. first replace ':'s from namespace separates with '_'s
  std::string lC_posixString =
      boost::replace_all_copy(aC_cplusTypeName, ":", "_");
  // 2. Next replace leading '<' for template class names with '.'
  lC_posixString = boost::replace_all_copy(lC_posixString, "<", ".");
  // 3. Remove trailing '>' from template class names
  lC_posixString = boost::replace_all_copy(lC_posixString, ">", "");
  return lC_posixString;
}

int run(const std::shared_ptr<Ice::Communicator>&);

int main(int argc, char* argv[]) {
  int status = EXIT_SUCCESS;

  //
  // Get EFSCAPE client configuration
  //
  log4cxx::BasicConfigurator::configure();
  gCp_logger->setLevel(log4cxx::Level::getDebug());

  // attempt to retrieve the root directory of efscape ICE configuration
  fs::path lC_EfscapeIcePath(".");  // default location
  char* lcp_env_variable =          // get EFSCAPE_HOME
      getenv("EFSCAPE_PATH");

  if (lcp_env_variable != 0) lC_EfscapeIcePath = fs::path(lcp_env_variable);

  fs::path lC_configPath = fs::path(lC_EfscapeIcePath) / fs::path("src") /
                           fs::path("server") / fs::path("config.client2");

  if (fs::exists(lC_configPath)) {
    LOG4CXX_DEBUG(gCp_logger,
                  "config path <" << lC_configPath.string() << "> exists");
  } else {
    LOG4CXX_ERROR(gCp_logger, "config path <" << lC_configPath.string()
                                              << "> does not exist");
    return EXIT_FAILURE;
  }

  fs::path lC_logger_config = fs::path(lC_EfscapeIcePath) / fs::path("src") /
                              fs::path("server") / fs::path("log4j.properties");

  log4cxx::PropertyConfigurator::configure(lC_logger_config.string().c_str());

  LOG4CXX_DEBUG(gCp_logger,
                "efscape ICE server config=<" << lC_configPath.string() << ">");

  try {
    //
    // CommunicatorHolder's ctor initializes an Ice communicator,
    // and its dtor destroys this communicator.
    //
    Ice::CommunicatorHolder ich(argc, argv, lC_configPath.string().c_str());

    //
    // The communicator initialization removes all Ice-related arguments from
    // argc/argv
    //
    if (argc > 1) {
      std::cerr << argv[0] << ": too many arguments" << std::endl;
      status = 1;
    } else {
      status = run(ich.communicator());
    }
  } catch (const std::exception& ex) {
    std::cerr << argv[0] << ": " << ex.what() << std::endl;
    status = 1;
  }

  return status;
}

int run(const std::shared_ptr<Ice::Communicator>& communicator) {
  int status = EXIT_SUCCESS;
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
  // if (argc > 2) {
  //   std::cerr << argv[0] << " usage: " << argv[0] << " [parmfile]\n";
  //   return EXIT_FAILURE;
  // } else if (argc == 2) {
  //   lC_parmName = argv[1];
  // }

  std::shared_ptr<efscape::ModelHomePrx> lCp_ModelHome;
  try {
    // get handle to model home (factory)
    lCp_ModelHome = Ice::checkedCast<efscape::ModelHomePrx>(
        communicator->stringToProxy("modelhome"));
  } catch (const Ice::NotRegisteredException&) {
    auto query = Ice::checkedCast<IceGrid::QueryPrx>(
        communicator->stringToProxy("EfscapeIceGrid/Query"));
    lCp_ModelHome = Ice::checkedCast<efscape::ModelHomePrx>(
        query->findObjectByType("::efscape::ModelHome"));
  }
  if (!lCp_ModelHome) {
    std::cerr << "couldn't find a `::efscape::ModelHome' object." << std::endl;
    return 1;
  }

  LOG4CXX_DEBUG(gCp_logger, "ModelHome accessed successfully!");

  // If no input file has been specified, display a list of all models
  // currently loaded, and allow the user to select a model and output
  // a default model parameter file.
  if (lC_parmName == "") {
    efscape::ModelNameList lC1_modelList = lCp_ModelHome->getModelList();

    std::cout << "** List of available models ***\n";

    for (int i = 0; i < lC1_modelList.size(); i++) {
      int idx = i + 1;
      std::cout << idx << ": " << lC1_modelList[i] << "\n";
    }

    // menu options
    std::cout << "\nEnter the number of the model (0 to exit)==> ";

    // process user input
    int li_userInput = 0;
    std::string lC_parmString = "";
    std::string lC_modelName;
    do {
      std::cin >> li_userInput;
      if (li_userInput > 0 && li_userInput <= lC1_modelList.size()) {
        lC_modelName = lC1_modelList[li_userInput - 1];
        std::cout << "Selected model <" << lC_modelName << ">\n";

        // Write JSON string into a buffer and read the buffer into a JSON
        // object so that an attribute may be added
        lC_parmString = lCp_ModelHome->getModelInfo(lC_modelName.c_str());

        std::stringstream lC_buffer(lC_parmString);
        Json::Value lC_jsonParameters;
        lC_buffer >> lC_jsonParameters;
        if (lC_jsonParameters.isMember("modelName")) {
          lC_parmName = lC_jsonParameters["modelName"].asString();
        } else {
          // generate 'modelName' from class name
          // 1. first replace ':'s from namespace separates with '_'s
          lC_parmName = cplusTypeName2Posix(lC_modelName);

          // add a "modelName" attribute
          lC_jsonParameters["modelName"] = lC_parmName;

          // Write the JSON string back into the cleared buffer
          lC_buffer.clear();
          lC_buffer << lC_jsonParameters;
          lC_parmString = lC_buffer.str();  // update the parm string
        }
        std::cout << "lC_parmName = <" << lC_parmName << ">\n";

      } else if (li_userInput > lC1_modelList.size()) {
        std::cout << "Model index <" << li_userInput << "> out of bounds\n";
        lC_parmName = "";
        lC_parmString = "";
      }
    } while (li_userInput > 0);

    if (lC_parmName != "") {
      // save the model parameter JSON string to a file
      lC_parmName += ".json";
      std::ofstream parmFile(lC_parmName.c_str());
      parmFile << lC_parmString << std::endl;
    }
    lCp_ModelHome->shutdown();
    return EXIT_SUCCESS;
  }

  // try to load the parameter file
  std::ifstream parmFile(lC_parmName.c_str());

  // if file can be opened
  std::shared_ptr<efscape::ModelPrx> lCp_Model;
  if (parmFile) {
    fs::path p = fs::path(lC_parmName.c_str());

    LOG4CXX_DEBUG(gCp_logger, "Using input parameter file <"
                                  << lC_parmName << "> with file extension <"
                                  << p.extension() << ">");

    // convert file to a string
    if (p.extension().string() == ".json") {
      std::ostringstream buf;
      char ch;
      while (buf && parmFile.get(ch)) buf.put(ch);
      LOG4CXX_DEBUG(gCp_logger, "JSON buffer=>");
      LOG4CXX_DEBUG(gCp_logger, buf.str());

      // first try to create model from JSON parameters
      lCp_Model = lCp_ModelHome->createFromParameters(buf.str());
      if (lCp_Model == nullptr) {
        lCp_Model = lCp_ModelHome->createFromJSON(buf.str());
      }
    }
  } else {
    throw "Unable to open parameter file\n";
  }

  if (lCp_Model == nullptr) {
    throw "Invalid Model proxy";
  }

  LOG4CXX_DEBUG(gCp_logger, "Model created!");

  // get a simulator for the model
  auto lCp_Simulator = lCp_ModelHome->createSim(lCp_Model);

  if (!lCp_Simulator) throw "Invalid Simulator proxy";

  LOG4CXX_DEBUG(gCp_logger, "Simulator created!");

  double ld_time = 0.0;

  if (lCp_Simulator->start()) {
    LOG4CXX_DEBUG(gCp_logger, "Simulation started!");

    // get initial output from the model
    efscape::Message lC_message = lCp_Model->outputFunction();

    // get a handle to the simulation model clock
    for (int i = 0; i < lC_message.size(); i++) {
      LOG4CXX_DEBUG(gCp_logger,
                    "message " << i << ": value on port <" << lC_message[i].port
                               << "> = " << lC_message[i].valueToJson);
    }

    LOG4CXX_DEBUG(gCp_logger,
                  "Next event time = " << lCp_Simulator->nextEventTime());

    while (!lCp_Simulator->halt()) {
      ld_time = lCp_Simulator->nextEventTime();
      LOG4CXX_DEBUG(gCp_logger, "simulation time = " << ld_time);
      lCp_Simulator->execNextEvent();
      efscape::Message lC_message = lCp_Model->outputFunction();
      if (lC_message.size() > 0) {
        LOG4CXX_DEBUG(gCp_logger, "time step = " << ld_time
                                                 << ", message size = "
                                                 << lC_message.size());
        for (int i = 0; i < lC_message.size(); i++) {
          LOG4CXX_DEBUG(gCp_logger, "message " << i << ": value on port <"
                                               << lC_message[i].port << "> = "
                                               << lC_message[i].valueToJson);
        }
      }
    }  // while ( (ld_time = ...

    LOG4CXX_DEBUG(gCp_logger, "Simulation completed!");
  } else {
    std::cerr << "Unable to start simulation\n";
    status = 1;
  }

  lCp_Simulator->destroy();
  lCp_Model->destroy();
  lCp_ModelHome->shutdown();

  return status;
}
