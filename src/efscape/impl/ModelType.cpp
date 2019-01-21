// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.cpp
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

#include <efscape/impl/ModelType.hpp>
#include <efscape/impl/efscapelib.hpp>

namespace efscape {
  namespace impl {

    ModelType::ModelType()
    {
      mC_attributes["typeName"] = "";
      mC_attributes["description"] = "";
      mC_attributes["libraryName"] = "";
      mC_attributes["url"] = "";
      mC_attributes["version"] = 1;

      mC_attributes["ports"]["inputs"] = Json::Value();
      mC_attributes["ports"]["outputs"] = Json::Value();

      mC_attributes["properties"] = Json::Value();
    }

    ModelType::ModelType(std::string aC_typeName,
			 std::string aC_description,
			 std::string aC_libraryName,
			 int ai_version)
    {
      mC_attributes["typeName"] = aC_typeName;
      mC_attributes["description"] = aC_description;
      mC_attributes["libraryName"] = aC_libraryName;
      mC_attributes["url"] = gcp_liburl;
      mC_attributes["version"] = ai_version;

      mC_attributes["ports"]["inputs"] = Json::Value();
      mC_attributes["ports"]["outputs"] = Json::Value();

      mC_attributes["properties"] = Json::Value();
    }

    ModelType::~ModelType() {}

    //
    // accessor/mutator methods
    //
    
    std::string ModelType::typeName() const {
      return mC_attributes["typeName"].asString();
    }

    std::string ModelType::description() const {
      return mC_attributes["description"].asString();
    }

    std::string ModelType::libraryName() const {
      return mC_attributes["libraryName"].asString();
    }

    int ModelType::version() const {
      return mC_attributes["version"].asInt();
    }
    
    Json::Value
    ModelType::addInputPort(std::string aC_portName,
			    Json::Value aC_portValue)
    {
      mC_attributes["ports"]["inputs"][aC_portName] = aC_portValue;
      return mC_attributes["ports"]["inputs"];
    }

    Json::Value
    ModelType::addOutputPort(std::string aC_portName,
			     Json::Value aC_portValue)
    {
      mC_attributes["ports"]["outputs"][aC_portName] = aC_portValue;
      return mC_attributes["ports"]["outputs"];
    }

    Json::Value
    ModelType::setProperties(Json::Value aC_properties)
    {
      mC_attributes["properties"] = aC_properties;
      
      return mC_attributes["properties"];
    }

    Json::Value
    ModelType::getProperties() const {
      return mC_attributes["properties"];
    }

    Json::Value ModelType::toJSON() const {
      return mC_attributes;
    }
    
  } // namespace impl
} // namespace efscape
