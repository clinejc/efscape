// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_decorator_serialization.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef __adevs_decorator_serialization_hpp_
#define __adevs_decorator_serialization_hpp_

#include <efscape/impl/adevs_config.hpp>

#include <adevs/adevs_serialization.hpp>

namespace boost {
  namespace serialization {

    /**
     * Free function for serialization save of the adevs::ModelDecorator<X>
     * class.
     *
     * @param ar reference to archive type
     * @param decorator reference to ModelDecorator model
     * @param version version number
     */
    template<class Archive>
    void save(Archive & ar, const efscape::impl::ModelDecorator& decorator,
	      const unsigned int version)
    {
      // invoke serialization save of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK >(decorator) );

      // save wrapped model
      const efscape::impl::DEVS* lCp_model = decorator.getWrappedModel();
      ar  & boost::serialization::make_nvp("WrappedModel",lCp_model);
    }

    /**
     * Free function for serialization load of the adevs::ModelDecorator<X>
     * class.
     *
     * @param ar reference to archive type
     * @param atomic reference to ModelDecorator model
     * @param version version number
     */
    template<class Archive>
    void load(Archive & ar, efscape::impl::ModelDecorator& decorator,
	      const unsigned int version)
    {
      // invoke serialization load of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK >(decorator) );

      // save wrapped model
      efscape::impl::DEVS* lCp_model;
      ar  & boost::serialization::make_nvp("WrappedModel",lCp_model);
      decorator.setWrappedModel(lCp_model);
    }

  }
}

#endif
