#include "transd.hpp"

#include "gpt_config.hpp"

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelType.ipp>
#include <efscape/utils/type.hpp>


namespace gpt {

  /// Assign unique 'names' to ports
  const efscape::impl::PortType transd::ariv("ariv");
  const efscape::impl::PortType transd::solved("solved");
  const efscape::impl::PortType transd::out("out");
  const efscape::impl::PortType transd::log("log");
  const efscape::impl::PortType transd::properties_in("properties_in");
  
  /// define transd model metadata
  transdType::transdType() :
    efscape::impl::ModelType(efscape::utils::type<transd>(),
  			     "    The transducer computes various statistics about the performance of the queuing system.  It receives new jobs on its ariv port, finished jobs on its solved port, and generates an output on its out port when the observation interval has elapsed.",
  			     gcp_libname,
  			     1)
  {

    //========================================================================
    // input ports:
    // * "arriv": job
    // * "solved": job
    // * "properties_in": {}
    //========================================================================
    job j;
    Json::Value lC_portValue =
      efscape::impl::exportDataTypeToJSON<job>(j);
    addInputPort(transd::ariv, lC_portValue);
    addInputPort(transd::solved, lC_portValue);
    addInputPort(transd::properties_in, Json::Value() );
      
    //========================================================================
    // output ports:
    // * "out": job
    // * "log": Json::Value
    //========================================================================
    addOutputPort(transd::out, lC_portValue);
    addOutputPort(transd::log, efscape::utils::type<Json::Value>());

    //========================================================================
    // properties
    //========================================================================
    Json::Value lC_properties;
    lC_properties["observ_time"] = 10.0;
      
    setProperties(lC_properties);
  }

  /// instantiate model type
  efscape::impl::ModelTypePtr transd::mSCp_modelType;

  const efscape::impl::ModelType& transd::getModelType()
  {
    if (mSCp_modelType == nullptr)
     mSCp_modelType.reset(new transdType());
    return *mSCp_modelType;
  }
  
  //============================================================================
  // class transd definition
  //============================================================================   
  transd::transd():
    adevs::Atomic<efscape::impl::IO_Type>(),
    observation_time(1.0),
    sigma(observation_time),
    total_ta(0.0),
    t(0.0),
    is_broadcasting(false)
  {
  }
  
  transd::transd(double observ_time):
    adevs::Atomic<efscape::impl::IO_Type>(),
    observation_time(observ_time),
    sigma(observation_time),
    total_ta(0.0),
    t(0.0),
    is_broadcasting(false)
  {
  }

  void transd::delta_int() 
  {
    if (is_broadcasting) {
      is_broadcasting = false;
      if (sigma > 1)
	return;
    }
    
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

    Json::Value lC_message;
    lC_message["End time"] = t;
    lC_message["jobs arrived"] = (int)jobs_arrived.size ();
    lC_message["jobs solved"] = (int)jobs_solved.size();
    lC_message["AVERAGE TA"] = avg_ta_time;
    lC_message["THROUGHPUT"] = throughput;
    
    mC_messages.append(lC_message);
    is_broadcasting = true;

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

	  // try writing to a log
	  Json::Value lC_value;
	  lC_value["event"] = "start job";
	  lC_value["id"] = j.id;
	  lC_value["time"] = t;
	  mC_messages.append(lC_value);
	  
	  jobs_arrived.push_back(j);
	}
	catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <job>");
	}
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
	      
	      // try writing to a log
	      Json::Value lC_value;
	      lC_value["event"] = "finish job";
	      lC_value["id"] = j.id;
	      lC_value["time"] = t;
	      mC_messages.append(lC_value);

	      break;
	    }
	  }
	  j.t = t;
	  jobs_solved.push_back(j);
	}
	catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <job>");
	}
      }
    }

    // Look for input on the property_in port.
    for (iter = x.begin(); iter != x.end(); iter++) {
      if ((*iter).port == properties_in) {
	LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
		      "Loading properties for proc...");
	try {
	  Json::Value args = boost::any_cast<Json::Value>( (*iter).value );
	  Json::Value lC_property = args["observ_time"];
	  if (lC_property.isDouble()) {
	    observation_time = lC_property.asDouble();
	    sigma = observation_time;
	    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(),
			  "Loaded property <observ_time> = "
			  << observation_time << "> for transd...");
	  }
	  return;		// need to short-circuit normal processing
	}
	catch(const boost::bad_any_cast &) {
	  LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
			"Unable to cast input as <Json::Value>");
	}
      }
    }

    is_broadcasting = true;
    
    // Continue with next event time unchanged
    sigma -= e;

    // std::cout << mC_messages << std::endl;
    std::cout << "sigma = " << sigma << std::endl;
  }

  void transd::delta_conf(const adevs::Bag<efscape::impl::IO_Type>& x)
  {
    delta_int();
    delta_ext(0.0,x);
  }

  void transd::output_func(adevs::Bag<efscape::impl::IO_Type>& y)
  {
    if (!mC_messages.isNull()) {
      efscape::impl::IO_Type pv(log, mC_messages);
      y.insert(pv);
      mC_messages = Json::Value();
      
      // std::cout << mC_messages << std::endl;
      std::cout << "output_func() sigma = " << sigma << std::endl;
    }

    if (sigma > 1)
      return;
    
    /// Generate an output event to stop the generator
    job j;
    efscape::impl::IO_Type pv(out,j);
    y.insert(pv);
  }

  double transd::ta() {
    if (sigma <= 1) {
      // std::cout << mC_messages << std::endl;
      std::cout << "ta() sigma = " << sigma << std::endl;
    }
    if (is_broadcasting) {
      return 0.;
    }
    return sigma;
  }

  void transd::gc_output(adevs::Bag<efscape::impl::IO_Type>& g){}
 
}
