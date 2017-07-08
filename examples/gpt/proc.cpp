#include "proc.hpp"

namespace gpt {

  /// Create unique 'names' for the model ports.
  const efscape::impl::PortType proc::in("in");
  const efscape::impl::PortType proc::out("out");

  proc::proc():
    efscape::impl::DEVS(),
    processing_time(1.0),
    sigma(DBL_MAX),
    val(nullptr)
  {
    t = 0.0;
  }

  proc::proc(double proc_time):
    efscape::impl::DEVS(),
    processing_time(proc_time),
    sigma(DBL_MAX),
    val(nullptr)
  {
    t = 0.0;
  }

  void proc::delta_int()
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

  void proc::delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    t += e;
    // If we are waiting for a job
    if (sigma == DBL_MAX) {
      // Make a copy of the job (original will be destroyed by the
      // generator at the end of this simulation cycle).
      try {
	val.reset(new job(boost::any_cast<job>( (*x.begin() ).value ) ) );
	// Wait for the required processing time before outputting the
	// completed job
	sigma = processing_time;
      }
      catch(const boost::bad_any_cast &) {
	std::cerr << "Unable to cast input as <job>\n";
      }
    }
    // Otherwise, model just continues with time of next event unchanged
    else {
      sigma -= e;
    }
  }
  
  void proc::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    // Discard the old job
    delta_int();
    // Process the incoming job
    delta_ext(0.0,x);
  }

  void proc::output_func(adevs::Bag<efscape::impl::IO_Type>& y)
  {
    // Produce a copy of the completed job on the out port
    efscape::impl::IO_Type pv(out,*val);
    y.insert(pv);
  }

  double proc::ta() { return sigma; }

  void proc::gc_output(adevs::Bag<efscape::impl::IO_Type>& g){}

  proc::~proc()
  {
    // if (val != NULL) 
    //   {
    // 	delete val;
    //   }
  }
 
} // namespace gpt
