#ifndef __transd_hpp_
#define __transd_hpp_
#include <vector>
#include <iostream>
#include <cstdlib>

#include <efscape/impl/adevs_config.hpp>
#include "job.hpp"

#include <adevs/adevs_cereal.hpp>

namespace gpt {
  /*
    The transducer computes various statistics about the
    performance of the queuing system.  It receives new jobs
    on its ariv port, finished jobs on its solved port,
    and generates and output on its out port when the observation
    interval has elapsed.
  */
  class transd: public adevs::Atomic<efscape::impl::IO_Type>
  {
  public:
    /// default Constructor
    transd();
    
    /// Constructor
    transd(double observ_time);

    /// Internal transition function
    void delta_int();
    
    /// External transition function
    void delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Confluent transition function
    void delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Output function
    void output_func(adevs::Bag<efscape::impl::IO_Type>& y);
    
    /// Time advance function
    double ta();
    
    /// Garbage collection. No heap allocation in output, so do nothing
    void gc_output(adevs::Bag<efscape::impl::IO_Type>& g);
    
    /// Destructor
    ~transd()
    {
      jobs_arrived.clear();
      jobs_solved.clear();
    }
    /// Model input port
    static const efscape::impl::PortType ariv;
    static const efscape::impl::PortType solved;
    
    /// Model output port
    static const efscape::impl::PortType out;

  private:
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive & ar)
    {
      ar( cereal::make_nvp("adevs::Atomic",
			   cereal::base_class<efscape::impl::DEVS >(this) ),
	  CEREAL_NVP(jobs_arrived),
	  CEREAL_NVP(jobs_solved),
	  CEREAL_NVP(observation_time),
	  CEREAL_NVP(sigma),
	  CEREAL_NVP(total_ta),
	  CEREAL_NVP(t) );
    }
  
    /// Model state variables
    std::vector<job> jobs_arrived;
    std::vector<job> jobs_solved; 
    double observation_time, sigma, total_ta, t;
  };

}

#endif
