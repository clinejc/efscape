#ifndef __proc_hpp_
#define __proc_hpp_

// parent class and data member definitions
#include <efscape/impl/efscapelib.hpp>
#include <efscape/impl/ModelType.hpp>
#include "job.hpp"

// serialization definitions
#include <adevs_cereal.hpp>

#include <cstdlib>

namespace gpt {

  /** proc model metadata */
  class procType : public efscape::impl::ModelType
  {
  public:
    procType();
  };
  
  /*
    A processor requires a fixed period of time to service a job.
    The processor can serve only one job at a time.  It the processor
    is busy, it simply discards incoming jobs.
  */
  class proc: public adevs::Atomic<efscape::impl::IO_Type>
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
    double ta();
    
    /**
       Garbage collection. No heap allocation in output_func, so
       do nothing.
    */
    void gc_output(adevs::Bag<efscape::impl::IO_Type>& g);

    /// Destructor
    ~proc();

    /// Model input port
    static const efscape::impl::PortType in;
    static const efscape::impl::PortType properties_in;

    /// Model output port
    static const efscape::impl::PortType out;

    /**
     * Returns the model type
     *
     * @returns const reference to model type
     */
    static const efscape::impl::ModelType& getModelType();

  private:
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive & ar)
    {
      ar( cereal::make_nvp("adevs::Atomic",
			   cereal::base_class< adevs::Atomic<efscape::impl::IO_Type> >(this) ),
	  CEREAL_NVP(processing_time),
	  CEREAL_NVP(sigma),
	  CEREAL_NVP(val),
	  CEREAL_NVP(t) );
    }
  
    /// Model state variables
    double processing_time, sigma;
    std::shared_ptr<job> val;
    double t;

    /** pointer to model type */
    static efscape::impl::ModelTypePtr mSCp_modelType;

  };

} // namespace gpt

#endif
