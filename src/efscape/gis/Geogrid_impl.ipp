// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Geogrid_impl.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEOGRIDIMPL_IPP
#define EFSCAPE_GIS_GEOGRIDIMPL_IPP

#include <efscape/gis/Geogrid_impl.hh>

#include <efscape/gis/CellIndex.hh>
#include <efscape/gis/gdal_utils.hpp>
#include <algorithm>

namespace efscape {

  namespace gis {

    //------------------------------------------
    // beginning of Geogrid_impl<Type> implementation
    //------------------------------------------
    /** default destructor */
    template < class Type >
    Geogrid_impl < Type >::Geogrid_impl() :
      Geogrid()
    {
      resize(1,1,1,
	     geos::geom::Envelope(0.,1.,0.,1.),
	     "");
    }

    /**
     * Constructs a Geogrid_impl<Type> object.
     *
     * @param acp_name file name of grid
     */
    template < class Type >
    Geogrid_impl < Type >::Geogrid_impl(const char* acp_name) :
      Geogrid(acp_name)
    {
      resize(1,1,1,
	     geos::geom::Envelope(0.,1.,0.,1.),
	     "");
    }

    /**
     * Constructs a Geogrid_impl<Type> object for a new grid dataset.
     *
     * @param acp_name grid name
     * @param ai_nCols number of columns
     * @param ai_nRows number of rows
     * @param ai_nBands number of bands
     * @param aCr_envelope spatial extent
     * @param acp_spatial_ref spatial reference as WKT
     */
    template < class Type >
    Geogrid_impl < Type >::Geogrid_impl(const char* acp_name, 
					int ai_nCols,
					int ai_nRows,
					int ai_nBands,
					const geos::geom::Envelope& aCr_envelope,
					const char* acp_spatial_ref) :
      Geogrid(acp_name)
    {
      resize(ai_nCols,
	     ai_nRows,
	     ai_nBands,
	     aCr_envelope,
	     acp_spatial_ref);
    }

    /**
     * Copy constructor.
     *
     * @param aCr_grid source grid
     */
    template < class Type >
    Geogrid_impl < Type >::Geogrid_impl(const Geogrid_impl<Type>& aCr_grid)
    {
      name(aCr_grid.name());
      resize(aCr_grid.num_cols(),
	     aCr_grid.num_rows(),
	     aCr_grid.num_bands(),
	     aCr_grid.envelope(),
	     aCr_grid.spatial_ref());

      // copy grid data
      for (int iBand = 0; iBand < (int)num_bands(); iBand++) {
	mC1_band_names[iBand] = // copy band names
	  aCr_grid.mC1_band_names[iBand];
	for (int iRow = 0; iRow < (int)num_rows(); iRow++)
	  for (int iCol = 0; iCol < (int)num_cols(); iCol++)
	    (*this)()[iBand][iRow][iCol] = aCr_grid()[iBand][iRow][iCol];
      }
    }


    /** destructor */
    template < class Type >
    Geogrid_impl < Type >::~Geogrid_impl() {}

    /**
     * Returns a clone of this geogrid.
     *
     * @returns handle to geogrid copy
     */
    template < class Type >
    Geogrid* Geogrid_impl < Type >::clone() const {
      return new Geogrid_impl< Type >(*this);
    }

    /**
     * Builds the grid.
     *
     * @param ai_nCols number of columns
     * @param ai_nRows number of rows
     * @param ai_nBands number of bands
     * @param aCr_envelope spatial extent
     * @param acp_spatial_ref spatial reference as WKT
     */
    template < class Type >
    void Geogrid_impl < Type >::resize(int ai_nCols,
				       int ai_nRows,
				       int ai_nBands,
				       const geos::geom::Envelope& aCr_envelope,
				       const char* acp_spatial_ref)
    {
      // resize data array
      if (mT3p_data.get() == 0)
	mT3p_data.reset(new array_type);
      mT3p_data->resize(boost::extents[ai_nBands][ai_nRows][ai_nCols]);
      mC1_band_names.resize(ai_nBands); // resize band names vector

      // set spatial info
      mC_envelope = aCr_envelope;
      mC_spatial_ref = acp_spatial_ref;

      for (int iBand = 0; iBand < ai_nBands; iBand++) {
	boost::shared_ptr< Band > lCp_band( new Band(iBand, this) );
	mCp1_bands.push_back(lCp_band);
      }
    }

