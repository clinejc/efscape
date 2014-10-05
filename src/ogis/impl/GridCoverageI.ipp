// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_GRIDCOVERAGE_I_IPP
#define OGIS_IMPL_GRIDCOVERAGE_I_IPP

#include <ogis/impl/GridCoverageI.hpp>

namespace ogis {

  namespace impl {

    /**
     * Utility function that returns the GDALDataType (enum) associated with
     * this type.
     *
     * @returns GDALDataType
     */
    template <class Type>
    GDALDataType getGDALDataType(void) {
      Type lT_value;
      char lc_value;
      unsigned short lus_value;
      short ls_value;
      unsigned long lul_value;
      long ll_value;
      float lf_value;
      double ld_value;

      if ( typeid(lT_value) == typeid( lc_value) )
	return (GDT_Byte);
      else if ( typeid(lT_value) == typeid( lus_value) )
	return (GDT_UInt16);
      else if ( typeid(lT_value) == typeid( ls_value) )
	return (GDT_Int16);
      else if ( typeid(lT_value) == typeid( lul_value) )
	return (GDT_UInt32);
      else if ( typeid(lT_value) == typeid( ll_value) )
	return (GDT_Int32);
      else if ( typeid(lT_value) == typeid( lf_value) )
	return (GDT_Float32);
      else if ( typeid(lT_value) == typeid( ld_value) )
	return (GDT_Float64);
      else			// default
	return (GDT_Float32);
	
    } // end of getGDALDataType<Type>()

    /**
     * Gets raster data block of type <Type> for the specified grid range.
     *
     * @tparam Type value type
     * @param aCr_GridRange grid range
     * @param aT1r_data grid data block
     */
    template <typename Type>
    void
    GridCoverageI::getDataBlock(const ::gc::GcGridRange& aCr_GridRange,
				std::vector<Type>& aT1r_data)
    {
      if (!containsRange(aCr_GridRange))
	throw gc::InvalidRange();

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
      else if (aCr_GridRange.size() == 3)
	lC_GridRange = aCr_GridRange;
      else			// containsRange(...) failed
	return;
 
      // Ice::Long ll_nZSize =
      // 	aCr_GridRange[0].max-aCr_GridRange[0].min+1;
      Ice::Long ll_nYSize =
	aCr_GridRange[1].max-aCr_GridRange[1].min+1;
      Ice::Long ll_nXSize =
	aCr_GridRange[2].max-aCr_GridRange[2].min+1;

      // prepare data buffer to insert band data
      aT1r_data.resize(0);

      // for each raster band
      Ice::Long ll_volume = ll_nYSize*ll_nXSize;
      Type* lT1_BandData = new Type [ll_volume];

      for (long z = aCr_GridRange[0].min; z <= aCr_GridRange[0].max; z++) {
	GDALRasterBand* lCp_band = mCp_dataset->GetRasterBand(z+1);

	if (lCp_band != NULL) {

	  // note: assuming contiguous storage and the bands (z,y,x)
	  lCp_band->RasterIO(GF_Read,
			     lC_GridRange[2].min,
			     lC_GridRange[1].min,
			     ll_nXSize,
			     ll_nYSize,
			     lT1_BandData,
			     ll_nXSize,
			     ll_nYSize,
			     getGDALDataType<Type>(),
			     0,
			     0);

	  // append band data
	  aT1r_data.insert(aT1r_data.end(),
			   &lT1_BandData[0],
			   &lT1_BandData[ll_volume]);
	} // if (lCp_band != NULL)
      }	  // for (long z = ...

      delete lT1_BandData;	// reclaim memory from band buffer

    } // GridCoverageI::getDataBlock<Type>(...)

    /**
     * Sets raster data block of type <Type> for the specified grid range.
     *
     * @tparam Type value type
     * @param aCr_GridRange grid range
     * @param aT1r_data grid data block
     */
    template <typename Type>
    void
    GridCoverageI::setDataBlock(const ::gc::GcGridRange& aCr_GridRange,
				const std::vector<Type>& aT1r_data)
    {
      if (!containsRange(aCr_GridRange))
	throw gc::InvalidRange();

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
      else if (aCr_GridRange.size() == 3)
	lC_GridRange = aCr_GridRange;
      else			// containsRange(...) failed
	return;
 
      Ice::Long ll_nZSize =
	aCr_GridRange[0].max-aCr_GridRange[0].min+1;
      Ice::Long ll_nYSize =
	aCr_GridRange[1].max-aCr_GridRange[1].min+1;
      Ice::Long ll_nXSize =
	aCr_GridRange[2].max-aCr_GridRange[2].min+1;

      // ensure that data block is the correct size
      Ice::Long ll_volume = ll_nZSize*ll_nYSize*ll_nXSize;

      if (aT1r_data.size() != ll_volume)
	throw gc::InvalidRange();

      // create a data buffer for a single band
      ll_volume = ll_nYSize*ll_nXSize; // volume for a single band
      // std::vector<Type> lT1_BandData;
      Type* lT1_BandData = new Type [ll_volume];

      // for each raster band
      for (long z = aCr_GridRange[0].min; z <= aCr_GridRange[0].max; z++) {
	GDALRasterBand* lCp_band = mCp_dataset->GetRasterBand(z+1);

	if (lCp_band != NULL) {

	  // copy band data to buffer
	  for (int idx = 0; idx < ll_volume; idx++)
	    lT1_BandData[idx] = aT1r_data[(z*ll_volume)+idx];

	  // note: assuming contiguous storage and the bands (z,y,x)
	  lCp_band->RasterIO(GF_Write,
			     lC_GridRange[2].min,
			     lC_GridRange[1].min,
			     ll_nXSize,
			     ll_nYSize,
			     &lT1_BandData[0],
			     ll_nXSize,
			     ll_nYSize,
			     getGDALDataType<Type>(),
			     0,
			     0);

	} // if (lCp_band != NULL)
      }	  // for (long z = ...

    } // GridCoverageI::setDataBlock<Type>(...)

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_GRIDCOVERAGE_I_IPP
