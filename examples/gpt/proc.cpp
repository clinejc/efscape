#include "proc.hpp"

#include "gpt_config.hpp"

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelType.ipp>
#include <efscape/utils/type.hpp>


namespace gpt {

  /// Create unique 'names' for the model ports.
  const efscape::impl::PortType proc::in("in");
  const efscape::impl::PortType proc::out("out");
  const efscape::impl::PortType proc::properties_in("properties_in");
  
  /// define proc model metadata
  procType::procType() :
    efscape::impl::ModelType(efscape::utils::type<proc>(),
  			     "A processor requires a fixed period of time to service a job.  The processor can serve only one job at a time.  If the processor is busy, it simply discards incoming jobs.",
  			     gcp_libname,
  			     1)
  {

    //========================================================================
    // input ports:
    // * "in": gpt::job
    //========================================================================
    job j;
    Json::Value lC_portValue =
      efscape::impl::exportDataTypeToJSON<job>(j);

    addInputPort(proc::in, lC_portValue);
    addInputPort(proc::properties_in, Json::Value());
		 
    //========================================================================
    // output ports:
    // * "out": gpt::job
    //========================================================================
    addOutputPort(proc::out, lC_portValue);

    //========================================================================
    // properties
    //========================================================================
    Json::Value lC_properties;
    lC_properties["processing_time"] = 1.0;
      
    setProperties(lC_properties);
  }

  /// instantiate model type
  efscape::impl::ModelTypePtr proc::mSCp_modelType;

  const efscape::impl::ModelType& proc::getModelType()
  {
    if (mSCp_modelType == nullptr)
     mSCp_modelType.reset(new procType());
    return *mSCp_modelType;
  }
 
  //============================================================================
  // class proc definition
  //============================================================================ 
  proc::proc():
    adevs::Atomic<efscape::impl::IO_Type>(),
    processing_time(1.0),
    sigma(DBL_MAX),
    val(nullptr)
  {
    t = 0.0;
  }

  proc::proc(double proc_time):
    adevs::Atomic<efscape::impl::IO_Type>(),
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
    val = nullptr;
  }

  void proc::delta_ext(double e, const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    t += e;
    
    // Look for input on the property_in port.
    adevs::Bag<efscape::impl::IO_Type>::const_iterator iter;
    for (iter = x.begin(); iter != x.end(); iter++) {
      if ((*iter).port == properties_in) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Loading properties for proc...");
	try {
	  Json::Value args = boost::any_cast<Json::Value>( (*iter).value );
	  Json::Value lC_property = args["processing_period"];
	  if (lC_property.isDouble()) {
	    processing_time = lC_property.asDouble();
	    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			  "Loaded property <processing_time> = "
			  << processing_time << " for proc...");
	  }	  
	}
	catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <Json::Value>");
	}
	return;		// need to short-circuit normal processing
      }
    }

    // If we are waiting for a job
    if (sigma == DBL_MAX) {
      // Make a copy of the job (original will be destroyed by the
      // generator at the end of this simulation cycle).
      try {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Receiving input on port <"
		      << (*(x.begin())).port << ">...");
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Extracted job...");
	val.reset(new job(boost::any_cast<job>( (*(x.begin())).value ) ) );
	LOG4CXX_INFO(efscape::impl::ModelHomeI::getLogger(),
		     "Received a new job");
	// Wait for the required processing time before outputting the
	// completed job
	sigma = processing_time;
      }
      catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <job>");
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
    if (val == nullptr)
      return;
    
    // Produce a copy of the completed job on the out port
    efscape::impl::IO_Type pv(out,*val);
    y.insert(pv);
  }

  double proc::ta() { return sigma; }

  void proc::gc_output(adevs::Bag<efscape::impl::IO_Type>& g){}

  proc::~proc()
  {
  }
 
} // namespace gpt
