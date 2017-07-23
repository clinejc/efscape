// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#ifndef EFSCAPE_IMPL_MODELTYPE_HPP
#define EFSCAPE_IMPL_MODELTYPE_HPP

#include <string>
#include <json/json.h>

namespace efscape {
  namespace impl {

    // forward declarations
    struct ModelType;

    // typedefs
    typedef std::shared_ptr<ModelType> ModelTypePtr;

    /**
     * Helper function that wraps a ModelType object in a smart pointer.
     *
     * @param aC_model_name type name
     * @param aC_description description
     * @param aC_library_name library name
     */
    ModelTypePtr createModelTypePtr(std::string aC_model_name,
				    std::string aC_description,
				    std::string aC_library_name);

    /**
     * A simple struction for hold information about an object class
     *
     * @author Jon Cline <clinej@alumni.stanford.edu>
     * @version 0.0.2 created 13 Jul 2017, updated 15 Jul 2017
     */
    class ModelType
    {
    public:
      
      /** default constructor */
      ModelType();

      /**
       * constructor
       *
       * @param aC_model_name type name
       * @param aC_description description
       * @param aC_library_name library name
       * @param ai_version type version
       */
      ModelType(std::string aC_model_name,
		std::string aC_description,
		std::string aC_library_name,
		int ai_version);

      virtual ~ModelType();

      //
      // accessor/mutator methods
      //
      std::string model_name() const;
	
      Json::Value toJSON() const;

    protected:

      void addInputPort(std::string aC_port_name,
			Json::Value aC_port_value);

      void addOutputPort(std::string aC_port_name,
			 Json::Value aC_port_value);

      void setProperties(Json::Value aC_properties);

      /** model type attributes*/
      Json::Value mC_attributes;

    };
    
  } // namespace impl
} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELTYPE_HPP
