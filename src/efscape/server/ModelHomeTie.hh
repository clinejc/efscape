// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_MODELHOMETIE_HH
#define EFSCAPE_SERVER_MODELHOMETIE_HH

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
     * @version 0.0.9 created 24 Dec 2006, revised 24 Sep 2014
     *
     * ChangeLog:
     *   - 2014-09-24 removed efscape::impl::ModelHomeI member
     *   - 2014-09-24 implemented createWithConfig(...) method
     *   - 2009-03-11 moved to directory and namespace efscape/server
     *   - 2009-02-28 replaced ModelI* return with adevs
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
      ::efscape::ModelPrx createWithConfig(const ::std::string &,
					   const ::std::wstring&,
					   const Ice::Current&);

      virtual ::efscape::ModelList getModelList(const Ice::Current&);

      virtual ::efscape::SimulatorPrx createSim(const ::efscape::ModelPrx&,
						const Ice::Current&);

      virtual
      ::efscape::JsonDatasetPrx createJsonDataset(const ::std::string&,
						  const ::std::string&,
						  const Ice::Current&);
      //
      // local (server-side) methods
      //

      ModelHomeTie();
      virtual ~ModelHomeTie();

    };				// class ModelHomeTie

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_MODELHOMETIE_HH
