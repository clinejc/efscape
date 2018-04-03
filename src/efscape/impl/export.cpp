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
#include <efscape/impl/ModelType.hpp>

//--------------------------------------------
// efscape::utils::CommandOpt-derived classes
//--------------------------------------------
#include <efscape/impl/BuildSim.hpp>
#include <efscape/impl/RunSim.hpp>

#include <json/json.h>

#include <fstream>

namespace efscape {
  namespace impl {

    //-----------------
    // register classes
    //-----------------
    // DIGRAPH class metadata
    class DigraphType : public ModelType
    {
    public:
      DigraphType() :
	ModelType("efscape::impl::DIGRAPH",
		  "The components of a digraph model must use PortValue objects as their basic I/O type: the port and value types are template arguments.",
		  gcp_libname,
		  1) {}
    };

    const bool
    lb_Digraph_registered =
       Singleton<ModelHomeI>::Instance().getModelFactory().
       registerType<DIGRAPH>(DigraphType().typeName(),
			     DigraphType().toJSON());

    // SimRunner class metadata
    class SimRunnerType : public ModelType
    {
    public:
      SimRunnerType() :
	ModelType(utils::type<SimRunner>(),
		  "Implements an adevs-based model wrapper that encapsulates a simulation model session.",
		  gcp_libname,
		  6) {}
    };

    const bool
    lb_SimRunner_registered =
      Singleton<ModelHomeI>::Instance().getModelFactory().
      registerType<SimRunner>(SimRunnerType().typeName(),
    			      SimRunnerType().toJSON() );

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