    /**
     * Builds the grid.
     *
     * @param aCr_grid 
     */
    template < class Type >
    void Geogrid_impl < Type >::resize(const Geogrid& aCr_grid)
    {
      resize(aCr_grid.num_cols(),
	     aCr_grid.num_rows(),
	     aCr_grid.num_bands(),
	     aCr_grid.envelope(),
	     aCr_grid.spatial_ref());
    }

    /**
     * Returns the envelope of the grid
     *
     * @returns envelope of the grid
     */
    template <class Type>
    geos::geom::Envelope Geogrid_impl<Type>::envelope() const {
      return mC_envelope;
    }

    /**
     * Returns the resolution of the grid
     *
     * @returns resolution of the grid
     */
    template <class Type>
    geos::geom::Coordinate Geogrid_impl<Type>::resolution() const {
      double ld_width = mC_envelope.getMaxX()-mC_envelope.getMinX();
      double ld_height = mC_envelope.getMaxY()-mC_envelope.getMinY();

      return (geos::geom::Coordinate(ld_width/num_cols(),
				     ld_height/num_rows()) );
    }

    /**
     * Returns the spatial reference of this grid as WKT.
     *
     * @returns spatial reference
     */
    template <class Type>
    const char* Geogrid_impl<Type>::spatial_ref() const {
      return mC_spatial_ref.c_str();
    }

    /**
     * Returns the number of columns or "X" size
     *
     * @returns number of columns
     */
    template <class Type>
    size_t Geogrid_impl<Type>::num_cols() const {
      return mT3p_data->shape()[2];
    }

    /**
     * Returns the number of rows or "Y" size
     *
     * @returns number of rows
     */
    template <class Type>
    size_t Geogrid_impl<Type>::num_rows() const {
      return mT3p_data->shape()[1];
    }

    /**
     * Returns the number of bands.
     *
     * @returns number of bands
     */
    template <class Type>
    unsigned int Geogrid_impl<Type>::num_bands() const {
      return mT3p_data->shape()[0];
    }

    /**
     * Returns the name of band <ai_band>
     *
     * @param ai_band band index
     * @returns band name
     * @throws std::out_of_range
     */
    template <class Type>
    std::string Geogrid_impl<Type>::band_name(int ai_band) const
      throw(std::out_of_range)
    {
      return mC1_band_names[ai_band];
    }

    /**
     * Sets the name of band<ai_band>
     *
     * @param acp_name band name
     */
    template <class Type>
    void Geogrid_impl<Type>::band_name(int ai_band, const char* acp_name)
      throw(std::out_of_range)
    {
      mC1_band_names[ai_band] = acp_name;
    }

    /**
     * @returns size of grid band
     */
    template <class Type>
    size_t Geogrid_impl<Type>::size() const {
      return (num_rows()*num_cols());
    }

    //------------
    // data access
    //------------
    /**
     * Returns data item at (band,row,col).
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type& Geogrid_impl<Type>::operator()(unsigned int aui_band,
					 size_t ai_row, size_t ai_col)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col).
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type Geogrid_impl<Type>::operator()(unsigned int aui_band,
					size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (0,row,col).
     *
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type& Geogrid_impl<Type>::operator()(size_t ai_row, size_t ai_col)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (*this)()[0][ai_row][ai_col] );
    }

    /**
     * Returns data item at (0,row,col).
     *
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type Geogrid_impl<Type>::operator()(size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (*this)()[0][ai_row][ai_col] );
    }

    /**
     * Returns data item at the specified index.
     *
     * @param aCr_index index
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type& Geogrid_impl<Type>::operator()(const Index& aCr_index)
      throw(std::out_of_range)
    {
      unsigned int li_band = aCr_index.band();
      size_t li_row = aCr_index.row();
      size_t li_col = aCr_index.col();

      (*mCp_index)(this,li_row,li_col);
      return ( (*this)()[li_band][li_row][li_col] );
    }

    /**
     * Returns data item at the specified index.
     *
     * @param aCr_index index
     * @returns data item
     * @throws std::out_of_range
     */
    template <class Type>
    Type Geogrid_impl<Type>::operator()(const Index& aCr_index) const
      throw(std::out_of_range)
    {
      unsigned int li_band = aCr_index.band();
      size_t li_row = aCr_index.row();
      size_t li_col = aCr_index.col();

      (*mCp_index)(this,li_row,li_col);
      return ( (*this)()[li_band][li_row][li_col] );
    }

