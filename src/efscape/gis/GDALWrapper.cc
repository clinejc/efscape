// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GDALWrapper.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/GDALWrapper.hh>

#include <efscape/gis/gdal_utils.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

namespace efscape {

  namespace gis {

    /** constructor */
    GDALWrapper::GDALWrapper() {}

    /**
     * constructor
     *
     * @param acp_name file name of grid
     * @param ab_is_mutable whether the datasource is writeable
     */
    GDALWrapper::GDALWrapper(const char* acp_filename, bool ab_is_mutable)
    {
      open(acp_filename, ab_is_mutable);
    } // GDALWrapper::GDALWrapper(const char*,bool)

    /**
     * constructor
     *
     * @param aCp_dataset handle to GDAL dataset
     */
    GDALWrapper::GDALWrapper(GDALDataset* aCp_dataset) {
      mCp_dataset.reset(aCp_dataset);
    }

    /**
     * constructor
     *
     * @param acp_name grid name
     * @param acp_format GDAL raster format
     * @param ai_nCols number of columns
     * @param ai_nRows number of rows
     * @param ai_nBands number of bands
     * @param ae_datatype,
     * @param ad1_transform affine transformation coefficients
     * @param acp_projection projection definition string for this dataset.
     */
    GDALWrapper::GDALWrapper(const char* acp_name,
			     const char* acp_format,
			     int ai_nCols,
			     int ai_nRows,
			     int ai_nBands,
			     GDALDataType ae_datatype,
			     double* ad1_transform,
			     const char* acp_projection) :
      mC_name(acp_name)
    {

      GDALAllRegister();	// register GDAL drivers
      GDALDriverManager* lCp_DriverManager = GetGDALDriverManager();

      GDALDriver* lCp_driver =
	lCp_DriverManager->GetDriverByName(acp_format);

      if (lCp_driver == NULL) {
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "driver<" << acp_format << "> not found\n";
	return;
      }

      // add file extension
      std::string lC_filename = std::string(acp_name) + "."
	+ lCp_driver->GetMetadataItem( GDAL_DMD_EXTENSION );

      // attempt to create grid
      char** papszOptions = NULL;
      mCp_dataset.reset(lCp_driver->Create(lC_filename.c_str(),
					   ai_nCols,
					   ai_nRows,
					   ai_nBands,
					   ae_datatype,
					   papszOptions));

      if (!is_open()) {
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "Unable to create GDALDataset\n";
	return;
      }

      if (mCp_dataset->SetGeoTransform(ad1_transform) == CE_Failure)
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "Unable to set geotransform\n";
      if (mCp_dataset->SetProjection(acp_projection) == CE_Failure)
	std::cerr << "*** GDALWrapper constructor error ***: "
		  << "Unable to set projection\n";

    } // GDALWrapper(const char*, const char*, int,...)

    /** destructor */
    GDALWrapper::~GDALWrapper() {}

    /**
     * Open file <acp_filename>.
     *
     * @param acp_filename file name
     * @param ab_is_mutable whether the datasource is writeable
     */
    void GDALWrapper::open(const char* acp_filename,
			   bool ab_is_mutable)
    {
      mC_name = acp_filename;

      // filename may consist of 3 ';'-delimited arguments
      //   <format>:<filename>:<subdataset>
      std::vector<std::string> lC1_args;
      // efscape::utils::split(mC_name,':', lC1_args);
      boost::split(lC1_args, mC_name, boost::is_any_of(":"));
      if (lC1_args.size() == 3)
	mC_name = lC1_args[2];	// subdataset name is the third argument

      if (is_open())
	close();

      // Use GDAL to open grid
      GDALAllRegister();	// register GDAL drivers

      GDALAccess le_access = GA_ReadOnly;
      if (ab_is_mutable)
	le_access = GA_Update;

      mCp_dataset.reset( (GDALDataset*) GDALOpen( acp_filename, le_access ) );

      if (!is_open()) {
	std::cerr << "Unable to open raster file <" << acp_filename << ">\n";
	return;
      }

    } // GDALWrapper::open(const char*)
    
    /** @returns whether the dataset is open */
    bool GDALWrapper::is_open() const { return mCp_dataset.get() != 0; }

    /** @returns whether this datasource is mutable */
    bool GDALWrapper::is_mutable() const {
      if (mCp_dataset.get() == 0)
	return false;
      return mCp_dataset->GetAccess() == GA_ReadOnly;
    }

    /**
     * Creates a geogrid associated with this dataset.
     *
     * @param acp_name geogrid name
     * @returns handle to new geogrid
     */
    Geogrid* GDALWrapper::create_geogrid(const char* acp_name) const {

      // determine the data type
      GDALDataType eType = mCp_dataset->GetRasterBand(1)->GetRasterDataType();

      Geogrid* lCp_geogrid = CreateGeogrid(eType);

      if (lCp_geogrid != 0) {

	// set geogrid name (ignore the supplied name)
	// to do (2007.02.11): fix problem with GeoObject::basename()
	lCp_geogrid->name(boost::filesystem::basename(name()).c_str());
	read(lCp_geogrid,0);
      }

      return lCp_geogrid;

    } // Geogrid* create_geogrid(const char* acp_name) const

    /**
     * Reads Geogrid data.
     *
     * @param aCp_grid handle to Geogrid
     * @param al_index index of data record
     * @returns exit status
     * @throws std::out_of_range
     */ 
    int GDALWrapper::read(Geogrid* aCp_grid,
			  long al_index) const
      throw (std::out_of_range)
    {
      if (aCp_grid == NULL || mCp_dataset.get() == NULL)
	return 1;

      return readGDALDataset(*mCp_dataset, aCp_grid);
    }

    /**
     * Writes Geogrid data.
     *
     * @param aCp_grid handle to Geogrid
     * @param al_index index of data record
     * @returns exit status
     * @throws std::out_of_range
     */ 
    int GDALWrapper::write(const Geogrid* aCp_grid,
			   long al_index)
      throw (std::out_of_range)
    {
      if (aCp_grid == NULL || mCp_dataset.get() == NULL)
	return 1;

      return writeGDALDataset(*mCp_dataset, aCp_grid);
    }

  } // namespace gis

} // namespace efscape
