// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include "SimpleGenerator.hh"

#include <efscape/impl/AdevsModel.hpp>
#include <efscape/impl/ModelHomeI.hpp>

#include <iostream>
#include <boost/multi_array.hpp>

namespace simplesim {

  // initialize class name
  const char SimpleGenerator_strings::name[] = "simplesim::SimpleGenerator";
  const char SimpleGenerator_strings::f_out[] = "out";

  /** constructor */
  SimpleGenerator::SimpleGenerator() :
    efscape::impl::EntityI(),
    adevs::Atomic<efscape::impl::IO_Type>(),
    mb_IsActive(false),
    mi_status(0)
  {
    name(SimpleGenerator_strings::name);
  }

  /** destructor */
  SimpleGenerator::~SimpleGenerator() {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Deleting SimpleGenerator...");
  }

  /**
   * Internal transition function.
   */
  void SimpleGenerator::delta_int() {
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "delta_int()");
    if (!mb_IsActive)		// activate
      mb_IsActive = true;
  }

  /**
   * External transition function.
   *
   * @param e time
   * @param xb bag of events
   */
  void SimpleGenerator::delta_ext(double e,
				  const adevs::Bag<efscape::impl::IO_Type>& xb)
  {
    if (mCp_state.get() == NULL)
      mCp_state.reset( new SimpleState() );
      
    // Attempt to "consume" input
    adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();

    for (; i != xb.end(); i++) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "input on port <"
		    << (*i).port << ">");
      if ( (*i).port == "clock_in") { // event on <clock_in> port
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Inspecting input on port=" << "clock_in");
	try {
	  // 1) try to extract a smart pointer to the clock
	  efscape::impl::ClockI* lCp_clock =
	    new efscape::impl::ClockI( boost::any_cast<efscape::impl::ClockI>( (*i).value ));
	  mCp_clock.reset(lCp_clock);

	  // pass clock pointer to the state data member
	  if (mCp_state.get() == NULL) // ensure that the state exists
	    mCp_state.reset( new SimpleState() );
	  mCp_state->setClock(mCp_clock.get());
	}
	catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <ClockI>");
	}
      }	// if ( (*i).port == "clock_in")
    }	// for (; ....
  }

  /**
   * Confluent transition function.
   *
   * @param xb bag of input events
   */
  void SimpleGenerator::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& xb)
  {
    delta_int();
    delta_ext(0.0,xb);
  }

  /**
   * Output function.
   *
   * @param yb bag of output events
   */
  void SimpleGenerator::output_func(adevs::Bag<efscape::impl::IO_Type>& yb) {
    // create initial state
    if (!mb_IsActive) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "SimpleGenerator::delta_output(): created state");
      mCp_state->count() = 0; // initialize count
    }
    else {
      // increment count
      ++mCp_state->count();
    }

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "SimpleGenerator::output_func(): count = "
		  << mCp_state->count());

    // output the model state
    out = mCp_state.get();
    efscape::impl::IO_Type y(SimpleGenerator_strings::f_out,
			     out);
    yb.insert(y);
  }

  /**
   * Time advance function.
   *
   * @return time advance
   */
  double SimpleGenerator::ta() {
    // first determine if the generator clock has initialized
    if (mCp_clock.get() == NULL) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "model not yet uninitialized -- clock is null!:");
     return DBL_MAX;
    }
    
    if (!mb_IsActive) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "model starting");
      return 0;
    }

    if (mCp_state->clock().time() >= mCp_state->clock().timeMax()) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "SimpleGenerator::ta(): stopping");
      return DBL_MAX;
    }

    double ld_time =
      mCp_state->clock().timeDelta();

    return mCp_state->clock().timeDelta();
  }

  /**
   * Output value garbage collection.
   *
   * @param g bag of events
   */
  void SimpleGenerator::gc_output(adevs::Bag<efscape::impl::IO_Type>& g) {
  }

}
