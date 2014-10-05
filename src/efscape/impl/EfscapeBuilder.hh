// __COPYRIGHT_START__
// Package Name : efscape
// File Name : EfscapeBuilder.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_EFSCAPEBUILDER_HH
#define EFSCAPE_IMPL_EFSCAPEBUILDER_HH

#include <efscape/impl/ModelBuilder.hh>

namespace efscape {

  namespace impl {

    /**
     * Implements an interface for creating a model by parsing a model
     * configuration file in boost::serialization XML format.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.02 created 08 Feb 2008, revised 28 Feb 2009
     *
     * ChangeLog:
     *   - 2009.02.28 Changed return type from ModelI* to DEVS*
     *   - 2008.02.08 Create class EfscapeBuilder.
     */
    class EfscapeBuilder : public ModelBuilder
    {
    public:

      EfscapeBuilder();
      ~EfscapeBuilder();

      /** @return builder name */
      static const char* name() { return "boost_serialization"; }

      adevs::Devs<IO_Type>* parse(const char* acp_filename);

    };

  }

}

#endif	// #ifndef EFSCAPE_IMPL_EFSCAPEBUILDER_HH
