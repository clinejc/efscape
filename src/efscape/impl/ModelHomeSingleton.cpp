// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeSingleton.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/impl/ModelHomeI.hpp>

#include <efscape/utils/Singleton.hpp>	 // alternative to Loki::Singleton

// instantiate Singleton template class
typedef  efscape::utils::Singleton< efscape::impl::ModelHomeI > ModelHomeSingleton;

namespace efscape {

  namespace impl {

    // implementation of export singleton interfaces
    template<>
    ModelHomeI& Singleton<ModelHomeI>::Instance()
    {
      return ::ModelHomeSingleton::instance();
    }

  } // namespace impl

} // namespace efscape
