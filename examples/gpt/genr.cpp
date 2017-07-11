#include "genr.hpp"

#include <efscape/impl/ModelHomeI.hpp>

namespace gpt {

  /// Create the static ports and assign them unique 'names'
  const efscape::impl::PortType genr::stop("stop");
  const efscape::impl::PortType genr::start("start");
  const efscape::impl::PortType genr::out("out");
  
  genr::genr():
    adevs::Atomic<efscape::impl::IO_Type>(),
    period(1.0),
    sigma(1.0),
    count(0)
  {
  }

  genr::genr(double period):
    adevs::Atomic<efscape::impl::IO_Type>(),
    period(period),
    sigma(period),
    count(0)
  {
  }

  void genr::delta_int()
  {
    /*
      We just produced a job via the output_func, so increment the
      job counter.
    */
    count++;
    // Wait until its time to produce the next job
    sigma = period;
  }

  void genr::delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    // Continue with next event time unchanged if, for some reason,
    // the input is on neither on these ports.
    sigma -= e;
    // Look for input on the start port.  If input is found,
    // hold until it is time to produce the first output.
    adevs::Bag<efscape::impl::IO_Type>::const_iterator iter;
    for (iter = x.begin(); iter != x.end(); iter++) {
      if ((*iter).port == start) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Starting genr...");
	sigma = period;
      }
    }
    // Look for input on the stop port.  If input is found,
    // stop the generator by setting our time of next event to infinity.
    for (iter = x.begin(); iter != x.end(); iter++) {
      if ((*iter).port == stop) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Stopping genr...");
	sigma = DBL_MAX;
      }
    }
  }
  
  void genr::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    // When an internal and external event coincide, compute
    // the internal state transition then process the input.
    delta_int();
    delta_ext(0.0,x);
  }

  void genr::output_func(adevs::Bag<efscape::impl::IO_Type>& y)
  {
    // Place a new job on the output port
    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		  "genr placing job on the output port...");
    job j(count);
    efscape::impl::IO_Type pv(out,j);
    y.insert(pv);
  }

  double genr::ta() { return sigma; }
  
} // namespace gpt
