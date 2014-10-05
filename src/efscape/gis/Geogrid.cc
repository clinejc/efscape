// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Geogrid.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/Geogrid.hh>

#include <efscape/gis/Geogrid_impl.hh>
#include <efscape/gis/CellIndex.hh>
#include <efscape/gis/gdal_utils.hpp>
#include <efscape/gis/GeoNetCDF.hh>

#include <gdal_priv.h>		// Geospatial Data Abstraction Library

#include <iostream>

namespace efscape {

  namespace gis {

    // declaration of a helper function
    int MyTextProgress( double dfComplete, const char *pszMessage, void *pData);

    /** default constructor */
    Geogrid::Geogrid() :
      mC_name("geogrid")
    {
      //---------------
      // set grid index
      //---------------
      mCp_index.reset(new CellIndex);
    }

    /**
     * constructor
     *
     * @param acp_name grid name
     */
    Geogrid::Geogrid(const char* acp_name) :
      mC_name(acp_name)
    {
      //---------------
      // set grid index
      //---------------
      mCp_index.reset(new CellIndex);

    } // end of Geogrid::Geogrid(const char*)

    /** destructor */
    Geogrid::~Geogrid() {
    }

    /**
     * Sets the cell index of the grid.
     *
     * @param aCp_index handle to cell index
     */
    void Geogrid::cell_index(CellIndex* aCp_index) {
      if (aCp_index)
	mCp_index.reset(aCp_index);
    }

    /** returns a handle to the associated GDALDataset */
    const boost::shared_ptr<GDALDataset>& Geogrid::dataset() const {
      return mCp_dataset;
    }

    /**
     * Sets the smart handle to the dataset.
     *
     * @param aCp_dataset smart handle to a GDALdataset
     * @returns whether successful
     */
    bool Geogrid::set_dataset(const boost::shared_ptr<GDALDataset>&
			      aCp_dataset)
    {
      if (aCp_dataset.get() == NULL)
	return false;

      mCp_dataset = aCp_dataset;
      return true;
    }

    /**
     * Writes geogrid data to the associated GDALDataset.
     *
     * @returns exit status
     */
    int Geogrid::write() {
      int li_status = 1;

      if (mCp_dataset.get() == NULL)
	return li_status;

      return writeGDALDataset(*mCp_dataset, this);

    } // Geogrid::write()

    /**
     * Writes geogrid data to GDALDataset <acp_filename>.
     *
     * @param acp_filename filename
     * @returns exit status
     */
    int Geogrid::write(const char* acp_filename)
    {
      int li_status = 1;

      GDALDataset* lCp_dataset = NULL;

      // if not attached to a dataset, create a new one
      if (mCp_dataset.get() == NULL) {
	lCp_dataset = createGDALDataset(acp_filename,
					"GTIFF",
					this);
      }
      else {
	// otherwise, create a copy
	GDALDriver* lCp_driver = mCp_dataset->GetDriver();
	char** papszOption;
	lCp_dataset = lCp_driver->CreateCopy(acp_filename,
					     mCp_dataset.get(),
					     0, // strict copy = false
					     papszOption,
					     MyTextProgress,
					     (void *)0);
      }

      mCp_dataset.reset(lCp_dataset);

      if ( mCp_dataset.get() != NULL)
	li_status = 0;

      return li_status;

    } // Geogrid::write(const char*)

    /**
     * Writes geogrid data to a netCDF dataset and creates a new read-only
     * GDALDataset from the netCDF dataset. Does not support time series
     * datasets at this time.
     *
     * @param aCr_GeoNetCDF
     * @param al_index grid index
     * @returns exit status
     */
    int Geogrid::write(GeoNetCDF& aCr_GeoNetCDF, long al_index)
    {
      int li_status = 1;
      if ( (li_status = aCr_GeoNetCDF.write(this, al_index)) == 0) {
	std::string lC_FileName = "netcdf:" + std::string(aCr_GeoNetCDF.name())
	  + ":" + this->name();
	mCp_dataset.reset( (GDALDataset*)GDALOpen(lC_FileName.c_str(),
						  GA_ReadOnly) );
      }

      return li_status;
    }

    /**
     * Returns true if the coordinate is contained by this grid.
     *
     * @param aCr_coord coordinate
     * @returns true if coordinate is contained by this grid
     */
    bool Geogrid::contains(const geos::geom::Coordinate& aCr_coord) const
    {
      return(this->envelope().contains(aCr_coord) );
    }

    /**
     * Returns the data index of the specified coordinate
     *
     * @param aCr_coord coordinate
     * @return grid index
     */
    Index Geogrid::coord2index(const geos::geom::Coordinate& aCr_coord) const
    {
      geos::geom::Envelope lC_envelope = envelope();
      geos::geom::Coordinate lC_res = resolution();

      double ld_xdiff = aCr_coord.x - lC_envelope.getMinX();
      double ld_ydiff = lC_envelope.getMaxY() - aCr_coord.y;

      return ( Index( (long)(ld_ydiff/lC_res.y),
		      (long)(ld_xdiff/lC_res.x) ) );

    } // end of Geogrid::coord2index(const geos::geom::Coordinate& aCr_coord)

