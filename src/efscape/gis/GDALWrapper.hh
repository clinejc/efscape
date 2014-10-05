// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GDALWrapper.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GDALWRAPPER_HH
#define EFSCAPE_GIS_GDALWRAPPER_HH

// parent class definition
#include <efscape/gis/GeoDataSource.hpp>

#include <gdal_priv.h>		// Geospatial Data Abstraction Library
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace efscape {

  namespace gis {

    /**
     * A base interface for accessing georeferenced grid data. It uses the
     * GDAL library for implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.97 created 23 Feb 2006, revised 09 Jun 2009
     */
    class GDALWrapper : public GeoDataSource
    {
       friend class boost::serialization::access;

    public:
      //
      // constructor/destructor
      //
      GDALWrapper();
      GDALWrapper(const char* acp_filename,
	      bool ab_is_mutable);
      GDALWrapper(GDALDataset* aCp_dataset);
      GDALWrapper(const char* acp_name,
	      const char* acp_format,
	      int ai_nCols,
	      int ai_nRows,
	      int ai_nBands,
	      GDALDataType ae_datatype,
	      double* ad1_transform,
	      const char* acp_projection);

      ~GDALWrapper();

      //
      // methods
      //
      void open(const char* acp_filename, bool ab_is_mutable);
      bool is_open() const;
      bool is_mutable() const;

      /** @returns name of dataset */
      const char* name() const { return mC_name.c_str(); }

      /**
       * Closes the GDAL dataset.
       *
       * @returns whether the dataset was closed successfully
       */
      bool close() { return true;}

      Geogrid* create_geogrid(const char* acp_name) const;

      /** @returns handle to GDALDataset */
      const boost::shared_ptr<GDALDataset>& dataset() { return mCp_dataset; }

      // read
      int read(Geogrid* aCp_grid, long al_index) const
	throw (std::out_of_range);

      // write
      int write(const Geogrid* aCp_grid, long al_index)
	throw (std::out_of_range);

    protected:

      /** handle to GDALDataset */
      boost::shared_ptr<GDALDataset> mCp_dataset;

      /** name */
      std::string mC_name;

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
	// parent class
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GeoDataSource);
      }

    };				// class GDALWrapper

    // typedef
    typedef boost::shared_ptr<GDALWrapper> GeoRasterGdalPtr; // deprecated
    typedef boost::shared_ptr<GDALWrapper> GDALWrapperPtr;

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GDALWRAPPER_HH
