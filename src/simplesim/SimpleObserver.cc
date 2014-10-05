// __COPYRIGHT_START__
// __COPYRIGHT_END_

// class definition and export
#include "SimpleObserver.hh"

#include "SimpleGenerator.hh"
#include "BasicModel.hh"

#include "efscape/impl/AdevsModel.hh"
#include "efscape/impl/ModelHomeI.hh"

#include <fstream>
#include <iostream>

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace simplesim {

  // initialize class name
  const char SimpleObserver_strings::name[] = "simplesim::SimpleObserver";
  const char SimpleObserver_strings::m_input[] = "input";

  /** constructor */
  SimpleObserver::SimpleObserver() :
    efscape::impl::EntityI(),
    adevs::Atomic<efscape::impl::IO_Type>()
  {
    name(SimpleObserver_strings::name);
  }

  /** destructor */
  SimpleObserver::~SimpleObserver() {
  }

  /**
   * Initializes the netCDF dataset for model cell output.
   *
   * @throws std::logic_error
   */
  void SimpleObserver::initialize()
    throw(std::logic_error)
  {
    std::string lC_message = "SimpleObserver::initialize()";

    // initialize handle to cell inputs
    efscape::impl::AdevsModel* lCp_RootModel =
      dynamic_cast<efscape::impl::AdevsModel*>( efscape::impl::getRootModel(this) );

    if (lCp_RootModel == 0) {
      lC_message +=
	": Unable to access root model";
      throw(std::logic_error(lC_message));
    }

    // get handle to clock
    const efscape::impl::ClockI* lCp_clock = lCp_RootModel->getClock();

    // // create observer dataset
    // std::string lC_FileName = "./output.nc";
    // efscape::impl::TimeSeries* lCp_dataset = new efscape::impl::TimeSeries;
    // lCp_dataset->open(lC_FileName.c_str());
    // mCp_dataset.reset( lCp_dataset );

    // if (!mCp_dataset->dataset()->is_valid()) {
    //   lC_message +=
    // 	": Unable to open cell observer dataset <" + lC_FileName + ">";
    //   throw(std::logic_error(lC_message));
    // }

    // LOG4CXX_DEBUG(getLogger(),
    // 		  "\tCreated dataset <"
    // 		  << lC_FileName);

    //--------------------
    // set time attributes
    //--------------------
    ptime lC_base_date =
      lCp_clock->date_time(lCp_clock->time());
    // ((efscape::impl::TimeSeries*)mCp_dataset.get())
    //   ->time_units(1, "days", lC_base_date);

    // // add variable <count>
    // NcVar* lCp_var = mCp_dataset->add_var("count", ncInt);
    

  } // SimpleObserver::initialize()

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

// 	    // write time series data
// 	    long ll_num_recs = 0;
// 	    if (lCp_state->clock().time() > 0)
// 	      ll_num_recs = mCp_dataset->num_recs();

// 	    // time
// 	    NcVar* lCp_var = mCp_dataset->dataset()->get_var("time");
// 	    double ld_time = lCp_state->clock().time();
// 	    lCp_var->put_rec(&ld_time,ll_num_recs);

// 	    // count
// 	    lCp_var = mCp_dataset->dataset()->get_var("count");
// // 	    int li_count = lCp_state->count().values().as_int(0);
// 	    int li_count = lCp_state->count();
// 	    lCp_var->put_rec(&li_count,ll_num_recs);

// 	    // update dataset
// 	    mCp_dataset->dataset()->sync();

// 	    LOG4CXX_DEBUG(getLogger(),
// 			  lCp_state->clock().date_time(lCp_state->clock().time())
// 			  << "\t"
// // 			  << lCp_state->count().values().as_int(0));
// 			  << lCp_state->count());


	    //---------------------------------------------------
	    // if this is the last iteration, write final results
	    //---------------------------------------------------
	    if (lCp_state->clock().time() == lCp_state->clock().timeMax()) {
	      LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			    "SimpleObserver: all observations complete");
	    }
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
  }

  /**
   * Time advance function.
   *
   * @return time advance
   */
  double SimpleObserver::ta() {
    return DBL_MAX;
  }

  /**
   * Output value garbage collection.
   *
   * @param g bag of events
   */
  void SimpleObserver::gc_output(adevs::Bag<efscape::impl::IO_Type>& g) {
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