    /**
     * Returns the data index and relative sub index of the specified
     * coordinate within subsampled at the specified resolution
     * (NumRows X NumCols)
     *
     * @param aCr_coord coordinate
     * @param aCr_index grid index
     * @param ai_NumRows number of rows within the cell
     * @param ai_NumCols number of columns within the cell
     * @return grid cell sub index
     */
    Index
    Geogrid::coord2index(const geos::geom::Coordinate& aCr_coord,
			 Index& aCr_index,
			 size_t ai_NumRows, size_t ai_NumCols) const
    {
      aCr_index = coord2index(aCr_coord);
      geos::geom::Envelope lC_envelope = envelope(aCr_index);
      geos::geom::Coordinate lC_res = resolution();
      lC_res.x /= (double)ai_NumCols;
      lC_res.y /= (double)ai_NumRows;

      double ld_xdiff = aCr_coord.x - lC_envelope.getMinX();
      double ld_ydiff = lC_envelope.getMaxY() - aCr_coord.y;

      return ( Index( (long)(ld_ydiff/lC_res.y),
			       (long)(ld_xdiff/lC_res.x) ) );

    } // end of Geogrid::coord2index(const geos::geom::Coordinate& aCr_coord,...

    /**
     * Returns the envelope of the specified grid cell.
     *
     * @param ai_row row index
     * @param ai_col column index
     * @returns envelope of grid cell
     */
    geos::geom::Envelope
    Geogrid::envelope(size_t ai_row, size_t ai_col) const {
      geos::geom::Envelope lC_envelope;

      // translate coordinates
      (*mCp_index)(this,ai_row,ai_col);

      if ( /*ai_row < 0 ||*/ ai_row >= num_rows() ||
	   /*ai_col < 0 ||*/ ai_col >= num_cols() )
	lC_envelope.setToNull();

      else {
	geos::geom::Coordinate lC_res = resolution();
	double ld_maxY = ((double)ai_row)*lC_res.y;
	double ld_minY = ld_maxY - lC_res.y;
	double ld_minX = ((double)ai_col)*lC_res.x;
	double ld_maxX = ld_minX + lC_res.x;

	lC_envelope.init(ld_minX, ld_maxX,
			 ld_minY, ld_maxY);
      }

      return lC_envelope;
	
    } // end of Geogrid::envelope(size_t,size_t)

    /**
     * Returns the envelope of the specified grid cell.
     *
     * @param aCr_index grid index
     * @returns envelope of grid cell
     */
    geos::geom::Envelope
    Geogrid::envelope(const Index& aCr_index) const {
      return ( this->envelope(aCr_index.row(),aCr_index.col()) );
    }

    /**
     * Returns the envelope of the specified grid range
     *
     * @param aCr_range grid range
     * @returns envelope of grid range
     */
    geos::geom::Envelope
    Geogrid::envelope(const Range& aCr_range) const
    {
      double ld_OriginX = envelope().getMinX();
      double ld_OriginY = envelope().getMaxY();

      geos::geom::Coordinate lC_res = resolution();
      double ld_minX = ld_OriginX + ( aCr_range.min().col() * lC_res.x );
      double ld_maxY = ld_OriginY + ( aCr_range.min().row() * lC_res.y );
      double ld_maxX = ld_OriginX + ( aCr_range.max().col() * lC_res.x );
      double ld_minY = ld_OriginY + ( aCr_range.max().row() * lC_res.y );
      

      return ( geos::geom::Envelope(ld_minX, ld_maxX, ld_minY, ld_maxY) );

    } // end of Geogrid::envelope(const Range&)

    /**
     * Returns the grid range of the grid.
     *
     * @returns grid range of the grid
     */
    Range Geogrid::range() const
    {
      return (Range(Index(0,0),
		    Index(num_rows()-1,num_cols()-1)));
    }

    /**
     * Returns whether the grid range intersects this grid.
     *
     * @param aCr_range specified grid range (set to grid intersection)
     * @returns whether grid range intersects this grid
     */
    bool Geogrid::intersection(Range& aCr_range) const
    {
      if (isa_torus())
	return true;


      unsigned long lul_MaxCol = aCr_range.max().col();
      unsigned long lul_MaxRow = aCr_range.max().row();
      unsigned long lul_MinCol = aCr_range.min().col();
      unsigned long lul_MinRow = aCr_range.min().row();

      if (//lul_MaxCol < 0 ||
	  //lul_MaxRow < 0 ||
	  lul_MinCol >= num_cols() ||
	  lul_MinRow >= num_rows())
	return false;

      aCr_range.min().col() = lul_MinCol > 0 ? lul_MinCol : 0;
      aCr_range.min().row() = lul_MinRow > 0 ? lul_MinRow : 0;
      aCr_range.max().col() =
	lul_MaxCol < num_cols() ? lul_MaxCol : num_cols()-1;
      aCr_range.max().row() =
	lul_MaxRow < num_rows() ? lul_MaxRow : num_rows()-1;

      return true;

    } // end of Geogrid::intersection(Range&)


