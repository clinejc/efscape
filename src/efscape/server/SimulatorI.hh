// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimulatorI.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_SIMULATOR_I_HH
#define EFSCAPE_SERVER_SIMULATOR_I_HH

#include <Ice/Ice.h>
#include <efscape/efscape.h>
#include <IceUtil/Handle.h>

namespace efscape {

  namespace server {

    // forward declarations
    class SimulatorI;

    // typedefs
    typedef IceUtil::Handle<SimulatorI> SimulatorIPtr;

    /**
     * This class implements the Simulator interface
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.2 created 26 Aug 2007, revised 11 Mar 2009
     *
     * ChangeLog:
     *   - 2009.03.11 moved to namespace efscape/server
     *   - 2007.08.26 created class SimulatorI.
     */
    class SimulatorI : virtual public ::efscape::Simulator,
		       public IceUtil::Mutex
    {
    public:

      SimulatorI();
      SimulatorI(const ::efscape::ModelPrx& aCp_model);
      ~SimulatorI();

      virtual bool start(const Ice::Current&);

      virtual ::Ice::Double nextEventTime(const Ice::Current&);

      virtual void execNextEvent(const Ice::Current&);

      virtual void computeNextOutput(const Ice::Current&);

      virtual void computeNextState(::Ice::Double,
				    const ::efscape::Message&,
				    const Ice::Current&);

      virtual bool halt(const Ice::Current&);

      virtual void destroy(const Ice::Current&);

    private:

      /** handle to model */
      ::efscape::ModelPrx mCp_model;

    };				// class SimulatorI

  } // namespace server

} // namespace efscape

#endif	// #define EFSCAPE_SERVER_SIMULATOR_I_HH

