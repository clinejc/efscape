// __COPYRIGHT_START__
// Package Name : efscape
// File Name : JsonDatasetI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_JSONDATASETI_HPP
#define EFSCAPE_SERVER_JSONDATASETI_HPP

#include <efscape/efscape.h>
#include <IceUtil/Handle.h>
#include <string>

namespace efscape {
  namespace server {

    // forward declarations
    class JsonDatasetI;

    // typedefs
    typedef IceUtil::Handle<JsonDatasetI> JsonDatasetIPtr;

    /**
     * a simple interface for accessing JSON datasets.
     * The components of a JSON dataset are:
     *  - schema
     *  - data
     *
     * ChangeLog:
     *  - 2014-10-30 Created class JsonDatasetI
     */
    class JsonDatasetI : virtual public JsonDataset
    {
    public:

      JsonDatasetI();
      JsonDatasetI(const ::std::string& aCr_data,
		  const ::std::string& aCr_schema);
      virtual ~JsonDatasetI();

      //
      // ICE interface efscape::JsonDataset
      //
      virtual ::std::string getSchema(const Ice::Current&);

      virtual ::std::string getData(const Ice::Current&);

      virtual void setData(const ::std::string&,
			   const ::std::string&,
			   const Ice::Current&);

    private:

      /** JSON dataset schema */
      ::std::string mC_schema;

      /** JSON dataset data */
      ::std::string mC_data;
    };

  } // namespace server
}   // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_JSONDATASETI_HPP
