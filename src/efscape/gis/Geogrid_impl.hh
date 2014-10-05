// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Geogrid_impl.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEOGRIDIMPL_HH
#define EFSCAPE_GIS_GEOGRIDIMPL_HH

#include <efscape/gis/Geogrid.hh>

#include <gdal_priv.h>		// Geospatial Data Abstraction Library
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <cfloat>

namespace efscape {

  namespace gis {

    template < class Type > class Geogrid_impl;

    /**
     * A base interface for accessing georeferenced grid data of type <Type>
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.54 created 18 Feb 2006, revised 24 May 2010
     */
    template < class Type >
    class Geogrid_impl : public Geogrid
    {

    public:

      typedef boost::multi_array<Type,3> array_type;

      /**
       * Encapsulates raster band data.
       *
       * @version 0.01 created 05 Apr 2009
       */
      class Band
      {
	friend class Geogrid_impl<Type>;

      public:

	int band() const;
	size_t num_cols() const;
	size_t num_rows() const;
	size_t size() const;
	Type min() const;
	Type max() const;
	double no_data() const;
	void no_data(double ad_no_data);

	Type& operator()(size_t ai_row, size_t ai_col)
	  throw(std::out_of_range);
	const Type operator()(size_t ai_row, size_t ai_col) const
	  throw(std::out_of_range);

	void fill(Type aT_value);

	std::vector<std::string> getCategoryNames();
	void setCategoryNames(const std::vector<std::string>& aC1_names);

	GDALColorTable* getColorTable();
	void setColorTable(GDALColorTable* aCp_ColorTable);

      protected:

	Band(int ai_band, Geogrid_impl<Type>* aCp_geogrid);

      private:

	Band();

	/** band index */
	int mi_band;

	/** handle to associated Geogrid */
	Geogrid_impl<Type>* mCp_geogrid;

	/** missing data value (DBL_MAX if not set) */
	double md_no_data;

      };			// class Band

      //
      // constructor/destructor
      //
      Geogrid_impl();
      Geogrid_impl(const char* acp_name);

      Geogrid_impl(const char* acp_name,
		   int ai_nCols,
		   int ai_nRows,
		   int ai_nBands,
		   const geos::geom::Envelope& aCr_envelope,
		   const char* acp_projection);
      Geogrid_impl(const Geogrid_impl<Type>& aCr_grid);
      ~Geogrid_impl();

      Geogrid* clone() const;

      void resize(int ai_nCols,
		  int ai_nRows,
		  int ai_nBands,
		  const geos::geom::Envelope& aCr_envelope,
		  const char* acp_projection);

      void resize(const Geogrid& aCr_grid);
      geos::geom::Envelope envelope() const;
      geos::geom::Coordinate resolution() const;
      const char* spatial_ref() const;
      size_t num_cols() const;
      size_t num_rows() const;
      unsigned int num_bands() const;
      std::string band_name(int ai_band) const
	throw(std::out_of_range);
      void band_name(int ai_band, const char* acp_name)
	throw(std::out_of_range);
      size_t size() const;

      //------------
      // data access
      //------------
      /** @returns reference to data array */
      array_type& operator()() { return *mT3p_data; }

      /** @returns const reference to data array */
      const array_type& operator()() const { return *mT3p_data; }

      /**
       * Returns reference to band <ai_band>
       *
       * @param ai_band band index
       * @returns reference to band <ai_band>
       * @throws(std::out_of_range)
       */
      Band& operator()(size_t ai_band)
	throw(std::out_of_range)
      {
	return *(mCp1_bands[ai_band]);
      }

      /**
       * Returns const reference to band <ai_band>
       *
       * @param ai_band band index
       * @returns const reference to band <ai_band>
       * @throws(std::out_of_range)
       */
      const Band& operator()(size_t ai_band) const
	throw(std::out_of_range)
      {
	return *(mCp1_bands[ai_band]);
      }

      Type& operator()(unsigned int aui_band,
		       size_t ai_row, size_t ai_col)
	throw(std::out_of_range);
      Type operator()(unsigned int aui_band,
		      size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      Type& operator()(size_t ai_row, size_t ai_col)
	throw(std::out_of_range);
      Type operator()(size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      Type& operator()(const Index& aCr_index)
	throw(std::out_of_range);
      Type operator()(const Index& aCr_index) const
	throw(std::out_of_range);

      bool as_bool(unsigned int aui_band,
		   size_t ai_row,  size_t ai_col) const
	throw(std::out_of_range);
      unsigned char as_byte(unsigned int aui_band,
			    size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      char as_char(unsigned int aui_band,
		   size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      short as_short(unsigned int aui_band,
		     size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      long as_long(unsigned int aui_band,
		   size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      float as_float(unsigned int aui_band,
		     size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);
      double as_double(unsigned int aui_band,
		       size_t ai_row, size_t ai_col) const
	throw(std::out_of_range);

      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       bool ab_val)
	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       unsigned char auc_val)
      	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       char ac_val)
      	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       short as_val)
	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       long al_val)
	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       float af_val)
	throw(std::out_of_range);
      void set(unsigned int aui_band,
	       size_t ai_row, size_t ai_col,
	       double ad_val)
	throw(std::out_of_range);

    protected:

      /** grid data */
      boost::scoped_ptr<array_type> mT3p_data;

      /** grid extent */
      geos::geom::Envelope mC_envelope;

      /** spatial reference as WKT */
      std::string mC_spatial_ref;

      /** band names */
      std::vector<std::string> mC1_band_names;

      /** vector of raster bands */
      std::vector< boost::shared_ptr< Band > > mCp1_bands;

    };				// class Geogrid_impl< Type >

    // template instantiation
    typedef Geogrid_impl<GByte> ByteGeogrid;
    typedef Geogrid_impl<char> CharGeogrid;
    typedef Geogrid_impl<short> ShortGeogrid;
    typedef Geogrid_impl<int> IntGeogrid;
    typedef Geogrid_impl<long> LongGeogrid;
    typedef Geogrid_impl<float> FloatGeogrid;
    typedef Geogrid_impl<double> DoubleGeogrid;

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GEOGRIDIMPL_HH
