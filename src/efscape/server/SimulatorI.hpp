// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimulatorI.hpp
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
      SimulatorI(::std::shared_ptr<ModelPrx> aCp_model);
      ~SimulatorI();

      virtual bool start(const Ice::Current&) override;

      virtual double nextEventTime(const Ice::Current&) override;

      virtual void execNextEvent(const Ice::Current&) override;

      virtual void computeNextOutput(const Ice::Current&) override;

      virtual void computeNextState(::Ice::Double,
				    ::efscape::Message,
				    const Ice::Current&) override;

      virtual bool halt(const Ice::Current&) override;

      virtual void destroy(const Ice::Current&) override;

    private:

      /** handle to model */
      ::std::shared_ptr<ModelPrx> mCp_model;

    };				// class SimulatorI

  } // namespace server

} // namespace efscape

#endif	// #define EFSCAPE_SERVER_SIMULATOR_I_HPP

