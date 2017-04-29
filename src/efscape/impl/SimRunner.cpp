// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

// class definition
#include <efscape/impl/SimRunner.hpp>

#include <efscape/impl/ModelHomeI.hpp>

namespace efscape {
  
  namespace impl {

    // instantiate class data members
    const PortType SimRunner::clock_in =
      "clock_in";
    const PortType SimRunner::properties_in =
      "properties_in";


    SimRunner::SimRunner() : ModelWrapperBase() {
    }

    SimRunner::~SimRunner() {
    }

    void SimRunner::delta_int() {
      ModelWrapperBase::delta_int();
    }

    void SimRunner::delta_ext(double e, const adevs::Bag<IO_Type>& xb) {
      // Attempt to "consume" input
      adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();

      for (; i != xb.end(); i++) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "SimRunner input on port <"
		      << (*i).port << ">");
	if ( (*i).port == "clock_in") { // event on <clock_in> port
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Found port=" << clock_in);
	  try {
	    // 1) try to extract the properties file name
	    mCp_ClockI = boost::any_cast<ClockIPtr>( (*i).value );
	  }
	  catch(const boost::bad_any_cast &) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <ClockI>");
	  }
	}
      }
	
      ModelWrapperBase::delta_ext(e, xb);
    }

    void SimRunner::translateInput(const adevs::Bag<IO_Type>&
				   external_input,
				   adevs::Bag<adevs::Event<IO_Type> >&
				   internal_input)
    {
    }

    void SimRunner::translateOutput(const
				    adevs::Bag<adevs::Event<IO_Type> >&
				    internal_output,
				    adevs::Bag<IO_Type>& external_output)
    {
    }
    
    void SimRunner::gc_input(adevs::Bag<adevs::Event<IO_Type> >& g)
    {
    }

    void SimRunner::gc_output(adevs::Bag<efscape::impl::IO_Type>& g)
    {
    }

    //-------------------------
    // accessor/mutator methods
    //-------------------------

    ///
    /// clock
    ///
    ClockIPtr& SimRunner::getClockIPtr() { return mCp_ClockI; }

    void SimRunner::setClockIPtr(const ClockIPtr& aCp_clock) {
      mCp_ClockI = aCp_clock;
    }

    const ClockI* SimRunner::getClock() const { return mCp_ClockI.get(); }

  } // namespace impl
  
}   // namespace efscape
