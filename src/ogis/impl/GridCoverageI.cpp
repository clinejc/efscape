// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GridCoverageI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <ogis/impl/GridCoverageI.ipp>

#include <ogis/impl/SampleDimensionI.hpp>
#include <ogis/impl/ogis_utils.hpp>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * @param aCp_dataset handle to GDAL dataset
     */
    GridCoverageI::GridCoverageI(GDALDataset* aCp_dataset) :
      CoverageI(aCp_dataset)
    {
      init();
    } // GridCoverageI::GridCoverageI(GDALDataset*)

    /**
     * constructor
     *
     * @param aCpr_dataset smart handle to GDAL dataset
     */
    GridCoverageI::GridCoverageI(const boost::shared_ptr<GDALDataset>&
				 aCpr_dataset) :
      CoverageI(aCpr_dataset)
    {
      init();
    } // GridCoverageI::GridCoverageIconst boost::shared_ptr<GDALDataset>&)

    /** destructor */
    GridCoverageI::~GridCoverageI () {
      std::cout << "Deleting grid <" << CoverageI::coverageName() << ">\n"; 
    }

    /**
     * Initializes the grid coverage.
     */
    void GridCoverageI::init() {

      if (mCp_dataset.get() == NULL)
	return;

      // initialize sample dimensions
      for (int iBand = 0; iBand < mCp_dataset->GetRasterCount(); iBand++) {

	// get the corresponding GDAL raster band
	GDALRasterBand* lCp_band = mCp_dataset->GetRasterBand(iBand+1);
	
	// create a new sample dimension and insert it into the vector
	SampleDimensionIPtr
	  lCp_SampleDimension(new SampleDimensionI(lCp_band));

	// note: the following is an std::vector<...>
	mCp1_SampleDimensions.push_back( lCp_SampleDimension );
      }

      // initialize dimension names
      if (mCp_dataset->GetRasterCount() > 1)
	mC1_DimensionNames.push_back( "z" );
      mC1_DimensionNames.push_back( "y" );
      mC1_DimensionNames.push_back( "x" );

    } // GridCoverageI::init()

    /**
     * Returns the geometry of this grid coverage.
     *
     * @param current method invocation
     * @returns grid geometry
     */
    ::gc::GcGridGeometry
    GridCoverageI::gridGeometry(const Ice::Current& current)
    {
      // create a grid 
      gc::GcGridGeometry lC_GridGeometry;

      int li_nZSize = mCp_dataset->GetRasterCount();
      int li_nYSize = mCp_dataset->GetRasterYSize();
      int li_nXSize = mCp_dataset->GetRasterYSize();

      if (li_nZSize > 1) {
	lC_GridGeometry.gridRange.resize(3);
	lC_GridGeometry.gridRange[2].min = 0;
	lC_GridGeometry.gridRange[2].max = li_nZSize-1;
      }
      else
	lC_GridGeometry.gridRange.resize(2);

      lC_GridGeometry.gridRange[0].min = 0;
      lC_GridGeometry.gridRange[0].max = li_nYSize-1;
      lC_GridGeometry.gridRange[1].min = 0;
      lC_GridGeometry.gridRange[1].max = li_nXSize-1;
      
      return lC_GridGeometry;

    } // GridCoverageI::gridGeometry

    /**
     * Gets raster data block as boolean.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::getDataBlockAsBoolean(const ::gc::GcGridRange& aCr_range,
					 ::gc::BooleanSeq& aCr_data,
					 const Ice::Current& current)
    {
      this->getDataBlock<bool>(aCr_range, aCr_data);
    }

    /**
     * Gets raster data block as byte.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::getDataBlockAsByte(const ::gc::GcGridRange& aCr_range,
				      ::gc::ByteSeq& aCr_data,
				      const Ice::Current& current)
    {
      this->getDataBlock<Ice::Byte>(aCr_range, aCr_data);
    }

    /**
     * Gets raster data block as integer.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::getDataBlockAsInt(const ::gc::GcGridRange& aCr_range,
				     ::gc::IntSeq& aCr_data,
				     const Ice::Current& current)
    {
      this->getDataBlock<int>(aCr_range, aCr_data);
    }

    /**
     * Gets raster data block as double.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::getDataBlockAsDouble(const ::gc::GcGridRange& aCr_range,
					      ::gc::DoubleSeq& aCr_data,
					      const Ice::Current& current)
    {
      this->getDataBlock<double>(aCr_range, aCr_data);
    }

    /**
     * Sets raster data block as boolean.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::setDataBlockAsBoolean(const ::gc::GcGridRange& aCr_range,
					 const ::gc::BooleanSeq& aCr_data,
					 const Ice::Current& current)
    {
      this->setDataBlock<bool>(aCr_range, aCr_data);
    }

    /**
     * Sets raster data block as byte.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::setDataBlockAsByte(const ::gc::GcGridRange& aCr_range,
				      const ::gc::ByteSeq& aCr_data,
				      const Ice::Current& current)
    {
      this->setDataBlock<Ice::Byte>(aCr_range, aCr_data);
    }

    /**
     * Sets raster data block as integer.
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::setDataBlockAsInt(const ::gc::GcGridRange& aCr_range,
				     const ::gc::IntSeq& aCr_data,
				     const Ice::Current& current)
    {
      this->setDataBlock<int>(aCr_range, aCr_data);
    }

    /**
     * Sets raster data block as double..
     *
     * @param aCr_range grid range
     * @param aCr_data grid data block
     * @param current method invocation
     */
    void
    GridCoverageI::setDataBlockAsDouble(const ::gc::GcGridRange& aCr_range,
					const ::gc::DoubleSeq& aCr_data,
					const Ice::Current& current)
    {
      this->setDataBlock<double>(aCr_range, aCr_data);
    }

    //----------------------
    // grid accessor methods
    //----------------------
    /**
     * Returns true if the specified range is contained by the grid
     *
     * @param aCr_GridRange grid range
     * @returns true if the specified range is contained by the grid
     */
    bool GridCoverageI::containsRange(const gc::GcGridRange& aCr_GridRange) {

      int li_nZSize = mCp_dataset->GetRasterCount();
      int li_nYSize = mCp_dataset->GetRasterYSize();
      int li_nXSize = mCp_dataset->GetRasterXSize();

      gc::GcGridRange lC_GridRange;

      if ( aCr_GridRange.size() == 2 ) {
	// prepend "z" dimension to grid range
	gc::range lC_range;
	lC_range.min = 0;
	lC_range.max = 0;
	lC_GridRange.push_back(lC_range);
	lC_GridRange.push_back( aCr_GridRange[0]);
	lC_GridRange.push_back( aCr_GridRange[1]);
      }
      else if (aCr_GridRange.size() == 3) {
	lC_GridRange = aCr_GridRange;
      }
      else {
	std::cerr << "GridCoverageI::containsRange(): "
		  << "*** error -- incorrect dimensions = "
		  << lC_GridRange.size() << " ***\n";
	return false;
      }

      if ( lC_GridRange[0].min > lC_GridRange[0].max ||
	   lC_GridRange[1].min > lC_GridRange[1].max ||
	   lC_GridRange[2].min > lC_GridRange[2].max) {
	std::cerr << "GridCoverageI::containsRange(): "
		  << "*** error --  min exceeds max ***\n";
	return false;
      }

      if ( lC_GridRange[0].min < 0 ||
	   lC_GridRange[1].min < 0 ||
	   lC_GridRange[2].min < 0) {
	std::cerr << "GridCoverageI::containsRange(): "
		  << "*** error --  min out of bounds ***\n";
	return false;
      }


      if ( lC_GridRange[0].max >= (unsigned int)li_nYSize  ||
	   lC_GridRange[1].max >= (unsigned int)li_nXSize ||
	   lC_GridRange[2].max >= (unsigned int)li_nZSize) {
	std::cerr << "GridCoverageI::containsRange(): "
		  << "*** error --  min out of bounds ***\n";
	return false;
      }

      return true;

    } // GridCoverageI::containsRange(...)

  } // namespace impl

} // namespace ogis
