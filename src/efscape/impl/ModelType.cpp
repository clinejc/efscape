// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelType.hpp>

namespace efscape {
  namespace impl {

    ModelType::ModelType()
    {
      mC_attributes["model_name"] = "";
      mC_attributes["description"] = "";
      mC_attributes["library_name"] = "";
      mC_attributes["version"] = 1;

      mC_attributes["ports"]["inputs"] = Json::Value();
      mC_attributes["ports"]["outputs"] = Json::Value();

      mC_attributes["properties"] = Json::Value();
    }

    ModelType::ModelType(std::string aC_model_name,
			 std::string aC_description,
			 std::string aC_library_name,
			 int ai_version)
    {
      mC_attributes["model_name"] = aC_model_name;
      mC_attributes["description"] = aC_description;
      mC_attributes["library_name"] = aC_library_name;
      mC_attributes["version"] = ai_version;

      mC_attributes["ports"]["inputs"] = Json::Value();
      mC_attributes["ports"]["outputs"] = Json::Value();

      mC_attributes["properties"] = Json::Value();

    }

    ModelType::~ModelType() {}

    //
    // accessor/mutator methods
    //
    
    std::string ModelType::model_name() const {
      return mC_attributes["model_name"].asString();
    }
    
    void ModelType::addInputPort(std::string aC_port_name,
				 Json::Value aC_port_value)
    {
      mC_attributes["ports"]["inputs"][aC_port_name] = aC_port_value;
    }

    void ModelType::addOutputPort(std::string aC_port_name,
				  Json::Value aC_port_value)
    {
      mC_attributes["ports"]["outputs"][aC_port_name] = aC_port_value;

    }

    void ModelType::setProperties(Json::Value aC_properties)
    {
      mC_attributes["properties"] = aC_properties;
    }

    Json::Value ModelType::toJSON() const {
      return mC_attributes;
    }

    ModelTypePtr createModelTypePtr(std::string aC_model_name,
				  std::string aC_description,
				  std::string aC_library_name)
    {
      ModelTypePtr
	lCp_info(new
		 ModelType(aC_model_name,
			  aC_description,
			  aC_library_name,
			  1) );
      return lCp_info;
    }
    
  } // namespace impl
} // namespace efscape
