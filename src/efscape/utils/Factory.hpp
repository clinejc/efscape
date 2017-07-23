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
#include <boost/foreach.hpp>
#include <json/json.h>
#include <map>
#include <set>

namespace efscape {
  namespace utils {

    /**
     * Defines an object factory with a metadata store.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 2.0.1 created 25 Nov 2015, revised 22 Jul 2017
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
       * @param properties type properties in JSON
       * @param creator factory creator
       * @returns whether registration was successful
       */
      template <typename DerivedType>
      bool registerType(const IdentifierType& id,
			Json::Value properties=Json::Value()) {

	bool lb_registered =
	  (mCF_factory_map.
	   insert( std::make_pair(id,
				  boost::factory<DerivedType*>()) ) ).second;
	if (lb_registered)
	  mCC_properties_map[id] = properties;

	return lb_registered;
      }
      
      /**
       * Registers an object creator.
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @param id factory key
       * @param aF_createObj object creator
       * @param properties type properties in JSON
       * @returns whether registration was successful
       */
      bool registerType(const IdentifierType& id,
			boost::function<BaseType* ()> aF_createObj,
			Json::Value properties=Json::Value()) {
	
	bool lb_registered =
	  (mCF_factory_map.
	   insert( std::make_pair(id,
				  aF_createObj) ) ).second;
	if (lb_registered)
	  mCC_properties_map[id] = properties;

	return lb_registered;
      }
      
      /**
       * Registers an object creator with arguments embedded in JSON
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @param id factory key
       * @param aF_createObj object creator
       * @returns whether registration was successful
       */
      bool
      registerTypeWithArgs(IdentifierType id,
			   boost::function<BaseType* (Json::Value)>
			   aF_createObj )
      {	
	bool lb_registered =
	  (mCF_factory_with_args_map.
	   insert( std::make_pair(id,
				  aF_createObj) ) ).second;
	return lb_registered;
      }
      
     /**
       * Creates an object of the specified type with arguments embedded in JSON
       *
       * @tparam IdentifierType factory key type
       * @tparam BaseType target object class
       * @param id type key
       * @param args JSON object containing arguments
       * @returns handle to new objec of type BaseType if successful
       */
      BaseType* createObject(const IdentifierType& id, Json::Value args) {
	BaseType* lCp_object = NULL;
	typename std::map< IdentifierType, boost::function<BaseType*(Json::Value)> >::iterator iter;
	if ( (iter = mCF_factory_with_args_map.find(id) ) !=
	     mCF_factory_with_args_map.end() )
	lCp_object = (iter->second)(args);

	return lCp_object;
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
       * @returns properties for type <aC_classname> in a JSON object
       */
      Json::Value
      getProperties(IdentifierType aC_typename) {
	typename std::map<IdentifierType, Json::Value>::iterator iter;
	if ( (iter = mCC_properties_map.find(aC_typename) ) !=
	   mCC_properties_map.end() )
	  return iter->second;

	return Json::Value();
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
	std::pair< IdentifierType, boost::function< BaseType*(Json::Value) > > factory_with_args_pair;
	
	BOOST_FOREACH(factory_pair, mCF_factory_map) {
	  lCC_TypeIDs.insert(factory_pair.first);
	}
	BOOST_FOREACH(factory_with_args_pair, mCF_factory_with_args_map) {
	  lCC_TypeIDs.insert(factory_with_args_pair.first);
	}
	
	return lCC_TypeIDs;
      }
      
    private:

      /** factory map */
      std::map< IdentifierType, boost::function<BaseType*()> > mCF_factory_map;

      /** factory with args map */
      std::map< IdentifierType, boost::function<BaseType*(Json::Value)> > mCF_factory_with_args_map;

      /** properties map */
      std::map<IdentifierType, Json::Value> mCC_properties_map;
    };

  } // namespace utils
}   // namespace efscape

#endif	// #ifndef EFSCAPE_UTILS_FACTORY_HPP
