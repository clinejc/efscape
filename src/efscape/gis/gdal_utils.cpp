// __COPYRIGHT_START__
// Package Name : efscape
// File Name : gdal_utils.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/gdal_utils.hpp>

#include <efscape/gis/gdal_utils.ipp>
#include <efscape/gis/Geogrid_impl.hh>
#include <efscape/gis/GeoNetCDF.hh>
#include <typeinfo>

// The following code for hex to rgb conversion was borrowed from
// http://www.velocityreviews.com/forums/t290718-p2-converting-a-hexidecimal-string-to-a-floating-point-array.html

// convert a hex color string to three integer RGB values.
// returns zero on success, non-zero on error.
int
hex2rgb(const char * hex, int& red, int& green, int& blue)
{
  unsigned long val;

  if (NULL == hex) {
    red = green = blue = 0;
    return 1;
  }

  // convert the hex string to an unsigned long int.
  // val will be zero if 'hex' is invalid.
  // Note: we should check 'errno' after this call
  // to see if a conversion error occurred;
  // see the docs for strtoul().
  val = strtoul(hex, NULL, 16);
  red = ((val & 0xff0000) >> 16);
  green = ((val & 0xff00) >> 8);
  blue = (val & 0xff);

  return 0;
}

// convert a hex color string to three float RGB values.
// returns zero on success, non-zero on error.
// Note this is the same as the int version except we
// divide each derived RGB color by 255.0. this method
// could simply call the int version then divide those
// results by 255.0 before returning
int
hex2rgb(const char * hex, float& red, float& green, float& blue)
{
  unsigned long val;

  if (NULL == hex) {
    red = green = blue = 0.0;
    return 1;
  }

  // convert the hex string to an unsigned long int.
  // val will be zero if 'hex' is invalid.
  // Note: we should check 'errno' after this call
  // to see if a conversion error occurred;
  // see the man page for strtoul(),
  val = strtoul(hex, NULL, 16);

  red = ((float)((val & 0xff0000) >> 16)) / 255.0;
  green = ((float)((val & 0xff00) >> 8)) / 255.0;
  blue = ((float)(val & 0xff)) / 255.0;

  return 0;
}

namespace efscape {
  namespace gis {

    /*
      Return a RGB colour value given a scalar v in the range [vmin,vmax]
      In this case each colour component ranges from 0 (no contribution) to
      1 (fully saturated), modifications for other ranges is trivial.
      The colour is clipped at the end of the scales if v is outside
      the range [vmin,vmax]
      typedef struct {
      double r,g,b;
      } COLOUR;

      Original C code copied from "Colour Ramping for Data Visualisation"
      written by Paul Bourke July 1996
      (http://local.wasp.uwa.edu.au/~pbourke/texture_colour/colourramp/
      accessed July 24, 2009). Modified by Jon Cline <jon.c.cline@gmail.com>
      to use GDALColorEntry.
    */
    GDALColorEntry getColor(double v,double vmin,double vmax)
    {
      GDALColorEntry c = {255,255,255,255}; // white
      double dv;

      if (v < vmin)
	v = vmin;
      if (v > vmax)
	v = vmax;
      dv = vmax - vmin;

      if (v < (vmin + 0.25 * dv)) {
	c.c1 = 0;
	c.c2 = (short)(255 * (4 * (v - vmin) / dv) );
      } else if (v < (vmin + 0.5 * dv)) {
	c.c1 = 0;
	c.c3 = (short)(255 * (1 + 4 * (vmin + 0.25 * dv - v) / dv) );
      } else if (v < (vmin + 0.75 * dv)) {
	c.c1 = (short)(255 * 4 * (v - vmin - 0.5 * dv) / dv);
	c.c3 = 0;
      } else {
	c.c2 = (short)(255 * (1 + 4 * (vmin + 0.75 * dv - v) / dv) );
	c.c3 = 0;
      }

      return(c);
    }

