// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeSingleton.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelHomeSingleton.hh>
#include <efscape/impl/ModelHomeI.hh>

#include <efscape/impl/ModelFactory.ipp>

typedef Loki::SingletonHolder<efscape::impl::ModelHomeI>
ModelHomeSingleton;

LOKI_SINGLETON_INSTANCE_DEFINITION(ModelHomeSingleton)

namespace efscape {

  namespace impl {

    // implementation of export singleton interfaces
    template<>
    ModelHomeI& Singleton<ModelHomeI>::Instance()
    {
      return ::ModelHomeSingleton::Instance();
    }

    // instantiate modelfactory
    template class ModelFactoryTmpl<DEVS>;

  } // namespace impl

} // namespace efscape
