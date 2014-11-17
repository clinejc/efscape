// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_ice_config.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_ADEVS_ICE_HH
#define EFSCAPE_SERVER_ADEVS_ICE_HH

// parent class definitions
#include <Ice/Ice.h>
#include <efscape/efscape.h>
// #include <efscape/Data.h>

#include <efscape/impl/adevs_config.hh>

// #include <efscape/server/DataFrameTie.hh>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace efscape {
  namespace server {

    /**
     * Base class for converting external model input into an internal
     * event.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.0.2 created 21 Apr 2008, revised 27 Oct 2014
     */
    class AdevsInputConsumer
    {
    public:

      AdevsInputConsumer();
      virtual ~AdevsInputConsumer();

      /**
       * Converts external model input into an internal event.
       *
       * @param current method invocation
       * @param aCr_external_input reference to external input
       * @param aCr_internal_event reference to generated event
       * @returns whether an event was generated
       */
      virtual
      bool operator()(const Ice::Current& aCr_current,
		      const ::efscape::ContentPtr& aCr_external_input,
		      adevs::Event<efscape::impl::IO_Type>&
		      aCr_internal_event) = 0;
    };

    /**
     * Base class for converting internal model output into a message containing
     * an external event.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 21 Apr 2008, revised 29 Oct 2014
     */
    class AdevsOutputProducer
    {
    public:

      AdevsOutputProducer();
      virtual ~AdevsOutputProducer();

      virtual
      bool operator()(const Ice::Current& aCr_current,
		      adevs::Event<efscape::impl::IO_Type>&
		      aCr_internal_event,
		      ::efscape::Message& aCr_external_output) = 0;

    };

    // typedefs
    typedef boost::shared_ptr<AdevsInputConsumer> AdevsInputConsumerPtr;
    typedef boost::shared_ptr<AdevsOutputProducer> AdevsOutputProducerPtr;
    typedef std::pair<std::string,::Ice::ObjectPrx> ObjectPrxPair;
    typedef std::map<std::string,::Ice::ObjectPrx> ObjectPrxMap;

    /**
     * Inserts outgoing JSON data into a message a message containing
     * an external event.
     */
    class JsonOutputProducer : public AdevsOutputProducer
    {
    public:
      JsonOutputProducer();

      bool operator()(const Ice::Current& aCr_current,
		      adevs::Event<efscape::impl::IO_Type>&
		      aCr_internal_event,
		      ::efscape::Message& aCr_external_output);
    };

    /**
     * Inserts outgoing GridCoverage objects into a message containing external
     * events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.0.2 created 30 May 2009, updated 27 Oct 2014
     */
    class GridCoverageProducer : public AdevsOutputProducer
    {
    public:
      GridCoverageProducer(efscape::impl::PortType aC_OutputPort);

      bool operator()(const Ice::Current& aCr_current,
		      adevs::Event<efscape::impl::IO_Type>&
		      aCr_internal_event,
		      ::efscape::Message& aCr_external_output);
    protected:

      /** outgoing port */
      efscape::impl::PortType mC_OutputPort;

    };				// class DataFrameProducer

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_ADEVS_ICE_HH 
