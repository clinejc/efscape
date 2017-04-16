// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include "SimpleGenerator.hh"

#include <efscape/impl/AdevsModel.hh>
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
   * Initializes the model.
   */
  void SimpleGenerator::initialize()
    throw(std::logic_error)
  {
    std::string lC_message = "SimpleGenerator::initialize()";
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "Entering " << lC_message);

    efscape::impl::AdevsModel* lCp_RootModel =
      dynamic_cast< efscape::impl::AdevsModel* >( efscape::impl::getRootModel(this) );
    if (lCp_RootModel)
      mCp_clock = lCp_RootModel->getClockIPtr();
    else
      mCp_clock.reset(new efscape::impl::ClockI);

    if (!mCp_state)
      mCp_state.reset( new SimpleState() );

    mCp_state->setClock(mCp_clock.get());
  }

  /**
   * Internal transition function.
   */
  void SimpleGenerator::delta_int() {
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
		    "SimpleGenerator::delta_int(): created state");
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

    if (!mb_IsActive) {
      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		    "SimpleGenerator::ta(): starting");
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
