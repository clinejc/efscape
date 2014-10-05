// __COPYRIGHT_START__
// Package Name : efscape
// File Name : LandscapeI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_LANDSCAPEI_HPP
#define EFSCAPE_GIS_LANDSCAPEI_HPP

#include <efscape/gis/Geogrid_impl.hh>
#include <gdal_priv.h>		// Geospatial Data Abstraction Library
#include <ogrsf_frmts.h>	// Adds support for GDAL/OGR simple features
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace efscape {

  namespace gis {

    // forward declarations
    class LandscapeI;

    // typedefs
    typedef boost::shared_ptr<LandscapeI> LandscapeIPtr;
    typedef boost::shared_ptr<OGRLayer> OGRLayerPtr;
    typedef std::set<GeogridPtr> GeogridSet;
    typedef std::map<std::string, GeogridPtr> GeogridMap;
    typedef std::set<OGRLayerPtr> OGRLayerSet;
    typedef std::map<std::string, OGRLayerPtr> OGRLayerMap;

    /**
     * The LandscapeI class provides a single unified interface for accessing
     * geospatial data. It includes covenience functions for accessing
     * individual raster grids by name.<br>
     * Major revision due to a major revision of GDAL/OGR (version 2.x).
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.0 created 13 Sep 2006, revised 05 Oct 2014
     *
     * ChangeLog:
     *  - 2014-10-05 major revision corresponding to GDAL/OGR updates.
     *  - 2010-11-26 revised
     *  - 2006-09-06 Created class LandscapeI
     */
    class LandscapeI {

    public:

      LandscapeI();
      ~LandscapeI();

      //
      // methods for accessing geogrids
      //
      Geogrid* addGeogrid(Geogrid* aCp_grid);

      Geogrid* getGeogrid(const char* acp_name);
      ByteGeogrid* getByteGeogrid(const char* acp_name);
      ShortGeogrid* getShortGeogrid(const char* acp_name);
      IntGeogrid* getIntGeogrid(const char* acp_name);
      LongGeogrid* getLongGeogrid(const char* acp_name);
      FloatGeogrid* getFloatGeogrid(const char* acp_name);
      DoubleGeogrid* getDoubleGeogrid(const char* acp_name);

      const Geogrid* getGeogrid(const char* acp_name) const;
      const ByteGeogrid* getByteGeogrid(const char* acp_name) const;
      const ShortGeogrid* getShortGeogrid(const char* acp_name) const;
      const IntGeogrid* getIntGeogrid(const char* acp_name) const;
      const LongGeogrid* getLongGeogrid(const char* acp_name) const;
      const FloatGeogrid* getFloatGeogrid(const char* acp_name) const;
      const DoubleGeogrid* getDoubleGeogrid(const char* acp_name) const;

      void getGeogrids(GeogridSet& aCCpr_geogrids);

      //
      // methods for accessing layers
      //

      /** @returns handle to GDALDataset */
      const boost::shared_ptr<GDALDataset>& getDataset() { return mCp_dataset; }

      void setDataset( const boost::shared_ptr<GDALDataset>& aCp_dataset);

    protected:

      /** map of landscape geogrid layers */
      boost::scoped_ptr<GeogridMap> mCCCp_Geogrids;

      /** handle to GDALDataset (dataset with feature layers) */
      boost::shared_ptr<GDALDataset> mCp_dataset;

    };				// class LandscapeI definition

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_LANDSCAPEI_HPP
