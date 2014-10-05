// __COPYRIGHT_START__
// Package Name : efscape
// File Name : gdal_utils.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GDAL_UTILS_IPP
#define EFSCAPE_GIS_GDAL_UTILS_IPP

#include <gdal_priv.h>		// Geospatial Data Abstraction Library
#include <efscape/gis/Geogrid_impl.hh>

#include <vector>
#include <limits>
#include <iostream>

namespace efscape {
  namespace gis {

    /**
     * Utility function that returns the GDALDataType (enum) associated with
     * this type.
     *
     * @returns GDALDataType
     */
    template <class Type>
    GDALDataType getGDALDataType(void) {
      Type lT_value;
      GByte luc_value;
      char lc_value;
      unsigned short lus_value;
      short ls_value;
      unsigned long lul_value;
      long ll_value;
      float lf_value;
      double ld_value;

      if ( typeid(lT_value) == typeid( luc_value) )
	return (GDT_Byte);
      else if ( typeid(lT_value) == typeid( lc_value) )
	// This option causes data conversion problems (2009.06.26)
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
	
    } // getGDALDataType<Type>()

    /**
     * Utility template function that reads GDALDataset as a Geogrid
     *
     * @param aCr_dataset reference to GDALDataset
     * @param aCr_grid reference to geogrid
     * @returns exit status
     */
    template <class Type>
    int readGDALDataset(GDALDataset& aCr_dataset,
			Geogrid_impl<Type>& aCr_grid) {

      // get grid dimensions
      int li_nBands = aCr_dataset.GetRasterCount();
      int li_nRows = aCr_dataset.GetRasterYSize();
      int li_nCols = aCr_dataset.GetRasterXSize();

      double ld1_GeoTransform[6];
      aCr_dataset.GetGeoTransform(ld1_GeoTransform);

      // grid origin is the upper left corner
      double ld_minX = ld1_GeoTransform[0];
      double ld_maxY = ld1_GeoTransform[3];

      // grid resolution
      double ld_dx = ld1_GeoTransform[1];
      double ld_dy = ld1_GeoTransform[5];

      double ld_maxX = ld_minX + ( ld_dx * ((double)li_nCols) );
      double ld_minY = ld_maxY + ( ld_dy * ((double)li_nRows) );

      geos::geom::Envelope lC_envelope(ld_minX, ld_maxX,
				       ld_minY, ld_maxY);

      // grid projection
      aCr_grid.resize(li_nCols,
		      li_nRows,
		      li_nBands,
		      lC_envelope,
		      aCr_dataset.GetProjectionRef());

      // allocate data block for raster data
      std::vector<Type> lT1_data(li_nRows*li_nCols);

      for (int iBand = 1; iBand <= li_nBands; iBand++) {
	GDALRasterBand* lCp_band = aCr_dataset.GetRasterBand(iBand);
	if (lCp_band != 0) {

	  CPLErr le_status =
	    lCp_band->RasterIO( GF_Read, 0, 0, li_nCols, li_nRows,
				&lT1_data[0],
				li_nCols, li_nRows,
				getGDALDataType<Type>(), 0, 0 );
	  if (le_status > 0)
	    return 1;

	  for (int iRow = 0; iRow < li_nRows; iRow++)
	    for (int iCol = 0; iCol < li_nCols; iCol++)
	      aCr_grid()[iBand-1][iRow][iCol] = lT1_data[iRow*li_nCols+iCol];

	  double ld_no_data = lCp_band->GetNoDataValue();

	  if ( ( (ld_no_data > (double)std::numeric_limits<Type>::min() &&
		  std::numeric_limits<Type>::is_integer) ||
		 (ld_no_data < (double)std::numeric_limits<Type>::min() &&
		  !std::numeric_limits<Type>::is_integer) ) &&
		 ld_no_data < (double)std::numeric_limits<Type>::max() ) {
	    aCr_grid(iBand-1).no_data(ld_no_data);
	  } // if ( (ld_no_data...
	}   // if (lCp_band != ...
      }	    //  for (int iBand = 0;...

      return 0;

    } // readGDALDataset(GDALDataset&, Geogrid_impl<Type>&)

    /**
     * Utility template function that writes Geogrid data to a GDALDataset.
     *
     * @param aCr_dataset reference to GDALDataset
     * @param aCr_grid reference to geogrid
     * @returns exit status
     */
    template <class Type>
    int writeGDALDataset(GDALDataset& aCr_dataset,
			 const Geogrid_impl<Type>& aCr_grid) {

      // get grid dimensions
      int li_nBands = aCr_dataset.GetRasterCount();
      int li_nRows = aCr_dataset.GetRasterYSize();
      int li_nCols = aCr_dataset.GetRasterXSize();

      if (aCr_grid.num_bands() != li_nBands ||
	  aCr_grid.num_rows() != li_nRows ||
	  aCr_grid.num_cols() != li_nCols)
	return 1;

      // allocate data block for raster data
      std::vector<Type> lT1_data(li_nRows*li_nCols);

      for (int iBand = 1; iBand <= li_nBands; iBand++) {
	GDALRasterBand* lCp_band = aCr_dataset.GetRasterBand(iBand);
	if (lCp_band != 0) {

	  for (int iRow = 0; iRow < li_nRows; iRow++)
	    for (int iCol = 0; iCol < li_nCols; iCol++)
	      lT1_data[iRow*li_nCols+iCol] = aCr_grid()[iBand-1][iRow][iCol];

	  CPLErr le_status =
	    lCp_band->RasterIO( GF_Write, 0, 0, li_nCols, li_nRows,
				&lT1_data[0],
				li_nCols, li_nRows,
				getGDALDataType<Type>(), 0, 0 );

	  if (le_status > 0)
	    return 1;

	  double ld_no_data = aCr_grid(iBand-1).no_data();
	  if ( ( (ld_no_data > (double)std::numeric_limits<Type>::min() &&
		  std::numeric_limits<Type>::is_integer) ||
		 (ld_no_data < (double)std::numeric_limits<Type>::min() &&
		  !std::numeric_limits<Type>::is_integer) ) &&
		 ld_no_data < (double)std::numeric_limits<Type>::max() ) {
	    lCp_band->SetNoDataValue( aCr_grid(iBand-1).no_data() );
	  }
	}
	else
	  return 1;
      }

      aCr_dataset.FlushCache();

      return 0;

    } // writeGDALDataset(GDALDataset&, Geogrid_impl<Type>&)

  } // namespace gis
}   // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GDAL_UTILS_IPP