    /**
     * Utility methods for determining the GDALDataType associated with a
     * Geogrid object.
     *
     * @param aCp_geogrid handle to geogrid
     * @returns GDALDataType
     */
    GDALDataType getGDALDataType(const Geogrid* aCp_geogrid) {
      GDALDataType le_datatype;
      if (dynamic_cast<const ByteGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Byte;
      else if (dynamic_cast<const CharGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Byte;
      else if (dynamic_cast<const ShortGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Int16;
      else if (dynamic_cast<const LongGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Int32;
      else if (dynamic_cast<const FloatGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Float32;
      else if (dynamic_cast<const DoubleGeogrid*>(aCp_geogrid))
	le_datatype = GDT_Float64;

      return le_datatype;
    }

    /**
     * Utility method for creating Geogrid based on GDAL data type.
     *
     * @param eType GDAL data type
     * @returns handle to new Geogrid object
     */
    Geogrid* CreateGeogrid(GDALDataType eType) {

      Geogrid* lCp_geogrid = NULL;

      switch (eType) {
      case GDT_Byte:
	lCp_geogrid = new ByteGeogrid;
	break;
      case GDT_UInt16:
      case GDT_Int16:
	lCp_geogrid = new ShortGeogrid;
	break;
      case GDT_UInt32:
      case GDT_Int32:
	lCp_geogrid = new LongGeogrid;
	break;
      case GDT_Float32:
	lCp_geogrid = new FloatGeogrid;
	break;
      case GDT_Float64:
      default:
	lCp_geogrid = new DoubleGeogrid;
      }

      return lCp_geogrid;

    } // CreateGeogrid(GDALDataType)

    /**
     * Utility method for creating Geogrid based on GDAL data type.
     *
     * @param eType GDAL data type
     * @param acp_name grid name
     * @param ai_nCols number of columns
     * @param ai_nRows number of rows
     * @param ai_nBands number of bands
     * @param aCr_envelope spatial extent
     * @param acp_spatial_ref spatial reference as WKT
     * @returns handle to new Geogrid object
     */
    Geogrid* CreateGeogrid(GDALDataType eType,
			   const char* acp_name,
			   int ai_nCols,
			   int ai_nRows,
			   int ai_nBands,
			   const geos::geom::Envelope& aCr_envelope,
			   const char* acp_projection)
    {
      Geogrid* lCp_geogrid = CreateGeogrid(eType);
      lCp_geogrid->name(acp_name);
      lCp_geogrid->resize(ai_nCols,
			  ai_nRows,
			  ai_nBands,
			  aCr_envelope,
			  acp_projection);
      return lCp_geogrid;
    }

    /**
     * Utility method for creating Geogrid based on GDAL data type.
     *
     * @param eType GDAL data type
     * @returns handle to new Geogrid object
     */
    Geogrid* CreateGeogrid(const boost::shared_ptr<GDALDataset>& aCp_dataset) {
      Geogrid* lCp_geogrid = NULL;
      if (aCp_dataset.get() == NULL)
	return lCp_geogrid;

      // Two assumptions
      //   1. There is at least one raster band
      //   2. All raster bands are the same data type
      lCp_geogrid =
	CreateGeogrid(aCp_dataset->GetRasterBand(1)->GetRasterDataType() );

      if (lCp_geogrid == NULL)
	return lCp_geogrid;

      readGDALDataset(*aCp_dataset, lCp_geogrid);

      lCp_geogrid->set_dataset(aCp_dataset);

      return lCp_geogrid;
    }

    /**
     * Utility method for creating a GDALDataset from a Geogrid.
     *
     * @param acp_name file name
     * @param acp_format GDAL raster format
     * @param aCp_geogrid handle to geogrid
     */
    GDALDataset* createGDALDataset(const char* acp_name,
				   const char* acp_format,
				   const Geogrid* aCp_geogrid)
    {
      GDALDataset* lCp_dataset = NULL;

      // determine the appropriate driver
      GDALAllRegister();	// register GDAL drivers
      GDALDriverManager* lCp_DriverManager = GetGDALDriverManager();

      GDALDriver* lCp_driver =
	lCp_DriverManager->GetDriverByName(acp_format);

      if (lCp_driver == NULL) {
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "driver<" << acp_format << "> not found\n";
	return lCp_dataset;
      }

      // add file extension
      std::string lC_filename = std::string(acp_name) + "."
	+ lCp_driver->GetMetadataItem( GDAL_DMD_EXTENSION );

      // if this is a netCDF file, create it via GeoNetCDF
      if (std::string(acp_format) == "netCDF") {
	GeoNetCDF* lCp_GeoNetCDF = createGeoNetCDF(lC_filename.c_str(),
						   *aCp_geogrid);
	lC_filename = "netcdf:" + lC_filename + ":" + aCp_geogrid->name();
	lCp_dataset = (GDALDataset*) GDALOpen(lC_filename.c_str(),
					      GA_ReadOnly);
	delete lCp_GeoNetCDF;
	return lCp_dataset;
      }

      // attempt to create grid dataset
      char** papszOptions = NULL;
      lCp_dataset = lCp_driver->Create(lC_filename.c_str(),
				       aCp_geogrid->num_cols(),
				       aCp_geogrid->num_rows(),
				       aCp_geogrid->num_bands(),
				       getGDALDataType(aCp_geogrid),
				       papszOptions);

      if (lCp_dataset == NULL) {
	std::cerr << "Unable to create GDALDataset\n";
	return lCp_dataset;
      }

      geos::geom::Envelope lC_envelope = aCp_geogrid->envelope();
      geos::geom::Coordinate lC_resolution = aCp_geogrid->resolution();
      double ld1_GeoTransform[6];
      ld1_GeoTransform[2] = 0;
      ld1_GeoTransform[4] = 0;

      // grid origin is the upper left corner
      ld1_GeoTransform[0] = lC_envelope.getMinX();
      ld1_GeoTransform[3] = lC_envelope.getMaxY();

      // grid resolution
      ld1_GeoTransform[1] = lC_resolution.x;
      ld1_GeoTransform[5] = lC_resolution.y;
      
      if (lCp_dataset->SetGeoTransform(ld1_GeoTransform) == CE_Failure)
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "Unable to set geotransform\n";
      if (lCp_dataset->SetProjection(aCp_geogrid->spatial_ref()) == CE_Failure)
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "Unable to set projection\n";

      return lCp_dataset;

    } // createGDALDataset(...)

    /**
     * Utility function that reads GDALDataset as a Geogrid
     *
     * @param aCr_dataset reference to GDALDataset
     * @param aCp_grid handle to geogrid
     * @returns exit status
     */
    int readGDALDataset(GDALDataset& aCr_dataset, Geogrid* aCp_grid)
    {
      int li_status = 1;
      
      if (aCp_grid == 0)
	return li_status;

      // read in geogrid data
      if (dynamic_cast<ByteGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(ByteGeogrid&)*aCp_grid);
      else if (dynamic_cast<CharGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(CharGeogrid&)*aCp_grid);
      else if (dynamic_cast<ShortGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(ShortGeogrid&)*aCp_grid);
      else if (dynamic_cast<LongGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(LongGeogrid&)*aCp_grid);
      else if (dynamic_cast<FloatGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(FloatGeogrid&)*aCp_grid);
      else if (dynamic_cast<DoubleGeogrid*>(aCp_grid))
	li_status = readGDALDataset(aCr_dataset,(DoubleGeogrid&)*aCp_grid);

      return li_status;

    } // readGDALDataset(const GDALDataset&,Geogrid&)

    /**
     * Utility function that writes Geogrid data to a GDALDataset.
     *
     * @param aCr_dataset reference to GDALDataset
     * @param aCr_grid handle to const geogrid
     * @returns exit status
     */
    int writeGDALDataset(GDALDataset& aCr_dataset, const Geogrid* aCp_grid)
    {
      int li_status = 1;

      if (aCp_grid == 0)
	return li_status;

      // write geogrid data
      if (dynamic_cast<const ByteGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const ByteGeogrid&)*aCp_grid);
      else if (dynamic_cast<const CharGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const CharGeogrid&)*aCp_grid);
      else if (dynamic_cast<const ShortGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const ShortGeogrid&)*aCp_grid);
      else if (dynamic_cast<const LongGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const LongGeogrid&)*aCp_grid);
      else if (dynamic_cast<const FloatGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const FloatGeogrid&)*aCp_grid);
      else if (dynamic_cast<const DoubleGeogrid*>(aCp_grid))
	li_status = writeGDALDataset(aCr_dataset,(const DoubleGeogrid&)*aCp_grid);

      return li_status;

    } // writeGDALDataset(GDALDataset&,const Geogrid&)

