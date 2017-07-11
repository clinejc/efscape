// __COPYRIGHT_START__
// Package Name : efscape
// File Name : export.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/SimRunner.hpp>

// model factory definitions
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

//--------------------------------------------
// efscape::utils::CommandOpt-derived classes
//--------------------------------------------
#include <efscape/impl/BuildModel.hpp>
#include <efscape/impl/BuildSim.hpp>
#include <efscape/impl/RunSim.hpp>

#include <json/json.h>

#include <fstream>

namespace efscape {
  namespace impl {

    //-----------------
    // register classes
    //-----------------
    /**
     * Creates DIGRAPH class metadata
     *
     * @returns metadata in a JSON property tree
     */
    Json::Value create_Digraph_info() {
      Json::Value lC_jsonObj;
      lC_jsonObj["info"] =
	"The components of a digraph model must use PortValue objects as their basic I/O type: the port and value types are template arguments.";
      lC_jsonObj["library"] =  "efscape-impl";

      return lC_jsonObj;
    }

    const bool
    lb_Digraph_registered =
      Singleton<ModelHomeI>::Instance().getModelFactory().
      registerType<DIGRAPH>("efscape::impl::DIGRAPH",
			    create_Digraph_info() );

    const bool
    lb_SimRunner_registered =
      Singleton<ModelHomeI>::Instance().getModelFactory().
      registerType<SimRunner>(efscape::utils::type<SimRunner>(),
    			      SimRunner::get_info() );
    
    // const bool
    // lb_BuildModel_registered =
    //   Singleton<ModelHomeI>::Instance().getCommandFactory().
    //   registerType<BuildModel>( BuildModel::ProgramName() );

    const bool
    lb_RunSim_registered =
      Singleton<ModelHomeI>::Instance().getCommandFactory().
      registerType<RunSim>( RunSim::ProgramName() );

    const bool lb_BuildSim_registered =
      Singleton<ModelHomeI>::Instance().getCommandFactory().
      registerType<BuildSim>( BuildSim::ProgramName() );
    
  } // namespace impl
}   // namespace efscape