    /**
     * Returns the grid range intersection of the specified range
     *
     * @param aCr_envelope envelope
     * @param aCr_result grid range of intersection
     * @returns true if envelope intersects grid
     */
    bool Geogrid::intersection(const geos::geom::Envelope& aCr_envelope,
			       Range& aCr_result) const
    {
      //
      // 1. compute envelope intersection
      //   a) either intersection with grid envelope or
      //   b) intersection with infinate plane (torus), i.e. entire envelope
      //
      geos::geom::Envelope lC_intersection; // envelope intersection
      if (! isa_torus()) {
	geos::geom::Envelope lC_ThisEnvelope = envelope();

	if ( lC_ThisEnvelope.isNull() ||
	     aCr_envelope.isNull() ||
	     ! lC_ThisEnvelope.intersects(&aCr_envelope) )
	  return false;

	double ld_IntMinX =
	  lC_ThisEnvelope.getMinX() > aCr_envelope.getMinX() ?
	  lC_ThisEnvelope.getMinX() : aCr_envelope.getMinX();
	double ld_IntMinY =
	  lC_ThisEnvelope.getMinY() > aCr_envelope.getMinY() ?
	  lC_ThisEnvelope.getMinY() : aCr_envelope.getMinY();
	double ld_IntMaxX =
	  lC_ThisEnvelope.getMaxX() > aCr_envelope.getMaxX() ?
	  lC_ThisEnvelope.getMaxX() : aCr_envelope.getMaxX();
	double ld_IntMaxY =
	  lC_ThisEnvelope.getMaxY() > aCr_envelope.getMaxY() ?
	  lC_ThisEnvelope.getMaxY() : aCr_envelope.getMaxY();

	lC_intersection.init(ld_IntMinX, ld_IntMaxX, ld_IntMinY, ld_IntMaxY);
      }
      else			// grid space is a torus
	lC_intersection = aCr_envelope;

      // compute candidate grid range min and max
      Index lC_min;
      Index lC_MinQtr =
	this->coord2index(geos::geom::Coordinate(lC_intersection.getMinX(),
					   lC_intersection.getMaxY() ),
			  lC_min,2,2);
      if ( lC_MinQtr.col() == 1 )
	lC_min.col()++;
      if ( lC_MinQtr.row() == 1 )
	lC_min.row()++;

      Index lC_max;
      Index lC_MaxQtr =
	this->coord2index(geos::geom::Coordinate(lC_intersection.getMaxX(),
					   lC_intersection.getMinY() ),
			  lC_max,2,2);
      if ( lC_MaxQtr.col() == 0 )
	lC_max.col()--;
      if ( lC_MaxQtr.row() == 0 )
	lC_max.row()--;

      aCr_result.init(lC_min,lC_max);

      return true;

    } // end of Geogrid::intersection(const Range&...)

    /**
     * Returns true if the grid space is a torus.
     *
     * @returns whether the grid space is a torus
     */
    bool Geogrid::isa_torus() const {
      return (dynamic_cast<Torus*>(mCp_index.get()));
    }

    /**
     * Utility function for monitoring progress of GDALDdriver::CreateCopy(...).
     * Code borrowed from GDAL website.
     *
     * @param dfComplete current progress ([0-1.0]
     * @param pszMessage message
     * @param pData void pointer to data
     */
    int MyTextProgress( double dfComplete, const char *pszMessage, void *pData)
    {
      if( pszMessage != NULL )
	std::cout << (int) (dfComplete*100) << "% complete: " << pszMessage
		  << std::endl;
      else if( pData != NULL )
	std::cout << (int) (dfComplete*100) << "% complete: " << ((char*)pData)
		  << std::endl;
      else
	std::cout << (int) (dfComplete*100) << "% complete.\n";
     
      return TRUE;
    }

    /**
     * utility method for creating Geogrid based on data type stored in a string
     *
     * @param acp_type data type in a string
     * @returns handle to new geogrid of type <acp_type>
     */
    Geogrid* CreateGeogrid(const char * const acp_type) {
      Geogrid* lCp_geogrid = 0;

      std::string lC_type = acp_type;

      if (lC_type == "byte")
	lCp_geogrid = new ByteGeogrid;
      else if (lC_type == "char")
	lCp_geogrid = new CharGeogrid;
      else if (lC_type == "short")
	lCp_geogrid = new ShortGeogrid;
      else if (lC_type == "int")
	lCp_geogrid = new IntGeogrid;
      else if (lC_type == "long")
	lCp_geogrid = new LongGeogrid;
      else if (lC_type == "float")
	lCp_geogrid = new FloatGeogrid;
      else if (lC_type == "double")
	lCp_geogrid = new DoubleGeogrid;
      // else type not supported

      return lCp_geogrid;
    }

  } // namespace gis

} // namespace efscape
