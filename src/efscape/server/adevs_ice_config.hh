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
     * Base class for converting external model input into a bag of internal
     * events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 21 Apr 2008, revised 21 Apr 2008
     */
    class AdevsInputConsumer
    {
    public:

      AdevsInputConsumer();
      virtual ~AdevsInputConsumer();

      virtual
      bool operator()(const Ice::Current& aCr_current,
		      const ::efscape::Message& aCr_external_input,
		      adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
		      aCr_internal_input) = 0;
    };

    /**
     * Base class for converting internal model output into a message containing
     * external events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 21 Apr 2008, revised 21 Apr 2008
     */
    class AdevsOutputProducer
    {
    public:

      AdevsOutputProducer();
      virtual ~AdevsOutputProducer();

      virtual
      bool operator()(const Ice::Current& aCr_current,
		      const adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
		      aCr_internal_output,
		      ::efscape::Message& aCr_external_output) = 0;

    };

    // typedefs
    typedef boost::shared_ptr<AdevsInputConsumer> AdevsInputConsumerPtr;
    typedef boost::shared_ptr<AdevsOutputProducer> AdevsOutputProducerPtr;
    typedef std::pair<std::string,::Ice::ObjectPrx> ObjectPrxPair;
    typedef std::map<std::string,::Ice::ObjectPrx> ObjectPrxMap;

    // /**
    //  * Inserts outgoing DataFrame objects into a message containing external
    //  * events.
    //  *
    //  * @author Jon C. Cline <clinej@stanfordalumni.org>
    //  * @version 0.02 created 21 Apr 2008, revised 30 May 2009
    //  */
    // class DataFrameProducer : public AdevsOutputProducer
    // {
    // public:
    //   DataFrameProducer(efscape::impl::PortType aC_OutputPort);

    //   bool operator()(const Ice::Current& aCr_current,
    // 		      const adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
    // 		      aCr_internal_output,
    // 		      ::efscape::Message& aCr_external_output);
    // protected:

    //   /** outgoing port */
    //   efscape::impl::PortType mC_OutputPort;

    // };				// class DataFrameProducer

    /**
     * Inserts outgoing GridCoverage objects into a message containing external
     * events.
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 30 May 2009
     */
    class GridCoverageProducer : public AdevsOutputProducer
    {
    public:
      GridCoverageProducer(efscape::impl::PortType aC_OutputPort);

      bool operator()(const Ice::Current& aCr_current,
		      const adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
		      aCr_internal_output,
		      ::efscape::Message& aCr_external_output);
    protected:

      /** outgoing port */
      efscape::impl::PortType mC_OutputPort;

    };				// class DataFrameProducer

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_ADEVS_ICE_HH 
