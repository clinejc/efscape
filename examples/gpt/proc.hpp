#ifndef __proc_hpp_
#define __proc_hpp_
#include <cstdlib>

#include <efscape/impl/adevs_config.hpp>
#include "job.h"

#include <adevs/adevs_cereal.hpp>

namespace gpt {
  /*
    A processor requires a fixed period of time to service a job.
    The processor can serve only one job at a time.  It the processor
    is busy, it simply discards incoming jobs.
  */
  class proc: public efscape::impl::DEVS
  {
  public:
    /// Default Constructor
    proc();
    
    /// Constructor.  The processing time is provided as an argument.
    proc(double proc_time);
    
    /// Internal transition function
    void delta_int();
    
    /// External transition function
    void delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Confluent transition function.
    void delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Output function.  
    void output_func(adevs::Bag<efscape::impl::IO_Type>& y);
    
    /// Time advance function.
    double ta() { return sigma; }
    
    /**
       Garbage collection. No heap allocation in output_func, so
       do nothing.
    */
    void gc_output(adevs::Bag<efscape::impl::IO_Type>& g){}

    /// Destructor
    ~proc()
    {
      // if (val != NULL) 
      //   {
      // 	delete val;
      //   }
    }

    /// Model input port
    static const efscape::impl::PortType in;
    /// Model output port
    static const efscape::impl::PortType out;

  private:
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive & ar)
    {
      ar( cereal::make_nvp("adevs::Atomic",
			   cereal::base_class<efscape::impl::DEVS >(this) ),
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

}
// namespace gpt

#endif
