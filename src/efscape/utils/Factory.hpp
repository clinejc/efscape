// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Factory.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#ifndef EFSCAPE_UTILS_FACTORY_HPP
#define EFSCAPE_UTILS_FACTORY_HPP

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <map>
#include <set>

namespace efscape {
  namespace utils {

    /**
     * Defines an object factory with a metadata store.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.0.0 created 25 Nov 2015, revised 08 Dec 2015
     *
     * @tparam IdentifierType factory key type
     * @tparam BaseType target object class
     */
    template <typename IdentifierType, typename BaseType>
    class Factory {
    public:
	
      Factory() {}
      ~Factory() {}

      /**
       * Registers an object creator.
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @tparm DerivedType
       * @param id factory key
       * @param properties type properties
       * @param creator factory creator
       * @returns whether registration was successful
       */
      template <typename DerivedType>
      bool registerType(const IdentifierType& id,
			boost::property_tree::ptree properties=boost::property_tree::ptree()) {

	bool lb_registered =
	  (mCF_factory_map.
	   insert( std::make_pair(id,
				  boost::factory<DerivedType*>()) ) ).second;
	if (lb_registered)
	  mCC_properties_map[id] = properties;

	return lb_registered;
      }

      /**
       * Creates an object of the specified type.
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @param id type key
       * @returns handle to new objec of type BaseType if successful
       */
      BaseType* createObject(const IdentifierType& id) {
	BaseType* lCp_object = NULL;
	typename std::map< IdentifierType, boost::function<BaseType*()> >::iterator iter;
	if ( (iter = mCF_factory_map.find(id) ) !=
	     mCF_factory_map.end() )
	lCp_object = (iter->second)();

	return lCp_object;
      }

      /**
       * Returns model info for type <aC_typename>.
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @param aC_classname model class name
       * @returns properties for type <aC_classname> in a property tree
       */
      boost::property_tree::ptree
      getProperties(IdentifierType aC_typename) {
	typename std::map<IdentifierType, boost::property_tree::ptree>::iterator iter;
	if ( (iter = mCC_properties_map.find(aC_typename) ) !=
	   mCC_properties_map.end() )
	  return iter->second;

	return boost::property_tree::ptree();
      }
      
      /**
       * Returns set of IDs for all types in the repository.
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @returns set of IDs for all types in the repository
       */
      std::set<IdentifierType> getTypeIDs() {
	std::set<IdentifierType> lCC_TypeIDs;
	std::pair< IdentifierType, boost::function< BaseType*() > > factory_pair;
	BOOST_FOREACH(factory_pair, mCF_factory_map) {
	  lCC_TypeIDs.insert(factory_pair.first);
	}

	return lCC_TypeIDs;
      }
      
    private:

      /** factory map */
      std::map< IdentifierType, boost::function<BaseType*()> > mCF_factory_map;

      /** properties map */
      std::map<IdentifierType, boost::property_tree::ptree> mCC_properties_map;
    };

  } // namespace utils
}   // namespace efscape

#endif	// #ifndef EFSCAPE_UTILS_FACTORY_HPP
