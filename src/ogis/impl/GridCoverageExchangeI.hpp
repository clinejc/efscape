// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GridCoverageExchangeI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_GRIDCOVERAGEEXCHANGE_I_HPP
#define OGIS_IMPL_GRIDCOVERAGEEXCHANGE_I_HPP

#include <ogis/ogc_gc.h>

#include <boost/shared_ptr.hpp>
#include <string>

// forward declaration
class GDALDriverManager;
class GDALDataset;

namespace ogis {

  namespace impl {

    class GridCoverageI;

    /**
     * Provides a partial implementation of the Open GIS GridCoverageExchange
     * interface. Implementation of the interface is delegated to the gdal
     * GDALDriverManager and GDALDriver classes.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 5.00 revised 11 Apr 2009
     *
     * ChangeLog:
     *   - 2009.04.16 Moved back to ogis::impl
     *   - 2009.04.14 Updated to support use of GDALDriverManager
     *   - 2007.10.21 Converted from CORBA to ICE.
     *   - 2006.08.29 Final snapshot of CORBA implementation.
     */
    class GridCoverageExchangeI : virtual public ::gc::GcGridCoverageExchange
    {

    public:

      GridCoverageExchangeI();
      virtual ~GridCoverageExchangeI();

      //
      // ICE interface efscape::Model
      //
      virtual ::Ice::Long numFormats(const Ice::Current&);

      virtual ::gc::StringSeq metadataNames(const Ice::Current&);

      virtual ::std::string getMetadataValue(const ::std::string&,
					     const Ice::Current&);

      virtual ::gc::GcFormat getFormat(::Ice::Long,
				       const Ice::Current&);

      virtual ::gc::StringSeq listSubNames(const ::std::string&,
					   const Ice::Current&);

      virtual ::gc::GcGridCoveragePtr createFromName(const ::std::string&,
						     const Ice::Current&);

      virtual void exportTo(const ::gc::GcGridCoveragePtr&,
			    const ::std::string&,
			    const ::std::string&,
			    const Ice::Current&);

      //-------------------------------
      // implementation (local) methods
      //-------------------------------

      // grid creation methods
      GridCoverageI*
      CreateGridCoverageI(const boost::shared_ptr<GDALDataset>&
			  aCpr_dataset);

    protected:

      /** handle to GDAL driver manager */
      GDALDriverManager* mCp_DriverManager;

    };

  } // namespace impl

} // namespace impl

#endif	// #ifndef OGIS_IMPL_GRIDCOVERAGEEXCHANGE_I_HPP