    /**
     * Returns data item at (band,row,col) as a bool.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a boolean
     * @throws std::out_of_range
     */
    template <class Type>
    bool Geogrid_impl<Type>::as_bool(unsigned int aui_band,
				     size_t ai_row,  size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (bool)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a byte (unsigned char).
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a byte
     * @throws std::out_of_range
     */
    template <class Type>
    unsigned char Geogrid_impl<Type>::as_byte(unsigned int aui_band,
					      size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (unsigned char)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a char.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a char
     * @throws std::out_of_range
     */
    template <class Type>
    char Geogrid_impl<Type>::as_char(unsigned int aui_band,
				     size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (char)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a short.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a short
     * @throws std::out_of_range
     */
    template <class Type>
    short Geogrid_impl<Type>::as_short(unsigned int aui_band,
				       size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (short)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a long.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a long
     * @throws std::out_of_range
     */
    template <class Type>
    long Geogrid_impl<Type>::as_long(unsigned int aui_band,
				     size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (long)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a float.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a float
     * @throws std::out_of_range
     */
    template <class Type>
    float Geogrid_impl<Type>::as_float(unsigned int aui_band,
				       size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (float)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Returns data item at (band,row,col) as a double.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item as a double
     * @throws std::out_of_range
     */
    template <class Type>
    double Geogrid_impl<Type>::as_double(unsigned int aui_band,
					 size_t ai_row, size_t ai_col) const
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      return ( (double)(*this)()[aui_band][ai_row][ai_col] );
    }

    /**
     * Sets the data item at (band,row,col) as a bool.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param ab_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 bool ab_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)ab_val;
    }

    /**
     * Sets the data item at (band,row,col) as a byte.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param ac_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 unsigned char auc_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)auc_val;
    }

    /**
     * Sets the data item at (band,row,col) as a char.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param ac_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 char ac_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)ac_val;
    }

    /**
     * Sets the data item at (band,row,col) as a short.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param as_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 short as_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)as_val;
    }

    /**
     * Sets the data item at (band,row,col) as a long.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param al_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 long al_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)al_val;
    }

    /**
     * Sets the data item at (band,row,col) as a float.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param af_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 float af_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)af_val;
    }

    /**
     * Sets the data item at (band,row,col) as a double.
     *
     * @param aui_band grid band
     * @param ai_row grid row
     * @param ai_col grid col
     * @param ad_val new value
     * @throws std::out_of_range
     */
    template <class Type>
    void Geogrid_impl<Type>::set(unsigned int aui_band,
				 size_t ai_row, size_t ai_col,
				 double ad_val)
      throw(std::out_of_range)
    {
      (*mCp_index)(this,ai_row,ai_col);
      (*this)()[aui_band][ai_row][ai_col] = (Type)ad_val;
    }

    //-----------------------------------------
    // end of Geogrid_impl<Type> implementation
    //-----------------------------------------

    //-------------------------------------------------
    // start of Geogrid_impl<Type>::Band implementation
    //-------------------------------------------------
    /**
     * constructor
     *
     * @param ai_band band index
     * @param aCp_geogrid handle to associated grid
     * @param acp_name band name (default: none)
     */
    template < class Type >
    Geogrid_impl < Type >::Band::Band(int ai_band,
				      Geogrid_impl<Type>* aCp_geogrid) :
      mi_band(ai_band),
      mCp_geogrid(aCp_geogrid),
      md_no_data(DBL_MAX)
    {
    }

    /** @returns band index */
    template < class Type >
    int Geogrid_impl < Type >::Band::band() const { return mi_band; }

    /** @returns number of columns */
    template < class Type >
    size_t Geogrid_impl < Type >::Band::num_cols() const {
      return mCp_geogrid->num_cols();
    }

    /** @returns number of rows */
    template < class Type >
    size_t Geogrid_impl < Type >::Band::num_rows() const {
      return mCp_geogrid->num_rows();
    }

    /** @returns band size */
    template < class Type >
    size_t Geogrid_impl < Type >::Band::size() const {
      return num_rows()*num_cols();
    }

    /**
     * Returns reference to data item at (row,col).
     *
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template < class Type >
    Type& Geogrid_impl < Type >::Band::operator()(size_t ai_row,
						  size_t ai_col)
      throw (std::out_of_range)
    {
      return (*mCp_geogrid)(mi_band,ai_row,ai_col);
    }

    /**
     * Returns data item at (row,col).
     *
     * @param ai_row grid row
     * @param ai_col grid col
     * @returns data item
     * @throws std::out_of_range
     */
    template < class Type >
    const Type Geogrid_impl < Type >::Band::operator()(size_t ai_row,
						       size_t ai_col) const
      throw (std::out_of_range)
    {
      return (*mCp_geogrid)(mi_band,ai_row,ai_col);
    }

    /**
     * Returns the minimum band value (excluding missing values).
     *
     * @returns minimum value
     */
    template < class Type >
    Type Geogrid_impl < Type >::Band::min() const {
      Type lT_min = (Type)DBL_MIN;
      for (int li_row = 0; li_row < num_rows(); li_row++)
	for (int li_col = 0; li_col < num_cols(); li_col++)
	  lT_min = std::min( lT_min, (*this)(li_row,li_col) );
      return lT_min;
    }

    /**
     * Returns the maximum band value (excluding missing values).
     *
     * @returns maximum value
     */
    template < class Type >
    Type Geogrid_impl < Type >::Band::max() const {
      Type lT_max = (Type)-DBL_MAX;
      for (int li_row = 0; li_row < num_rows(); li_row++)
	for (int li_col = 0; li_col < num_cols(); li_col++)
	  lT_max = std::max( lT_max, (*this)(li_row,li_col) );
      return lT_max;
    }

    /**
     * Returns the missing data value for this band (DBL_MAX if not set).
     *
     * @returns missing data value
     */
    template < class Type >
    double Geogrid_impl < Type >::Band::no_data() const {
      return md_no_data;
    }

    /**
     * Sets the missing data value for this band.
     *
     * @param ad_no_data new missing data value
     */
    template < class Type >
    void Geogrid_impl < Type >::Band::no_data(double ad_no_data) {
      md_no_data = ad_no_data;
    }

    /**
     * Fills the entire band data block with value <aT_value>
     *
     * @param aT_value fill value
     */
    template <class Type>
    void Geogrid_impl<Type>::Band::fill(Type aT_value) {
      std::fill((*mCp_geogrid->mT3p_data)[mi_band].origin(),
		(*mCp_geogrid->mT3p_data)[mi_band].origin()
		+(*mCp_geogrid->mT3p_data)[mi_band].num_elements(),
		aT_value);
    }

    /**
     * Returns the vector of raster band category names.
     *
     * @returns vector of category names
     */
    template < class Type >
    std::vector<std::string>
    Geogrid_impl < Type >::Band::getCategoryNames() {
      if (mCp_geogrid->dataset() == NULL)
	return std::vector<std::string>();

      return ( getStdStrings(mCp_geogrid->dataset()->
			     GetRasterBand(mi_band+1)->GetCategoryNames() ) );
    }

    /**
     * Sets the vector of raster band category names.
     *
     * @param aC1_names vector of category names
     */
    template < class Type >
    void
    Geogrid_impl < Type >::Band::setCategoryNames(const
						  std::vector<std::string>&
						  aC1_names)
    {
      if (mCp_geogrid->dataset() == NULL)
	return;

      mCp_geogrid->dataset()->
	GetRasterBand(mi_band+1)->SetCategoryNames( getCSLStrings(aC1_names) );
    }

    /**
     * Returns a handle to a copy of the color table.
     *
     * @returns a handle to a copy of the color table
     */
    template < class Type >
    GDALColorTable*
    Geogrid_impl < Type >::Band::getColorTable() {
      if (mCp_geogrid->dataset() == NULL)
	return 0;

      GDALColorTable* lCp_ColorTable =
	mCp_geogrid->dataset()->GetRasterBand(mi_band+1)->GetColorTable();

      if ( lCp_ColorTable != NULL )
	return lCp_ColorTable->Clone();

      return lCp_ColorTable;
    }

    /**
     * Sets the color table.
     *
     * @param aCp_Color handle to the color table
     */
    template < class Type >
    void
    Geogrid_impl < Type >::Band::setColorTable(GDALColorTable* aCp_ColorTable) {
      if (mCp_geogrid->dataset() == NULL)
	return;

      mCp_geogrid->dataset()->GetRasterBand(mi_band+1)
	->SetColorTable(aCp_ColorTable);
    }

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GEOGRIDIMPL_IPP
