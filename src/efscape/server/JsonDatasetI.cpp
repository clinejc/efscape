// __COPYRIGHT_START__
// Package Name : efscape
// File Name : JsonDatasetI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/server/JsonDatasetI.hpp>

namespace efscape {
  namespace server {

    /** default constructor */
    JsonDatasetI::JsonDatasetI() {}


    /**
     * Constructor
     * 
     * @param aCr_data JSON data
     * @param aCr_schema JSON schema (optional)
     */
    JsonDatasetI::JsonDatasetI(const ::std::string& aCr_data,
			       const ::std::string& aCr_schema) :
      mC_data(aCr_data),
      mC_schema(aCr_schema)
    {
    }

    /** destructor */
    JsonDatasetI::~JsonDatasetI()
    {
      std::cout << "Deleting JsonDataset...\n";
    }

    /**
     * Returns JSON schema for this dataset
     *
     * @param current current method invocation
     * @returns JSON schema for this dataset
     */
    ::std::string
    JsonDatasetI::getSchema(const Ice::Current& current)
    {
      return mC_schema;
    }

    /**
     * Returns JSON data for this dataset
     *
     * @param current current method invocation
     * @returns JSON data for this dataset
     */
    ::std::string
    JsonDatasetI::getData(const Ice::Current& current)
    {
      return mC_data;
    }

    /**
     * Sets the data for this JSON dataset
     *
     * @param data data for this dataset
     * @param schema schema for this dataset
     * @param current current method invocation
     */
    void
    JsonDatasetI::setData(const ::std::string& data,
			  const ::std::string& schema,
			  const Ice::Current& current)
    {
      mC_data = data;
      mC_schema = schema;
    }

  } // namespace server
}   // namespace efscape
