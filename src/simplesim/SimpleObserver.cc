// __COPYRIGHT_START__
// __COPYRIGHT_END_

// class definition and export
#include "SimpleObserver.hh"

#include "SimpleGenerator.hh"
#include "BasicModel.hh"

#include <efscape/impl/AdevsModel.hpp>
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/utils/boost_utils.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace simplesim {

  // initialize class name
  const char SimpleObserver_strings::name[] = "simplesim::SimpleObserver";
  const char SimpleObserver_strings::m_input[] = "input";
  const char SimpleObserver_strings::f_output[] = "output";

  /** constructor */
  SimpleObserver::SimpleObserver() :
    efscape::impl::EntityI(),
    adevs::Atomic<efscape::impl::IO_Type>(),
    mb_is_active(false)
  {
    name(SimpleObserver_strings::name);

    mCp_ptree.reset( new boost::property_tree::ptree );
  }

  /** destructor */
  SimpleObserver::~SimpleObserver() {
  }

  /**
   *Internal transition function.
   */
  void SimpleObserver::delta_int() {
  }

  /**
   * External transition function.
   *
   * @param e time
   * @param xb bag of events
   */
  void SimpleObserver::delta_ext(double e,
				 const adevs::Bag<efscape::impl::IO_Type>& xb)
  {
    // Attempt to "consume" input (should be <const SimpleState*>)
    adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();
    for (; i != xb.end(); i++) {
      if ( (*i).port == SimpleObserver_strings::m_input) { // event on input port

	try {

	  // try to extract state
	  const SimpleState* lCp_state =
	    boost::any_cast<const SimpleState*>( (*i).value );

	  if (lCp_state != 0) {
	    // reset the ptree
	    mCp_ptree.reset( new boost::property_tree::ptree );
	    
	    // add the observation to the ptree
	    boost::property_tree::ptree child;
	    child.put("date_time",
		   boost::lexical_cast<std::string>(lCp_state->clock().date_time(lCp_state->clock().time())) );
	    child.put("count",
		   boost::lexical_cast<std::string>(lCp_state->count()) );

	    mCp_ptree->push_back(std::make_pair("row",child));

	    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			  "property tree = "
			  << efscape::utils::ptree_to_json(*mCp_ptree) );

	    //---------------------------------------------------
	    // if this is the last iteration, write final results
	    //---------------------------------------------------
	    if (lCp_state->clock().time() == lCp_state->clock().timeMax()) {
	      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			    "SimpleObserver: all observations complete");
	    }
	    mb_is_active = true;
	  }
	}
	catch(const boost::bad_any_cast &) {
	  std::cerr << "Unable to cast input as <const SimpleState*>\n";
	}
      }
    }
  }

  /**
   * Confluent transition function.
   *
   * @param xb bag of input events
   */
  void SimpleObserver::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& xb)
  {
    delta_ext(0.0,xb);
    delta_int();
  }

  /**
   * Output function.
   *
   * @param yb bag of output events
   */
  void SimpleObserver::output_func(adevs::Bag<efscape::impl::IO_Type>& yb) {
    // generting ptree output
    boost::property_tree::ptree pt;
    pt.put("testkey", "testvalue");
    pt.add_child("MyArray", *mCp_ptree);

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "SimpleObserver::output_func()")
    efscape::impl::IO_Type y(SimpleObserver_strings::f_output,
			     pt);
    yb.insert(y);
  }

  /**
   * Time advance function.
   *
   * @return time advance
   */
  double SimpleObserver::ta() {
    if (mb_is_active) {
      mb_is_active = false;
      return 0.;
    }
    return DBL_MAX;
  }

  /**
   * Output value garbage collection.
   *
   * @param yb bag of events
   */
  void SimpleObserver::gc_output(adevs::Bag<efscape::impl::IO_Type>& yb) {
  }

} // namespace simplesim

// namespace efscape {
//   namespace impl {
//     // register class AdevsModel
//     const bool lb_registered = ModelHomeI::GetModelFactory().
//       RegisterModel<simplesim::SimpleObserver>("This is a simple observer",
// 					       simplesim::gcp_libname);
//   }
// }
