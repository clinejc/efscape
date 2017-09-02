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
    class ModelType;

    // typedefs
    typedef std::shared_ptr<ModelType> ModelTypePtr;

    /**
     * Helper function that exports a data type to JSON format
     *
     * @tparam DataType data type
     * @param value data value
     * @returns JSON value
     */
    template <class DataType>
    Json::Value exportDataTypeToJSON(DataType value);

    /**
     * A simple struction for hold information about an object class
     *
     * @author Jon Cline <clinej@alumni.stanford.edu>
     * @version 0.0.3 created 13 Jul 2017, updated 14 Aug 2017
     */
    class ModelType
    {
    public:
      
      /** default constructor */
      ModelType();

      /**
       * constructor
       *
       * @param aC_typeName type name
       * @param aC_description description
       * @param aC_libraryName library name
       * @param ai_version type version
       */
      ModelType(std::string aC_typeName,
		std::string aC_description,
		std::string aC_libraryName,
		int ai_version);

      virtual ~ModelType();

      //
      // accessor/mutator methods
      //
      std::string typeName() const;

      std::string description() const;

      std::string libraryName() const;

      int version() const;

      Json::Value addInputPort(std::string aC_portName,
			       Json::Value aC_portValue);

      Json::Value addOutputPort(std::string aC_portName,
			 Json::Value aC_portValue);

      Json::Value setProperties(Json::Value aC_properties);
      Json::Value getProperties() const;

      Json::Value toJSON() const;

    protected:

      /** model type name */
      std::string mC_typeName;

      /** description */
      std::string mC_description;

      /** library name */
      std::string mC_libraryName;

      /** model type version */
      int mi_version;

      /** input ports */
      Json::Value mC_inputPorts;

      /** output ports */
      Json::Value mC_outputPorts;

      /** model properties */
      Json::Value mC_properties;
      
    };				// class ModelType
    
  } // namespace impl
} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELTYPE_HPP
