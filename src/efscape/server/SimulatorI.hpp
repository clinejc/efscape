// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimulatorI.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_SIMULATOR_I_HPP
#define EFSCAPE_SERVER_SIMULATOR_I_HPP

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
     * @version 1.0.0 created 26 Aug 2007, revised 11 Mar 2009
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

#endif	// #define EFSCAPE_SERVER_SIMULATOR_I_HPP

