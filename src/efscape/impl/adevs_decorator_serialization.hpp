// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_decorator_serialization.hpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
#ifndef __adevs_decorator_serialization_hpp_
#define __adevs_decorator_serialization_hpp_

#include <efscape/impl/adevs_config.hpp>

#include <adevs_serialization.hpp>

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
