// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_MODELHOMETIE_HPP
#define EFSCAPE_SERVER_MODELHOMETIE_HPP

// parent class definition
#include <Ice/Ice.h>
#include <efscape/efscape.h>

namespace efscape {

  namespace server {

    /**
     * Implements the ICE efscape::ModelHome interface, a factory for simulation
     * models. It replaces the ModelRepository interface.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 3.0.0 created 24 Dec 2006, revised 23 Apr 2017
     */
    class ModelHomeTie : virtual public ::efscape::ModelHome
    {
    public:

      //
      // ICE interface ::efscape::ModelHome
      //
      virtual
      ::efscape::ModelPrx create(const ::std::string&,
				 const Ice::Current&);

      virtual
      ::efscape::ModelPrx createFromXML(const ::std::wstring&,
					const Ice::Current&);

      virtual
      ::efscape::ModelPrx createFromJSON(const ::std::string&,
					 const Ice::Current&);

      virtual ::efscape::ModelNameList getModelList(const Ice::Current&);

      virtual ::std::string getModelInfo(const ::std::string&,
					 const Ice::Current&);

      virtual ::efscape::SimulatorPrx createSim(const ::efscape::ModelPrx&,
						const Ice::Current&);
      //
      // local (server-side) methods
      //

      ModelHomeTie();
      virtual ~ModelHomeTie();

    };				// class ModelHomeTie

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_MODELHOMETIE_HPP
