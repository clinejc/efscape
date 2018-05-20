// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.cpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__

// class definition
#include <efscape/impl/SimRunner.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/impl/ModelType.hpp>

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
      mCp_ClockI->timeMax() = DBL_MAX;
    }

    SimRunner::SimRunner(Json::Value aC_parameters) :
      ModelWrapperBase()
    {
      // initialize the clock
      //
      mCp_ClockI.reset(new ClockI);
      mCp_ClockI->timeMax() = DBL_MAX;

      // retrieve <typeName>
      //
      Json::Value lC_modelTypeNameValue = aC_parameters["typeName"];
      if (!lC_modelTypeNameValue.isString()) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Missing <typeName>");
	return;
      }

      mC_modelTypeName = lC_modelTypeNameValue.asString();

      // retrieve the model properties
      //
      Json::Value lC_modelProperties = aC_parameters["properties"];

      // load and set wrapped model
      //      
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Retrieving the model from the factory");
      DEVSPtr lCp_model( Singleton<ModelHomeI>::Instance()
			 .getModelFactory()
			 .createObject( mC_modelTypeName, lC_modelProperties ) );

      if (lCp_model == nullptr) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Unable to create model <" << mC_modelName << ">");
	return;
      }

      setWrappedModel(lCp_model);
      
    } // SimRunner::SimRunner(Json::Value)


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
     
    ///
    /// clock
    ///
    ClockIPtr& SimRunner::getClockIPtr() { return mCp_ClockI; }
    const ClockIPtr& SimRunner::getClockIPtr() const { return mCp_ClockI; }

    void SimRunner::setClockIPtr(const ClockIPtr& aCp_clock) {
      mCp_ClockI = aCp_clock;
    }

    const ClockI& SimRunner::getClock() const { return *mCp_ClockI; }

  } // namespace impl
  
}   // namespace efscape