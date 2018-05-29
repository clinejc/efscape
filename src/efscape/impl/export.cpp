// __COPYRIGHT_START__
// Package Name : efscape
// File Name : export.cpp
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

#include <efscape/impl/SimRunner.hpp>

// model factory definitions
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/impl/ModelType.hpp>

//--------------------------------------------
// efscape::utils::CommandOpt-derived classes
//--------------------------------------------
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

    const bool
    lb_RunSim_registered =
      Singleton<ModelHomeI>::Instance().getCommandFactory().
      registerType<RunSim>( RunSim::ProgramName() );

  } // namespace impl
}   // namespace efscape
