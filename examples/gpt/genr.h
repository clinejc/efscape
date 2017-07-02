#ifndef _genr_h_
#define _genr_h_
#include "adevs.h"
#include "job.h"

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/access.hpp>

/*
The genr class produces jobs periodically.  The genr starts
producing jobs when it receives an input on its start port.
It stops producing jobs when it receives an input on its
stop port.  Jobs appear on the out port.
*/
class genr: public adevs::Atomic<PortValue>
{
public:
  /// Constructor.  The generator period is provided here.
  genr():
    adevs::Atomic<PortValue>(),
    period(1.0),
    sigma(1.0),
    count(0)
  {
  }
  genr(double period):
    adevs::Atomic<PortValue>(),
    period(period),
    sigma(period),
    count(0)
  {
  }
  /// Internal transition function
  void delta_int()
  {
    /*
      We just produced a job via the output_func, so increment the
      job counter.
    */
    count++;
    // Wait until its time to produce the next job
    sigma = period;
  }
  /// External transition function
  void delta_ext(double e, const adevs::Bag<PortValue>& x)
  {
    // Continue with next event time unchanged if, for some reason,
    // the input is on neither on these ports.
    sigma -= e;
    // Look for input on the start port.  If input is found,
    // hold until it is time to produce the first output.
    adevs::Bag<PortValue>::const_iterator iter;
    for (iter = x.begin(); iter != x.end(); iter++)
      {
	if ((*iter).port == start)
	  {
	    sigma = period;
	  }
      }
    // Look for input on the stop port.  If input is found,
    // stop the generator by setting our time of next event to infinity.
    for (iter = x.begin(); iter != x.end(); iter++)
      {
	if ((*iter).port == stop)
	  {
	    sigma = DBL_MAX;
	  }
      }
  }
  /// Confluent transition function
  void delta_conf(const adevs::Bag<PortValue>& x)
  {
    // When an internal and external event coincide, compute
    // the internal state transition then process the input.
    delta_int();
    delta_ext(0.0,x);
  }
  /// Output function.  
  void output_func(adevs::Bag<PortValue>& y)
  {
    // Place a new job on the output port
    job j(count);
    PortValue pv(out,j);
    y.insert(pv);
  }
  /// Time advance function.
  double ta() { return sigma; }
  /// Output doesn't require heap allocation, so don't do anything
  void gc_output(adevs::Bag<PortValue>& g){}
  /// Model input ports
  static const int start;
  static const int stop;
  /// Model output port
  static const int out;

private:
  friend class cereal::access;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar( cereal::make_nvp("adevs::Atomic",
			 cereal::base_class<adevs::Atomic<PortValue> >(this) ),
	CEREAL_NVP(period),
	CEREAL_NVP(sigma),
	CEREAL_NVP(count) );
  }
  
  /// Model state variables
  double period, sigma;
  int count;
};

/// Create the static ports and assign them unique 'names' (numbers)
const int genr::stop(0);
const int genr::start(1);
const int genr::out(2);
  
#endif
