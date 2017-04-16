// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeSingleton.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP
#define EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP

#include <efscape/impl/singletondll_export.h>

namespace efscape {

  namespace impl {

    /**
     * External singleton interface (see loki/test).
     */
    template<class T>
    class Singleton
    {
    public:
      static T& Instance();
    };


  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP
