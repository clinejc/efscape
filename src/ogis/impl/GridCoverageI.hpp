// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GridCoverageI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_GRIDCOVERAGE_I_HPP
#define OGIS_IMPL_GRIDCOVERAGE_I_HPP

// parent class definitions
#include <ogis/impl/CoverageI.hpp>
#include <ogis/ogc_gc.h>

#include <gdal_priv.h>		// gdal is used to implement coverage

#include <vector>

namespace ogis {

  namespace impl {

    /**
     * This class implements a OGC-based CORBA interface for Grid Coverages.
     * Utilizes a GDALDataset (inherited from parent class CoverageI) for the
     * underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 5.00 created 31 January, revised 16 Apr 2009
     *
     * ChangeLog:
     *   - 2009.04.16 Moved back to ogis::impl
     *   - 2009.04.14 Updated to support use of GDALDataset for implementation
     *   - 2007-10-24 Changes to implementation
     *     - Changed visibility of constructors to public
     *     - Added smart pointers for the wrapped dataset and geogrid
     *   - 2007-09-04 Class name changed to GridCoverageI
     *   - 2007-09-04 Moved from ogis::impl to efscape::ogis
     *   - 2007-09-03 Converted from CORBA to ICE.
     */
    class GridCoverageI: virtual public ::gc::GcGridCoverage,
			 virtual public CoverageI
    {
      friend class GridCoverageExchangeI;

    public:

      //------------------
      // ICE grid services
      //------------------
      virtual ::gc::GcGridGeometry gridGeometry(const Ice::Current&);

      virtual void getDataBlockAsBoolean(const ::gc::GcGridRange&,
					 ::gc::BooleanSeq&,
					 const Ice::Current&);

      virtual void getDataBlockAsByte(const ::gc::GcGridRange&,
				      ::gc::ByteSeq&,
				      const Ice::Current&);

      virtual void getDataBlockAsInt(const ::gc::GcGridRange&,
				     ::gc::IntSeq&,
				     const Ice::Current&);


      virtual void getDataBlockAsDouble(const ::gc::GcGridRange&,
                                      ::gc::DoubleSeq&,
                                      const Ice::Current&);

      virtual void setDataBlockAsBoolean(const ::gc::GcGridRange&,
					 const ::gc::BooleanSeq&,
					 const Ice::Current&);

      virtual void setDataBlockAsByte(const ::gc::GcGridRange&,
				      const ::gc::ByteSeq&,
				      const Ice::Current&);

      virtual void setDataBlockAsInt(const ::gc::GcGridRange&,
				     const ::gc::IntSeq&,
				     const Ice::Current&);

      virtual void setDataBlockAsDouble(const ::gc::GcGridRange&,
					const ::gc::DoubleSeq&,
					const Ice::Current&);

      //------------------------------------------------------------------------
      // implementation of local (server-side) methods
      //------------------------------------------------------------------------
      GridCoverageI (GDALDataset* aCp_dataset);
      GridCoverageI (const boost::shared_ptr<GDALDataset>& aCpr_dataset);
      GridCoverageI (const GridCoverageI& aCr_other);

      virtual ~GridCoverageI();

    protected:

      virtual void init();

      template <typename Type>
      void getDataBlock(const ::gc::GcGridRange& aCr_GridRange,
			std::vector<Type>& aT1_data);

      template <typename Type>
      void setDataBlock(const ::gc::GcGridRange& aCr_GridRange,
			const std::vector<Type>& aT1_data);

      bool containsRange( const gc::GcGridRange& aCr_GridRange );

    }; // class GridCoverageI

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_GRIDCOVERAGE_I_HPP
