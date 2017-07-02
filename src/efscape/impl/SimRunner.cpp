// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

// class definition
#include <efscape/impl/SimRunner.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/adevs_json.hpp>

#include <json/json.h>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

namespace efscape {
  
  namespace impl {

    // instantiate class data members
    const PortType SimRunner::clock_in =
      "clock_in";
    const PortType SimRunner::properties_in =
      "properties_in";


    SimRunner::SimRunner() :
      ModelWrapperBase()
    {      
      // initialize the clock
      mCp_ClockI.reset(new ClockI);     
    }

    SimRunner::~SimRunner() {
    }

    void SimRunner::delta_int() {
      // advance clock
      if (getTime() < adevs_inf<double>())
	getClockIPtr()->time() = getTime() + ta();
      
       ModelWrapperBase::delta_int();
    }

    void SimRunner::delta_ext(double e, const adevs::Bag<IO_Type>& xb) {
      // advance clock
      if (e >= 0)
	getClockIPtr()->time()+=e;
      
      // Attempt to "consume" input
      adevs::Bag<efscape::impl::IO_Type>::const_iterator i = xb.begin();

      for (; i != xb.end(); i++) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "SimRunner input on port <"
		      << (*i).port << ">");
	if ( (*i).port == "properties_in") { // event on <properties_in> port
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Found port=" << properties_in);
	  try {
	    // 1) try to extract a property tree
	    std::string lC_properties = boost::any_cast<std::string>( (*i).value );
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "properties="
			  << lC_properties);
	    convert_from_json(lC_properties);
	  }
	  catch(const boost::bad_any_cast &) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <std::string>");
	  }
	}
	else if ( (*i).port == "clock_in") { // event on <clock_in> port
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Found port=" << clock_in);
	  try {
	    // 2) try to extract a smart pointer to the clock
	    ClockI* lCp_clock =
	      new ClockI( boost::any_cast<ClockI>( (*i).value ) );
	    mCp_ClockI.reset( lCp_clock );
	  }
	  catch(const boost::bad_any_cast &) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <ClockI>");
	  }
	}
      }
	
      ModelWrapperBase::delta_ext(e, xb);
    }

    void SimRunner::delta_conf(const adevs::Bag<IO_Type>& xb) {
      // advance clock
      if (getTime() < adevs_inf<double>())
	getClockIPtr()->time() = getTime();

      ModelWrapperBase::delta_conf(xb);
    }

    double SimRunner::ta() {
     double ld_delta_t = ModelWrapperBase::ta();
      if (getTime() + ld_delta_t > getClockIPtr()->timeMax())
	return adevs_inf<double>();

      return ModelWrapperBase::ta();
    }
    
    void SimRunner::translateInput(const adevs::Bag<IO_Type>&
				   external_input,
				   adevs::Bag<adevs::Event<IO_Type> >&
				   internal_input)
    {
      adevs::Bag<IO_Type>::iterator iter;
      for (iter = external_input.begin(); iter != external_input.end();
	   iter++) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "passing on port <" << (*iter).port << ">");
	internal_input.insert(adevs::Event<IO_Type>(getWrappedModel().get(),
						    (*iter)));
      }
    }

    void SimRunner::translateOutput(const
				    adevs::Bag<adevs::Event<IO_Type> >&
				    internal_output,
				    adevs::Bag<IO_Type>& external_output)
    {
      adevs::Bag<adevs::Event<IO_Type> >::iterator iter;
      for (iter = internal_output.begin(); iter != internal_output.end();
	   iter++) {
	external_output.insert( IO_Type( (*iter).value ) );
      }
    }
    
    void SimRunner::gc_input(adevs::Bag<adevs::Event<IO_Type> >& g)
    {
    }

    void SimRunner::gc_output(adevs::Bag<efscape::impl::IO_Type>& g)
    {
    }

    void SimRunner::createModel()
      throw(adevs::exception)
    {
      // to be overridden by derived classes
    }
    
    //-------------------------
    // accessor/mutator methods
    //-------------------------

    boost::property_tree::ptree SimRunner::get_info() {
      boost::property_tree::ptree lC_ptree;
      lC_ptree.put("info",
    		   "Implements an adevs-based model wrapper that encapsulates a simulation model session");
      lC_ptree.put("library", "efscape-impl" );

      return lC_ptree;
    }

    ///
    /// model configuraiton/properties
    ///
    std::string SimRunner::convert_to_json() const
    {
      return mC_modelJson;
    }

    unsigned int
    SimRunner::convert_from_json(const std::string& aC_modelJson)
    {
      // and parse properies
      unsigned int li_property_cnt = 0;

      Json::Value lC_config;
      Json::Value lC_attribute;
      
      std::istringstream lC_buffer_in(aC_modelJson.c_str());
      lC_buffer_in >> lC_config;

      if ( (lC_attribute = lC_config["baseClassName"]).isString() ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Found <baseClassName>: already processed");
	++li_property_cnt;
      }

      if ( (lC_attribute = lC_config["className"]).isString() ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Found <className>: already processed");
	++li_property_cnt;
      }

      if ( (lC_attribute = lC_config["wrappedModel"]).isString() ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Found <wrappedModel>: already processed");
	++li_property_cnt;
      }

      if ( (lC_attribute = lC_config["time"]).isObject() ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Found time info: setting clock");

	efscape::impl::convert_from_json(lC_attribute, *mCp_ClockI);
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Simulation time units = " << mCp_ClockI->timeUnits() );
	++li_property_cnt;
      }
      else {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Attribute <time> is not an object and cannot be parsed!");
      }	// if ( (lC_attribute = lC_config["time"]).isObject() )

      return li_property_cnt;

    } // unsigned int SimRunner::convert_from_json()
    
    ///
    /// clock
    ///
    ClockIPtr& SimRunner::getClockIPtr() { return mCp_ClockI; }

    void SimRunner::setClockIPtr(const ClockIPtr& aCp_clock) {
      mCp_ClockI = aCp_clock;
    }

    const ClockI* SimRunner::getClock() const { return mCp_ClockI.get(); }

  } // namespace impl
  
}   // namespace efscape
