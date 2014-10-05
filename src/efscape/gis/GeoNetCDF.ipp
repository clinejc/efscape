// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GeoNetCDF.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEONETCDF_IPP
#define EFSCAPE_GIS_GEONETCDF_IPP

#include <efscape/gis/GeoNetCDF.hh>

namespace efscape {

  namespace gis {

    /**
     * Reads a GeoNetCDF dataset as a Geogrid.
     *
     * @tparm Type value type
     * @param aCr_grid handle to grid
     * @param al_index record index
     * @returns exit status
     * @throws std::out_of_range
     */
    template <class Type>
    int GeoNetCDF::read(Geogrid_impl<Type>& aCr_grid, long al_index) const
      throw (std::out_of_range)
    {
      if (!this->isa_timeseries() && al_index != 0) {
	std::ostringstream lC_error_msg;
	lC_error_msg << "invalid index <" << al_index << "> for single layer";
	throw (std::out_of_range(lC_error_msg.str()));
      }

      long ll_max_index = this->num_recs()-1;
      if (ll_max_index < 0)
	ll_max_index = 0;

      if (al_index < 0 || al_index > ll_max_index) {
	std::ostringstream lC_error_msg;
	lC_error_msg << "index <" << al_index << "> is out of the valid range "
		     << "[0-" << ll_max_index << "]";
	throw (std::out_of_range(lC_error_msg.str()));
      }

      if (!this->is_georeferenced(aCr_grid.name()))
	return 1;

      NcVar* lCp_var = this->dataset()->get_var(aCr_grid.name());
      if (lCp_var == 0)
	return 1;

      // get grid dimensions
      int li_nBands = this->num_bands();
      int li_nRows = this->num_rows();
      int li_nCols = this->num_cols();

      // set geogrid dimensions if not already set
      if (aCr_grid.num_bands() != li_nBands ||
	  aCr_grid.num_rows() != li_nRows ||
	  aCr_grid.num_cols() != li_nCols)

	aCr_grid.resize( li_nCols,
			 li_nRows,
			 li_nBands,
			 this->envelope(),
			 this->spatial_ref() );

      if (this->num_recs() == 0)
	return 1;		// still in define mode

      if (!this->isa_timeseries()) {

	// allocate data block for raster data
	std::vector<Type> lT1_data(li_nBands*li_nRows*li_nCols);

	// get data
	if (li_nBands == 1)
	  lCp_var->get(&lT1_data[0],
		       aCr_grid.num_rows(),
		       aCr_grid.num_cols());
	else
	  lCp_var->get(&lT1_data[0],
		       aCr_grid.num_bands(),
		       aCr_grid.num_rows(),
		       aCr_grid.num_cols());

	// copy data to geogrid
	for (int iBand = 0; iBand < li_nBands; iBand++)
	  for (int iRow = 0; iRow < li_nRows; iRow++)
	    for (int iCol = 0; iCol < li_nCols; iCol++)
	      aCr_grid()[iBand][iRow][iCol] =
		lT1_data[li_nCols*(iBand*li_nRows + iRow) + iCol];

      }
      else {			// this is a time series

	NcValues* lCp_values = lCp_var->get_rec(al_index);

	switch (lCp_var->type()) {
	case ncByte:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_ncbyte(li_nCols*(iBand*li_nRows + iRow) + iCol);
	  break;
	case ncChar:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_char(li_nCols*(iBand*li_nRows + iRow) + iCol);
	  break;
	case ncShort:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_short(li_nCols*(iBand*li_nRows + iRow) + iCol);
	  break;
	case ncInt:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_int(li_nCols*(iBand*li_nRows + iRow) + iCol);
	  break;
	case ncFloat:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_float(li_nCols*(iBand*li_nRows + iRow) + iCol);
	  break;
	case ncDouble:
	default:
	  for (int iBand = 0; iBand < li_nBands; iBand++)
	    for (int iRow = 0; iRow < li_nRows; iRow++)
	      for (int iCol = 0; iCol < li_nCols; iCol++)
		aCr_grid()[iBand][iRow][iCol] =
		  lCp_values->as_double(li_nCols*(iBand*li_nRows + iRow) + iCol);
	}

	delete lCp_values;	// delete data block
      }

      return 0;

    } // GeoNetCDF::read<>(Geogrid_impl<Type>&,long) const

    /**
     * Writes Geogrid data to a GeoNetCDF dataset.
     *
     * @tparm Type value type
     * @param aCr_grid handle to grid
     * @param al_index record index
     * @returns exit status
     * @throws std::out_of_range
     */
    template <class Type>
    int GeoNetCDF::write(const Geogrid_impl<Type>& aCr_grid, long al_index)
      throw (std::out_of_range)
    {
      if (!this->isa_timeseries() && al_index != 0) {
	std::ostringstream lC_error_msg;
	lC_error_msg << "invalid index <" << al_index << "> for single layer";
	throw (std::out_of_range(lC_error_msg.str()));
      }

      if (al_index < 0 || al_index > this->num_recs()) {
	long ll_max_index = this->num_recs();
	std::ostringstream lC_error_msg;
	lC_error_msg << "index <" << al_index << "> is out of the valid range "
		     << "[0-" << ll_max_index << "]";
	throw (std::out_of_range(lC_error_msg.str()));
      }

      if (!this->is_georeferenced(aCr_grid.name()))
	return 1;

      NcVar* lCp_var = this->dataset()->get_var(aCr_grid.name());
      if (lCp_var == 0)
	return 1;

      if ( this->isa_timeseries() ) {
	  NcDim* lCp_time_dim = this->dataset()->get_dim("time");
	  if (al_index < lCp_time_dim->size()) {
	    double ld_time = this->delta_t()*(double)al_index;
	    this->dataset()->get_var("time")->put_rec(&ld_time, al_index);
	  }
      }

      // get grid dimensions
      int li_nBands = this->num_bands();
      int li_nRows = this->num_rows();
      int li_nCols = this->num_cols();

      if (aCr_grid.num_bands() != li_nBands ||
	  aCr_grid.num_rows() != li_nRows ||
	  aCr_grid.num_cols() != li_nCols)
	return 1;

      // allocate data block for raster data
      std::vector<Type> lT1_data(li_nBands*li_nRows*li_nCols);

      for (int iBand = 0; iBand < li_nBands; iBand++) {
	for (int iRow = 0; iRow < li_nRows; iRow++) {
	  for (int iCol = 0; iCol < li_nCols; iCol++)
	    lT1_data[li_nCols*(iBand*li_nRows + iRow) + iCol] =
	      aCr_grid()[iBand][iRow][iCol];
	}
      }
      
      if (!this->isa_timeseries()) {
	if (li_nBands == 1)
	  lCp_var->put(&lT1_data[0],
		       aCr_grid.num_rows(),
		       aCr_grid.num_cols());
	else
	  lCp_var->put(&lT1_data[0],
		       aCr_grid.num_bands(),
		       aCr_grid.num_rows(),
		       aCr_grid.num_cols());
      }
      else
	lCp_var->put_rec(&lT1_data[0], al_index);

      return 0;

    } // GeoNetCDF.write<>(const Geogrid_impl<Type>&,long)

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GEONETCDF_IPP
