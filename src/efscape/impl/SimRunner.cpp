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

#include <boost/property_tree/json_parser.hpp>

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
      // initialize properties
      mCp_ptree.reset( new boost::property_tree::ptree );
      
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
	    setProperties( boost::any_cast<boost::property_tree::ptree>( (*i).value ) );
	  }
	  catch(const boost::bad_any_cast &) {
	    LOG4CXX_ERROR(ModelHomeI::getLogger(),
			  "Unable to cast input as <boost::property_tree::ptree>");
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
	internal_input.insert(adevs::Event<IO_Type>(getWrappedModel(),
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
    /// properties
    ///
    boost::property_tree::ptree SimRunner::getProperties() const
    {
      return *mCp_ptree;
    }
    
    unsigned int
    SimRunner::setProperties(const boost::property_tree::ptree& aC_pt)
    {
      // set ptree
      mCp_ptree.reset( new boost::property_tree::ptree(aC_pt) );

      // and parse properies
      unsigned int li_property_cnt = 0;

      // iterate through the child nodes
      BOOST_FOREACH( boost::property_tree::ptree::value_type const& rowPair,
		     aC_pt.get_child( "" ) ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "=> property "
		      << "<" << rowPair.first << ">="
		      << "<" << rowPair.second.get_value<std::string>() << ">");
	boost::optional<std::string> lC_propOpt =
	  aC_pt.get_optional<std::string>(rowPair.first);
	if (!lC_propOpt) {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"property access failed!");
	  continue;
	}

	// parse info from this node
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "property access ok!");
	
	// if this is a leaf (an attribute)
	if (rowPair.second.empty() && !rowPair.second.data().empty() ) {
	  if (rowPair.first == std::string("baseType") ) { // 1) base type
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found <baseType>: already processed");
	    ++li_property_cnt;
	  }
	  else if (rowPair.first == std::string("type") ) { // 2) type (class of model)
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found <type>: already processed");
	    ++li_property_cnt;
	  }
	  else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Attribute ignored");
	  }
	}
	// if this is a child node (object)
	else if (!rowPair.second.empty() && rowPair.second.data().empty()) {
	  if (rowPair.first == std::string("wrappedModel") ) { // 3) "wrappedModel"
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found wrapped model:"
			  << " should already have been created");
	    ++li_property_cnt;
	  }
	  else if (rowPair.first == std::string("time") ) { // 4) time
	    // 2) time
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Found time info: setting clock");

	    // extract sim "time" object info
	    std::ostringstream lC_buffer_out;
	    boost::property_tree::write_json( lC_buffer_out,
					      rowPair.second );

	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "time: " << lC_buffer_out.str());

	    // first attempt to extract the "time" object via picojson
	    std::istringstream lC_buffer_in(lC_buffer_out.str().c_str());
	    SimTime lC_time;
	    picojson::convert::from_string(lC_buffer_in.str(),
					   lC_time);
	      
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "time = {"
			  << "stopAt: " << lC_time.stopAt << ", "
			  << "delta_t: " << lC_time.delta_t << ","
			  << "units: " << lC_time.units << "}");

	    mCp_ClockI->timeDelta() = lC_time.delta_t;
	    mCp_ClockI->timeMax() = lC_time.stopAt;
	    mCp_ClockI->timeUnits(lC_time.units.c_str());
	    
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Simulator clock set for time interval ["
			  << mCp_ClockI->time() << ","
			  << mCp_ClockI->timeMax() << "], time delta = "
			  << mCp_ClockI->timeDelta() << ", units = "
			  << mCp_ClockI->units() << ", time units = "
			  << mCp_ClockI->timeUnits());

	    // second attempt to parse the "time" object
	    BOOST_FOREACH( boost::property_tree::ptree::value_type const& timePair, rowPair.second.get_child("") ) {
	      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			    "=> time attribute "
			    << "<" << timePair.first << ">="
			    << "<" << timePair.second.get_value<std::string>() << ">");
	      if (timePair.first == "delta_t") {
		lC_time.delta_t = timePair.second.get_value<double>();
	      }
	      else if (timePair.first == "stopAt") {
		lC_time.stopAt = timePair.second.get_value<double>();
	      }
	      else if (timePair.first == "units") {
		lC_time.units = timePair.second.get_value<std::string>();
	      }
	    }

	    mCp_ClockI->timeDelta() = lC_time.delta_t;
	    mCp_ClockI->timeMax() = lC_time.stopAt;
	    mCp_ClockI->timeUnits(lC_time.units.c_str());
	    
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Simulator clock set (2nd time) for time interval ["
			  << mCp_ClockI->time() << ","
			  << mCp_ClockI->timeMax() << "], time delta = "
			  << mCp_ClockI->timeDelta() << ", units = "
			  << mCp_ClockI->units() << ", time units = "
			  << mCp_ClockI->timeUnits());
	    
	    ++li_property_cnt;
	  }
	  else {
	    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			  "Object ignored");
	  }
	}
	else {
	  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
			"Neither a proper leaf or proper node");
	}
      }	//  BOOST_FOREACH(...

      return li_property_cnt;
    }

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
