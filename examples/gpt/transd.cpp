#include "transd.hpp"

namespace gpt {

  /// Assign unique 'names' to ports
  const efscape::impl::PortType transd::ariv("ariv");
  const efscape::impl::PortType transd::solved("solved");
  const efscape::impl::PortType transd::out("out");
  
  transd::transd():
    efscape::impl::DEVS(),
    observation_time(1.0),
    sigma(observation_time),
    total_ta(0.0),
    t(0.0)
  {
  }
  
  transd::transd(double observ_time):
    efscape::impl::DEVS(),
    observation_time(observ_time),
    sigma(observation_time),
    total_ta(0.0),
    t(0.0)
  {
  }

  void transd::delta_int() 
  {
    // Keep track of the simulation time
    t += sigma;
    /// Model dumps interesting statistics to the console
    /// when the observation period ends.
    double throughput;
    double avg_ta_time;
    if (!jobs_solved.empty()) {
      avg_ta_time = total_ta / jobs_solved.size();
      if (t > 0.0) {
	throughput = jobs_solved.size() / t;
      }
      else {
	throughput = 0.0;
      }
    }
    else {
      avg_ta_time = 0.0;
      throughput = 0.0;
    }
    std::cout << "End time: " << t << std::endl;
    std::cout << "jobs arrived : " << jobs_arrived.size () << std::endl;
    std::cout << "jobs solved : " << jobs_solved.size() << std::endl;
    std::cout << "AVERAGE TA = " << avg_ta_time << std::endl;
    std::cout << "THROUGHPUT = " << throughput << std::endl;
    // Passivate once the data collection period has ended and the
    // results are reported.
    sigma = DBL_MAX; 
  }

  void transd::delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    // Keep track of the simulation time
    t += e;
    // Save new jobs in order to compute statistics when they are
    // completed.
    adevs::Bag<efscape::impl::IO_Type>::iterator iter;
    for (iter = x.begin(); iter != x.end(); iter++) {
      if ((*iter).port == ariv) {
	try {
	  job j(boost::any_cast<job>( (*iter).value ));
	  j.t = t;
	  std::cout << "Start job " << j.id << " @ t = " << t << std::endl;
	  jobs_arrived.push_back(j);
	}
	catch(const boost::bad_any_cast &) {
	  std::cerr << "Unable to cast input as <job>\n";
	}
      }
      // Compute time required to process completed jobs
      for (iter = x.begin(); iter != x.end(); iter++) {
	if ((*iter).port == solved) {
	  try {
	    job j(boost::any_cast<job>( (*iter).value ));
	    std::vector<job>::iterator i = jobs_arrived.begin();
	    for (; i != jobs_arrived.end(); i++) {
	      if ((*i).id == j.id) {
		total_ta += t-(*i).t;
		std::cout << "Finish job " << j.id << " @ t = " << t << std::endl;
		break;
	      }
	    }
	    j.t = t;
	    jobs_solved.push_back(j);
	  }
	  catch(const boost::bad_any_cast &) {
	    std::cerr << "Unable to cast input as <job>\n";
	  }
	}
      }
      // Continue with next event time unchanged
      sigma -= e;
    }
    
  }

  void transd::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    delta_int();
    delta_ext(0.0,x);
  }

  void transd::output_func(adevs::Bag<efscape::impl::IO_Type>& y)
  {
    /// Generate an output event to stop the generator
    job j;
    efscape::impl::IO_Type pv(out,j);
    y.insert(pv);
  }
 
}
