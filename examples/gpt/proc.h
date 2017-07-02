#ifndef __proc_h_
#define __proc_h_
#include <cstdlib>
#include "adevs.h"
#include "job.h"

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/access.hpp>

/*
  A processor requires a fixed period of time to service a job.
  The processor can serve only one job at a time.  It the processor
  is busy, it simply discards incoming jobs.
*/
class proc: public adevs::Atomic<PortValue>
{
public:
  /// Constructor.  The processing time is provided as an argument.
  proc():
    adevs::Atomic<PortValue>(),
    processing_time(1.0),
    sigma(DBL_MAX),
    val(nullptr)
  {
    t = 0.0;
  }
  proc(double proc_time):
    adevs::Atomic<PortValue>(),
    processing_time(proc_time),
    sigma(DBL_MAX),
    val(nullptr)
  {
    t = 0.0;
  }
  /// Internal transition function
  void delta_int()
  {
    t += sigma;
    // Done with the job, so set time of next event to infinity
    sigma = DBL_MAX;
    // Discard the completed job
    // if (val.get() != NULL) 
    //   {
    // 	delete val;
    //   }
    val = nullptr;
  }
  /// External transition function
  void delta_ext(double e, const adevs::Bag<PortValue>& x)
  {
    t += e;
    // If we are waiting for a job
    if (sigma == DBL_MAX) 
      {
	// Make a copy of the job (original will be destroyed by the
	// generator at the end of this simulation cycle).
	val.reset(new job((*(x.begin())).value) );
	// Wait for the required processing time before outputting the
	// completed job
	sigma = processing_time;
      }
    // Otherwise, model just continues with time of next event unchanged
    else
      {
	sigma -= e;
      }
  }
  /// Confluent transition function.
  void delta_conf(const adevs::Bag<PortValue>& x)
  {
    // Discard the old job
    delta_int();
    // Process the incoming job
    delta_ext(0.0,x);
  }
  /// Output function.  
  void output_func(adevs::Bag<PortValue>& y)
  {
    // Produce a copy of the completed job on the out port
    PortValue pv(out,*val);
    y.insert(pv);
  }
  /// Time advance function.
  double ta() { return sigma; }
  /**
     Garbage collection. No heap allocation in output_func, so
     do nothing.
  */
  void gc_output(adevs::Bag<PortValue>& g){}
  /// Destructor
  ~proc()
  {
    // if (val != NULL) 
    //   {
    // 	delete val;
    //   }
  }

  /// Model input port
  static const int in;
  /// Model output port
  static const int out;

private:
  friend class cereal::access;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar( cereal::make_nvp("adevs::Atomic",
			 cereal::base_class<adevs::Atomic<PortValue> >(this) ),
	CEREAL_NVP(processing_time),
	CEREAL_NVP(sigma),
	CEREAL_NVP(val),
	CEREAL_NVP(t) );
  }
  
  /// Model state variables
  double processing_time, sigma;
  std::shared_ptr<job> val;
  double t;
};

/// Create unique 'names' for the model ports.
const int proc::in(0);
const int proc::out(1);

#endif
