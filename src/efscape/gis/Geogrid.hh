// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Geogrid.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEOGRID_HH
#define EFSCAPE_GIS_GEOGRID_HH

#include <string>
#include <stdexcept>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <efscape/gis/Index.hh>
#include <efscape/gis/Range.hh>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// forward declarations
class GDALDataset;

namespace efscape {

  namespace gis {

    // forward declarations
    class CellIndex;
    class GeoNetCDF;

    typedef boost::shared_ptr<GDALDataset> GDALDatasetPtr;

    /**
     * A base interface for accessing georeferenced grid data.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.1.1 created 23 Feb 2006, revised 15 Sep 2009
     */
    class Geogrid
    {
    public:
      //
      // constructor/destructor
      //
      Geogrid();
      Geogrid(const char* acp_name);

      virtual ~Geogrid();

      virtual Geogrid* clone() const { return 0; }

      //
      // methods
      //

      virtual void resize(int ai_nCols,
			  int ai_nRows,
			  int ai_nBands,
			  const geos::geom::Envelope& aCr_envelope,
			  const char* acp_projection) {}
      virtual void resize(const Geogrid& aCr_grid) {}

      /** @returns grid name */
      const char* name() const { return mC_name.c_str(); }

      /**
       * Sets the grid name.
       *
       * @param acp_name new grid name
       */
      void name(const char* acp_name) {
	mC_name = acp_name;
      }

      virtual geos::geom::Envelope envelope() const { return geos::geom::Envelope(); }
      virtual geos::geom::Coordinate resolution() const { return geos::geom::Coordinate(); }
      virtual const char* spatial_ref() const { return ""; }

      virtual size_t num_cols() const { return 0; }
      virtual size_t num_rows() const { return 0; }
      virtual unsigned int num_bands() const { return 0; }
      
      virtual size_t size() const { return 0; }
      void cell_index(CellIndex* aCp_index);

      //------------
      // data access
      //------------
      virtual bool as_bool(unsigned int aui_band,
			  size_t ai_row, size_t ai_col) const
      	throw(std::out_of_range) { return 0; }
      virtual unsigned char as_byte(unsigned int aui_band,
				    size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0; }
      virtual char as_char(unsigned int aui_band,
			  size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0; }
      virtual short as_short(unsigned int aui_band,
			    size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0; }
      virtual long as_long(unsigned int aui_band,
			  size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0; }
      virtual float as_float(unsigned int aui_band,
			    size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0.; }
      virtual double as_double(unsigned int aui_band,
			      size_t ai_row, size_t ai_col) const
	throw(std::out_of_range) { return 0.; }

      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       bool ab_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       unsigned char auc_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       char ac_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       short as_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       long al_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       float af_val)
	throw(std::out_of_range) {}
      virtual void set(unsigned int aui_band,
		       size_t ai_row, size_t ai_col,
		       double ad_val)
	throw(std::out_of_range) {}

      bool contains(const geos::geom::Coordinate& aCr_coord) const;

      Index coord2index(const geos::geom::Coordinate& aCr_coord) const;
      Index coord2index(const geos::geom::Coordinate& aCr_coord,
			Index& aCr_index,
			size_t ai_NumRows, size_t ai_NumCols) const;

      geos::geom::Envelope envelope(size_t ai_row, size_t ai_col) const;
      geos::geom::Envelope envelope(const Index& aCr_index) const;
      geos::geom::Envelope envelope(const Range& aCr_range) const;

      Range range() const;
      bool intersection(Range& aCr_range) const;
      bool intersection(const geos::geom::Envelope& aCr_envelope,
			Range& aCr_result) const;

      bool isa_torus() const;

      const boost::shared_ptr<GDALDataset>& dataset() const;
      bool set_dataset(const boost::shared_ptr<GDALDataset>& aCp_dataset);

      int write();
      int write(const char* acp_name);
      int write(GeoNetCDF& aCr_GeoNetCDF, long al_index);

    protected:

      /** handle to CellIndex */
      boost::scoped_ptr<CellIndex> mCp_index;

    private:

      /** grid name */
      std::string mC_name;

      /** handle to GDALDataset */
      boost::shared_ptr<GDALDataset> mCp_dataset;

    };				// class Geogrid

    // typedef
    typedef boost::shared_ptr<Geogrid> GeogridPtr;

    // utility method for creating Geogrid based on data type stored in a string
    Geogrid* CreateGeogrid(const char * const acp_type);

  } // end of namespace gis

} // end of namespace efscape

#endif