    /**
     * Utility function for converting an array of gdal strings to a std vector
     * of std strings.
     *
     * @param papszString array of gdal strings
     * @returns vector of std strings
     */
    std::vector<std::string> getStdStrings(char** papszStrings) {
      std::vector<std::string> lC1_strings;

      int li_length = CSLCount(papszStrings);

      for (int i = 0; i < li_length; i++) {
	std::string lC_field = CSLGetField(papszStrings, i);

 	lC1_strings.push_back(lC_field);

      }	// end of for (int i = 0...

      return lC1_strings;
    }

    /**
     * Utility function for converting an std vector of std strings to an array
     * of gdal strings.
     *
     * @param aC1_string vector of string
     * @returns array of gdal string
     */
    char** getCSLStrings(const std::vector<std::string>& aC1_strings) {
      char** papszStrings = NULL;

      for (int i = 0; i < aC1_strings.size(); i++)
	papszStrings = CSLAddString(papszStrings, aC1_strings[i].c_str());

      return papszStrings;
    }

    /**
     * utility function for copying metadata names from a GDAMajorObject
     * to a string sequence
     *
     * @param aCp_GDALObject pointer to GDALObject
     * @returns string vector containing array of metadata names
     */
    std::vector<std::string> getMetadataNames(GDALMajorObject* aCp_GDALObject)
    {
      // access GDALMajorObject metadata
      char **papszMetadata = aCp_GDALObject->GetMetadata();

      // create a string sequence to hold metadata names
      std::vector<std::string> lC1_MetadataNames;

      int li_length = CSLCount(papszMetadata);

      //-----------------------------------------------------------------------
      // copy each metadata name to the sequence
      //
      // Each GDAL string field (see "cpl_string.h") consists of a name/value
      // pair separated by either a ":" or a "=". If neither separator is
      // found, the name is set to the entire field. (2006-02-06)
      //-----------------------------------------------------------------------
      for (int i = 0; i < li_length; i++) {
	std::string lC_field = CSLGetField(papszMetadata, i);

	// split name from <name>:<value> pair
	std::string lC_name;
	std::string::size_type pos = 0;
	std::string::size_type endpos = lC_field.size();

	// find name/pair separator
	if ( ( pos = lC_field.find(":") ) != std::string::npos )
	  endpos = pos;
	else if ( ( pos = lC_field.find("=") ) != std::string::npos )
	  endpos = pos;

	lC_name = lC_field.substr(0, endpos); // set metadata name

 	lC1_MetadataNames.push_back(lC_name);

      }	// end of for (int i = 0...

      CSLDestroy(papszMetadata);	// destroy the string list

      return lC1_MetadataNames;

    } // metadataNames(...)

  } // namespace gis
}   // namespace efscape
