// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelBuilder.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELBUILDER_HH
#define EFSCAPE_IMPL_MODELBUILDER_HH

#include <loki/Singleton.h>
#include <loki/Factory.h>
#include <efscape/impl/adevs_config.hh>

namespace efscape {

  namespace impl {

    /**
     * Provides an abstract interface for creating a model by parsing a model
     * configuration file.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.02 created 08 Feb 2008, revised 28 Feb 2009
     *
     * ChangeLog:
     *   - 2009.02.28 Changed return type from ModelI* to DEVS*
     *   - 2008.02.08 Create class ModelBuilder.
     */
    class ModelBuilder
    {
    public:

      ModelBuilder();
      virtual ~ModelBuilder();

      virtual adevs::Devs<IO_Type>* parse(const char* acp_filename) = 0;

    };

    /**
     * This template instantiates a factory for model builder objects.
     */
    typedef ::Loki::SingletonHolder
    <
      ::Loki::Factory< ModelBuilder, std::string >
      > TheBuilderFactory;

  }

}

#endif	// #ifndef EFSCAPE_IMPL_MODELBUILDER_HH
