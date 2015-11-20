// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelBuilder.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELBUILDER_HH
#define EFSCAPE_IMPL_MODELBUILDER_HH

#include <efscape/impl/adevs_config.hh>

namespace efscape {

  namespace impl {

   /**
     * Provides an abstract interface for creating a model by parsing a model
     * configuration file.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.3 created 08 Feb 2008, revised 17 Nov 2015
     *
     * ChangeLog:
     *   - 2015-11-17 Created template<> class ModelBuilderTmpl to supercede
     *     * Supercedes the original base class ModelBuilder
     *   - 2009-02-28 Changed return type from ModelI* to DEVS*
     *   - 2008-02-08 Created class ModelBuilder
     */
    template <class BaseType>
    class ModelBuilderTmpl
    {
    public:
      ModelBuilderTmpl() {}
      virtual ~ModelBuilderTmpl() {}

      virtual BaseType* parse(const char* acp_filename) = 0;
    };

    typedef ModelBuilderTmpl<DEVS> ModelBuilder;
  }

}

#endif	// #ifndef EFSCAPE_IMPL_MODELBUILDER_HH
