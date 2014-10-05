// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ogis_utils.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_OGIS_UTILS_HPP
#define EFSCAPE_GIS_OGIS_UTILS_HPP

#include "gdal_priv.h"		// Geospatial Data Abstraction Library

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

namespace ogis {
  namespace impl {

    // utilities for converting gdal strings to and from std strings
    std::vector<std::string> getStdStrings(char** papszStrings);
    char** getCSLStrings(const std::vector<std::string>& aC1_strings);

    //----------------------------------------------------
    // utility functions for getting and setting  metadata
    //----------------------------------------------------
    std::vector<std::string> getMetadataNames(GDALMajorObject* aCp_GDALObject);
    
  } // namespace impl
}   // namespace ogis

#endif	// #ifndef EFSCAPE_GIS_OGIS_UTILS_HPP
