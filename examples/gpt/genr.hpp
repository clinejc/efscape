#ifndef _genr_hpp_
#define _genr_hpp_

#include <efscape/impl/adevs_config.hpp>
#include "job.hpp"

#include <adevs/adevs_cereal.hpp>

namespace gpt {
  /*
    The genr class produces jobs periodically.  The genr starts
    producing jobs when it receives an input on its start port.
    It stops producing jobs when it receives an input on its
    stop port.  Jobs appear on the out port.
  */
  class genr: public efscape::impl::DEVS
  {
  public:
    /// Constructor.  The generator period is provided here.
    genr();
    genr(double period);
    
    /// Internal transition function
    void delta_int();
    
    /// External transition function
    void delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Confluent transition function
    void delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x);
    
    /// Output function.  
    void output_func(adevs::Bag<efscape::impl::IO_Type>& y);
    
    /// Time advance function.
    double ta() { return sigma; }
    
    /// Output doesn't require heap allocation, so don't do anything
    void gc_output(adevs::Bag<efscape::impl::IO_Type>& g){}

    /// Model input ports
    static const efscape::impl::PortType start;
    static const efscape::impl::PortType stop;

    /// Model output port
    static const efscape::impl::PortType out;

  private:
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive & ar)
    {
      ar( cereal::make_nvp("adevs::Atomic",
			   cereal::base_class<efscape::impl::DEVS >(this) ),
	  CEREAL_NVP(period),
	  CEREAL_NVP(sigma),
	  CEREAL_NVP(count) );
    }
  
    /// Model state variables
    double period, sigma;
    int count;
  };

}
  
#endif
