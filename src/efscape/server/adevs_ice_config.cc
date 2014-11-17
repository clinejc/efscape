// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_ice_config.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/server/adevs_ice_config.hh>

#include <IceUtil/Handle.h>

namespace efscape {

  namespace server {

    //----------------------------------------
    // class AdevsInputConsumer implementation
    //----------------------------------------

    /** default constructor */
    AdevsInputConsumer::AdevsInputConsumer() {}
    
    /** destructor */
    AdevsInputConsumer::~AdevsInputConsumer() {}


    //-----------------------------------------
    // class AdevsOutputProducer implementation
    //-----------------------------------------

    /** default constructor */
    AdevsOutputProducer::AdevsOutputProducer() {}
    
    /** destructor */
    AdevsOutputProducer::~AdevsOutputProducer() {}

    //---------------------------------------
    // class DataFrameProducer implementation
    //----------------------------------------

    /** default constructor */
    JsonOutputProducer::JsonOutputProducer() {}

    bool
    JsonOutputProducer::operator()(const Ice::Current& aCr_current,
				   adevs::Event<efscape::impl::IO_Type>&
				   aCr_internal_event,
				   ::efscape::Message& aCr_external_output)
    {
      // if event is associated with the following output ports:
      //  - 'json_out'
      // -  'geojson_out'
      // the event value is assumed to be stored in a string
      if (aCr_internal_event.value.port == "json_out" ||
	  aCr_internal_event.value.port == "geojson_out") {

	try {
	  std::string lC_string_value =
	    boost::any_cast<std::string>(aCr_internal_event.value.value);
	}
	catch (const boost::bad_any_cast &) {
	  return false;
	}
	return true;
      }

      return false;
    }

    // /**
    //  * constructor
    //  *
    //  * @param aC_OutputPort output port
    //  */
    // DataFrameProducer::DataFrameProducer(efscape::impl::PortType
    // 					 aC_OutputPort) {}

    // /**
    //  * Converts messages from the wrap`ped model for output.
    //  *
    //  * @param aCr_current current method invocation
    //  * @param aCr_internal_output output from internal model
    //  * @param aCr_external_output converted output
    //  */
    // bool
    // DataFrameProducer::operator()(const Ice::Current& aCr_current,
    // 				  const adevs::Bag<adevs::Event
    // 				  <efscape::impl::IO_Type> >&
    // 				  aCr_internal_output,
    // 				  ::efscape::Message& aCr_external_output)
    // {
    //   ::efscape::ContentPtr lCp_content;

    //   adevs::Bag< adevs::Event<efscape::impl::IO_Type> >::iterator iter;
    //   for (iter = aCr_internal_output.begin();
    // 	   iter != aCr_internal_output.end(); iter++) {
    // 	if ( (*iter).value.port == mC_OutputPort ) { // output port of source
    // 	  try {
    // 	    // data value should be a DataFrame smart pointer
    // 	    efscape::impl::DataFrameIPtr lCp_DataFrameI =
    // 	      boost::any_cast<efscape::impl::DataFrameIPtr>((*iter).value.value);

    // 	    if (lCp_DataFrameI.get() != 0) {

    // 	      std::cout << "DataFrameProducer(): converting a data frame...\n";

    // 	      ::efscape::data::DataFramePtr lCp_DataFrame =
    // 		new DataFrameTie(lCp_DataFrameI);

    // 	      // activate the DataFrame servant
    // 	      ::efscape::data::DataFramePrx lCp_DataFramePrx = 
    // 		  ::efscape::data::DataFramePrx::
    // 	      uncheckedCast(aCr_current.adapter
    // 			    ->addWithUUID(lCp_DataFrame));

    // 	      lCp_content = new ::efscape::Content(mC_OutputPort,
    // 						   lCp_DataFramePrx);

    // 	      aCr_external_output.push_back( lCp_content );

    // 	    } //  if (lCp_DataFrameI.get() != 0)
    // 	  }
    // 	  catch(const boost::bad_any_cast &) {
    // 	    std::cerr << ": unable to cast output as <DataFrame*>\n";
    // 	  }
	  
    // 	} // if ( (*iter)...
    //   }	  // for (iter = ...
    // }	  // DataFrameProducer::operator()(...)

  } // namespace server

} // namespace efscape
