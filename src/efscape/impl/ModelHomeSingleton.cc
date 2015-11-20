// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeSingleton.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelHomeSingleton.hh>
#include <efscape/impl/ModelHomeI.hh>

#include <efscape/impl/ModelFactory.ipp> // template implementations
// #include <loki/Singleton.h>		 // Loki::Singleton
#include <efscape/utils/Singleton.hpp>	 // alternative to Loki::Singleton

// start of Loki::Singleton implementation
// typedef Loki::SingletonHolder<efscape::impl::ModelHomeI>
// ModelHomeSingleton;

// LOKI_SINGLETON_INSTANCE_DEFINITION(ModelHomeSingleton)
// end of Loki::Singleton implementation

// instantiate Singleton template class
typedef  efscape::utils::Singleton< efscape::impl::ModelHomeI > ModelHomeSingleton2;

namespace efscape {

  namespace impl {

    // implementation of export singleton interfaces
    template<>
    ModelHomeI& Singleton<ModelHomeI>::Instance()
    {
      return ::ModelHomeSingleton2::instance();
      // return ::ModelHomeSingleton::Instance();
    }

  } // namespace impl

} // namespace efscape
