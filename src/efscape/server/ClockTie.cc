// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockTie.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/server/ClockTie.hh>

#include <efscape/impl/ModelHomeI.hpp> // for server-side logging

namespace efscape {

  namespace server {

    /**
     * Returns the name of the entity.
     *
     * @returns the name of the entity
     */
    ::std::string
    ClockTie::getName(const Ice::Current& current) const
    {
      return name();
    }

    /**
     * Returns the time units.
     *
     * @param current method invocation
     * @returns time units
     */
    ::std::string
    ClockTie::timeUnits(const Ice::Current& current) const
    {
      if (mCp_clock.get())
	return mCp_clock->timeUnits();

      return "";
    }

    /**
     * Returns the minimum time delta.
     *
     * @param current method invocation
     * @returns time delta
     */
    ::Ice::Double
    ClockTie::timeDelta(const Ice::Current& current) const
    {
      if (mCp_clock.get())
	return mCp_clock->timeDelta();

      return 0.0;
    }

    /**
     * Returns the maximum time.
     *
     * @param current method invocation
     * @returns time max
     */
    ::Ice::Double
    ClockTie::timeMax(const Ice::Current& current) const
    {
      if (mCp_clock.get())
	return mCp_clock->timeMax();

      return 0.0;
    }

    /**
     * Returns the current time.
     *
     * @param current method invocation
     * @returns current time
     */
    ::Ice::Double
    ClockTie::timeCurrent(const Ice::Current& current) const
    {
      if (mCp_clock.get())
	return mCp_clock->time();

      return 0.0;
    }

    //--------------------------------------------------------------------------
    // implementation of local (server-side) methods
    //--------------------------------------------------------------------------

    /** constructor */
    ClockTie::ClockTie()
    {
      name("Clock");
    }

    /**
     * constructor
     *
     * @param aCp_clock handle to wrapped clock
     */
    ClockTie::ClockTie(const
		       boost::shared_ptr<efscape::impl::ClockI>& aCp_clock) :
      mCp_clock(aCp_clock) {}

    /** destructor */
    ClockTie::~ClockTie() {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "Deleting ClockTie...");
    }

  } // namespace server

} // namespace efscape
