// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockTie.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_CLOCKTIE_HH
#define EFSCAPE_SERVER_CLOCKTIE_HH

// parent class definitions
#include <efscape/efscape.h>
#include <efscape/impl/EntityI.hh>

#include <IceUtil/Handle.h>
#include <efscape/impl/ClockI.hh>
#include <boost/shared_ptr.hpp>

namespace efscape {

  namespace server {

    // forward declarations
    class ClockTie;

    // typedefs
    typedef IceUtil::Handle<ClockTie> ClockTiePtr;

    /**
     * Defines the Clock class, which holds time-related information for
     * a Efscape simulation. The attributes are loosely based on the CDC netCDF
     * Conventions for Gridded Data.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 create 18 Oct 2008, revised 06 Mar 2009
     *
     * ChangeLog:
     *   - 2009.03.06 Added implementation of ICE efscape/Entity interface
     *   - 2008.10.18 Created class ClockTie
     */
    class ClockTie : virtual public ::efscape::Clock,
		     virtual public efscape::impl::EntityI
    {
    public:

      ClockTie();
      ClockTie(const boost::shared_ptr<efscape::impl::ClockI>& aCp_clock);
      virtual ~ClockTie();

      //
      // ICE interface efscape::Entity
      //
      virtual ::std::string getName(const Ice::Current&) const;

      //
      // ICE interface efscape::Clock
      //
      virtual ::std::string timeUnits(const Ice::Current&) const;

      virtual ::Ice::Double timeDelta(const Ice::Current&) const;

      virtual ::Ice::Double timeMax(const Ice::Current&) const;

      virtual ::Ice::Double timeCurrent(const Ice::Current&) const;

      //
      // local (server-side) interfaces
      //

      /** @returns a handle to the wrapped clock */
      efscape::impl::ClockI* getClock() {
	return mCp_clock.get();
      }

      /**
       * Sets the wrapped clock.
       *
       * @param aCp_clock clock handle
       */
      void setClock(const boost::shared_ptr<efscape::impl::ClockI>& aCp_clock) {
	mCp_clock = aCp_clock;
      }

    private:

      /** shared handle to wrapped clock */
      boost::shared_ptr<efscape::impl::ClockI> mCp_clock;

    };				// class ClockTie definition

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_CLOCKTIE_HH

