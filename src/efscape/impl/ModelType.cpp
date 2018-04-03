// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelType.hpp>

namespace efscape {
  namespace impl {

    ModelType::ModelType() :
      mC_typeName(""),
      mC_description(""),
      mC_libraryName(""),
      mi_version(1)
    {
    }

    ModelType::ModelType(std::string aC_typeName,
			 std::string aC_description,
			 std::string aC_libraryName,
			 int ai_version) :
      mC_typeName(aC_typeName),
      mC_description(aC_description),
      mC_libraryName(aC_libraryName),
      mi_version(ai_version)
    {
    }

    ModelType::~ModelType() {}

    //
    // accessor/mutator methods
    //
    
    std::string ModelType::typeName() const {
      return mC_typeName;
    }

    std::string ModelType::description() const {
      return mC_description;
    }

    std::string ModelType::libraryName() const {
      return mC_libraryName;
    }

    int ModelType::version() const {
      return mi_version;
    }
    
    Json::Value
    ModelType::addInputPort(std::string aC_portName,
			    Json::Value aC_portValue)
    {
      mC_inputPorts[aC_portName] = aC_portValue;
      return mC_inputPorts;
    }

    Json::Value
    ModelType::addOutputPort(std::string aC_portName,
			     Json::Value aC_portValue)
    {
      mC_outputPorts[aC_portName] = aC_portValue;
      return mC_outputPorts;
    }

    Json::Value
    ModelType::setProperties(Json::Value aC_properties)
    {
      mC_properties = aC_properties;
      
      return mC_properties;
    }

    Json::Value
    ModelType::getProperties() const {
      return mC_properties;
    }

    Json::Value ModelType::toJSON() const {
      Json::Value lC_attributes;
      
      lC_attributes["typeName"] = mC_typeName;
      lC_attributes["description"] = mC_description;
      lC_attributes["libraryName"] = mC_libraryName;
      lC_attributes["version"] = mi_version;

      lC_attributes["ports"]["inputs"] = mC_inputPorts;
      lC_attributes["ports"]["outputs"] = mC_outputPorts;

      lC_attributes["properties"] = mC_properties;

      
      return lC_attributes;
    }
    
  } // namespace impl
} // namespace efscape
