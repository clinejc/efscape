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
     * @version 0.1.0 created 24 Dec 2006, revised 08 Dec 2015
     *
     * ChangeLog:
     *   - 2015-12-08 Removed newly added method
     *     * string getModelProperties(...)
     *   - 2015-11-22 One revised and one new method
     *     * string getModelInfo(...) -- revised
     *     * string getModelProperties(...) -- new
     *   - 2015-05-31 removed createWithConfig(...) method
     *   - 2015-05-27 implemented createFromJSON(...) method
     *   - 2014-12-14 removed method for accessing JsonDataset
     *   - 2014-11-02 added implemention of new method for accessing JsonDataset
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

#endif	// #ifndef EFSCAPE_SERVER_MODELHOMETIE_HH
