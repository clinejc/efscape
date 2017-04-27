// __COPYRIGHT_START__
// Package Name : efscape
// File Name : EntityI.hh
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_ENTITY_I_HH
#define EFSCAPE_IMPL_ENTITY_I_HH

#include <boost/shared_ptr.hpp>
#include <string>

// boost serialization definitions
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

namespace efscape {

  namespace impl {

    /**
     * This class defines a basic Entity interface for the efscape library.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.7 created 18 Oct 2006, revised 16 Aug 2014
     *
     * ChangeLog:
     *   - 2014.08.16 removed Cloneable interface
     *   - 2009.09.13 removed reflcpp implementation
     *   - 2009.03.06 removed implementation of efscape/Entity
     *   - 2007.12.25 moved protected method "name(const char*)" to public
     *   - 2007.08.26 getName() method changed from const as per interface
     *   - 2006.12.26 replaces impl/EntityBase
     *   - 2006.12.24 added parent class 'efscape/Entity'
     */
    class EntityI
    {
      friend class boost::serialization::access;

    public:

      EntityI();
      virtual ~EntityI();

      /** @returns model name */
      const char* name() const { return mC_name.c_str(); }

      /** @param acp_name name to set */
      virtual void name( const char* acp_name ) { mC_name = acp_name; }

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
	// name
	ar & boost::serialization::make_nvp("name", mC_name);
      }

      /** entity name */
      std::string mC_name;

    };				// class EntityI definition

  } // namespace impl

} // namespace efscape

BOOST_CLASS_VERSION(efscape::impl::EntityI,7)

#endif	// #ifndef EFSCAPE_IMPL_ENTITY_I_HH
