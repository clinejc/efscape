// __COPYRIGHT_START__
// Package Name : efscape
// File Name : gdal_utils.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GDAL_UTILS_HPP
#define EFSCAPE_GIS_GDAL_UTILS_HPP

#include <gdal_priv.h>		// Geospatial Data Abstraction Library

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

// utility methods for converting hex to rgb
int hex2rgb(const char * hex, int& red, int& green, int& blue);
int hex2rgb(const char * hex, float& red, float& green, float& blue);

namespace geos {
  namespace geom {
    class Envelope;
  }
}

namespace efscape {
  namespace gis {

    GDALColorEntry getColor(double v, double vmin, double vmax);

    // forward declaration
    class Geogrid;

    // utility functions for creating Geogrid from GDAL data
    GDALDataType getGDALDataType(const Geogrid* aCp_geogrid);
    Geogrid* CreateGeogrid(GDALDataType eType);
    Geogrid* CreateGeogrid(GDALDataType eType,
			   const char* acp_name,
			   int ai_nCols,
			   int ai_nRows,
			   int ai_nBands,
			   const geos::geom::Envelope& aCr_envelope,
			   const char* acp_projection);
    Geogrid* CreateGeogrid(const boost::shared_ptr<GDALDataset>& aCr_dataset);

    GDALDataset* createGDALDataset(const char* acp_name,
				   const char* acp_format,
				   const Geogrid* aCp_geogrid);

    // utility functions for reading and writing data from GDAL datasets
    int readGDALDataset(GDALDataset& aCr_dataset, Geogrid* aCp_grid);
    int writeGDALDataset(GDALDataset& aCr_dataset, const Geogrid* aCp_grid);

    // utilities for converting gdal strings to and from std strings
    std::vector<std::string> getStdStrings(char** papszStrings);
    char** getCSLStrings(const std::vector<std::string>& aC1_strings);

    //----------------------------------------------------
    // utility functions for getting and setting  metadata
    //----------------------------------------------------
    std::vector<std::string> getMetadataNames(GDALMajorObject* aCp_GDALObject);
    
  } // namespace gis
}   // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GDAL_UTILS_HPP
