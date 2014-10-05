// __COPYRIGHT_START__
// Package Name : efscape
// File Name : TestCode.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <TestCode.hh>

#include <efscape/gis/GDALWrapper.hh>
#include <efscape/gis/GeoNetCDF.hh>
#include <efscape/gis/gdal_utils.hpp>
#include <gdal_priv.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <ogr_spatialref.h>

#include <iostream>

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace gistest {

  //---------------------------------------
  // start of class OpenGDAL implementation
  //---------------------------------------
  /** constructor */
  OpenGDAL::OpenGDAL()
  {
    name("OpenGDAL");
    info("Tests ability to open a GDAL dataset");
    mC_filename = "data/sics_veg.tif";
  }

  /**
   * Initializes the test with the specified parameters and returns the status.
   *
   * @param aC1r_args vector of arguments
   * @returns status of initialization
   */
  int OpenGDAL::init(const std::vector<std::string>& aC1r_args) {
    if (aC1r_args.size() != 1) {
      mC_ErrorMsg << "test <" << name() << "> " << usage() << "\n";
      return EXIT_FAILURE;
    }
    mC_filename = aC1r_args[0];
    return EXIT_SUCCESS;
  }

  /**
   * Opens a GDAL.
   *
   * @param acp_filename file name of raster dataset
   * @return status of call
   */
  int OpenGDAL::operator()() {

    //------------------
    // open GDAL dataset
    //------------------
    efscape::gis::GDALWrapper lC_GDALWrapper(mC_filename.c_str(),GA_ReadOnly);

    if (lC_GDALWrapper.dataset() == 0) {
      mC_ErrorMsg << "Unable to open raster file <" << mC_filename.c_str()
		  << ">\n";

      return EXIT_FAILURE;
    }

    std::cout << "Successfully opened raster file<" << mC_filename.c_str()
	      << ">\n";

    int li_NumBands = lC_GDALWrapper.dataset()->GetRasterCount();
    std::cout << "Number of bands = " << li_NumBands
	      << std::endl
	      << "X size = " << lC_GDALWrapper.dataset()->GetRasterXSize()
	      << std::endl
	      << "Y size = " << lC_GDALWrapper.dataset()->GetRasterYSize()
	      << std::endl;

    if (li_NumBands == 0) {
      mC_ErrorMsg << "Raster file has no bands\n";

      return EXIT_FAILURE;
    }

    // test the geodataset iterator
    efscape::gis::GeoDataSource::Iterator lC_GridIter =
      lC_GDALWrapper.begin( lC_GDALWrapper.name() );
    geos::geom::Envelope lC_envelope;
    geos::geom::Coordinate lC_resolution;

    std::cout << "GeoDataSource Iterator:"
	      << "\n\tname:<" << lC_GridIter.name() << ">"
	      << "\n\tindex:" << lC_GridIter.index()
	      << std::endl;

    if (lC_GridIter.is_valid()) {
      std::cout << "\n\tnumber of bands: " << (*lC_GridIter)->num_bands()
		<< "\n\tnumber of rows: " << (*lC_GridIter)->num_rows()
		<< "\n\tnumber of columns: " << (*lC_GridIter)->num_cols()
		<< std::endl;

      lC_envelope = (*lC_GridIter)->envelope();
      lC_resolution = (*lC_GridIter)->resolution();

      std::cout << "Origin = ("
		<< lC_envelope.getMinX() << ", " 
		<< lC_envelope.getMaxY() << ")\n";
      std::cout << "Pixel Size = ("
		<< lC_resolution.x << ", "
		<< lC_resolution.y << ")\n";
    }
    else
      std::cout << "Grid Iterator has a null grid\n";



    //--------------------------
    // retrieve raster band info
    //--------------------------
    efscape::gis::Geogrid_impl<double> lC_grid("grid");
    if (lC_GDALWrapper.read(&lC_grid,0) != 0) {
      mC_ErrorMsg << "Unable to read raster data from file <"
		  << mC_filename.c_str() << ">\n";
    }

    std::cout << "Number of raster bands = " << lC_grid.num_bands()
	      << std::endl;
    lC_envelope = lC_grid.envelope();
    lC_resolution = lC_grid.resolution();

    std::cout << "Origin = ("
	      << lC_envelope.getMinX() << ", " 
	      << lC_envelope.getMaxY() << ")\n";
    std::cout << "Pixel Size = ("
	      << lC_resolution.x << ", "
	      << lC_resolution.y << ")\n";

    //-------------------
    // retrieve grid data
    //-------------------
    std::cout << "Band is " << lC_grid.num_rows() << "x" << lC_grid.num_cols()
	      << std::endl;

    int li_RowMax = (9 < lC_grid.num_rows()-1) ?  9 : lC_grid.num_rows()-1;
    int li_ColMax = (9 < lC_grid.num_cols()-1) ?  9 : lC_grid.num_cols()-1;

    efscape::gis::Index lC_MinIndex(0,0);
    efscape::gis::Index lC_MaxIndex(li_ColMax,li_RowMax);
    efscape::gis::Range lC_range(lC_MinIndex,lC_MaxIndex);

    std::cout << "Printing grid data from range " << lC_range << "\n"
	      << "\tgrid data is "
	      << lC_range.num_rows() << "X"
	      << lC_range.num_cols() << "\n"
	      << "\tgrid data size = " << lC_range.size() << "\n";

    // test grid operator()[size_t ai_row][size_t ai_col]
    efscape::gis::DoubleGeogrid* lCp_dblgrid = NULL;
    if ( (lCp_dblgrid = dynamic_cast<efscape::gis::DoubleGeogrid*>(*lC_GridIter)) ) {
      std::cout << "Geogrid iterator is of type <double>\n";
    }
    else {
      std::cout << "Geogrid iterator is NOT of type <double>\n";
      lCp_dblgrid = &lC_grid;
    }


    std::cout << "grid data (operator test):\n";
    for (int iRow = lC_MinIndex.row(); iRow <= lC_MaxIndex.row(); iRow++) {
      for (int iCol = lC_MinIndex.col(); iCol <= lC_MaxIndex.col(); iCol++)
	std::cout << (int)(*lCp_dblgrid)()[0][iRow][iCol] << " ";
      std::cout << std::endl;
    }

    return EXIT_SUCCESS;

  } // OpenDataSet(...)

  //-------------------------------------
  // end of class OpenGDAL implementation
  //-------------------------------------

  //----------------------------------------------
  // start of class CreateGDAL implementation
  //----------------------------------------------
  /** constructor */
  CreateGDAL::CreateGDAL() :
    mC_filename("raster.tif"),
    mC_drivername("GTiff")
  {
    name("CreateGDAL");
    info("Tests ability to create a new GDAL dataset");
  }

  /**
   * Initializes the test with the specified parameters and returns the status.
   *
   * @param aC1r_args vector of arguments
   * @returns status of initialization
   */
  int CreateGDAL::init(const std::vector<std::string>& aC1r_args) {
    if (aC1r_args.size() != 2) {
      mC_ErrorMsg << "test <" << name() << "> " << usage() << "\n";

      return EXIT_FAILURE;
    }
    mC_filename = aC1r_args[0];
    mC_drivername = aC1r_args[1];
    return EXIT_SUCCESS;
  }

  /**
   * Opens a new GDAL.
   *
   * @returns exit status of test
   */
  int CreateGDAL::operator()() {

    // get driver for this format
    GDALDriverManager* poDriverManager = GetGDALDriverManager();
    GDALDriver* poDriver =
      poDriverManager->GetDriverByName(mC_drivername.c_str());

    if (poDriver == 0) {
      mC_ErrorMsg << "Unable to retrieve driver <" << mC_drivername.c_str()
		  << ">\n";
      return EXIT_FAILURE;
    }

    std::cout << "Retrieved driver <" << mC_drivername.c_str() << ">\n";

    //------------------------
    // create new GDAL dataset
    //------------------------
    char** papszOptions = NULL;
    GDALDataset* poNewDataset =
      poDriver->Create(mC_filename.c_str(), 100, 100, 1,
		       GDT_Byte, papszOptions);

    if ( poNewDataset == 0) {
      mC_ErrorMsg << "New dataset is null\n";
      return EXIT_FAILURE;
    }

    // set geotransform
    double ld1_transform[6];
    ld1_transform[0] = 535255.39031473; // upper left x
    ld1_transform[1] = 500.0;		// w-e pixel resolution 
    ld1_transform[2] = 0;
    ld1_transform[3] = 2791354.27830428; // upper left y
    ld1_transform[4] = 0;
    ld1_transform[5] = -500.0;		// n-s pixel resolution 

    // set projection
    OGRSpatialReference lC_oSRS;

    // get band info
    int nBlockXSize, nBlockYSize;
    int bGotMin, bGotMax;
    double adfMinMax[2];
    GDALRasterBand* poBand = poNewDataset->GetRasterBand(1);

    // now create color table
    GDALColorTable lC_ColorTable;

    for (int iState = 0; iState <  8; iState++) {
      GDALColorEntry lC_Entry = efscape::gis::getColor((double)iState,
						       0,7);
      lC_ColorTable.SetColorEntry(iState,&lC_Entry);
    }

    poBand->SetColorTable(&lC_ColorTable);
    poBand->SetColorInterpretation(GCI_PaletteIndex);

    // write raster band data
    int nRows = poBand->GetXSize();
    int nCols = poBand->GetYSize();
    std::vector<GByte> luc1_data(nRows*nCols);
    for (int iRow = 0; iRow < nRows; iRow++)
      for (int iCol = 0; iCol < nCols; iCol++) {
	int state =  rand() % 8;
	luc1_data[iRow*nCols+iCol] = (GByte)state;
      }

    poBand->RasterIO( GF_Write, 0, 0, nCols, nRows,
		      &luc1_data[0],
		      nCols, nRows,
		      GDT_Byte, 0, 0 );

    poNewDataset->FlushCache();
 
    CSLDestroy(papszOptions);

    // print list of files associated with this dataset
    std::vector<std::string> lC1_names =
      efscape::gis::getStdStrings(poNewDataset->GetFileList());
    std::cout << "There are " << lC1_names.size() << " files\n";
    for (int i = 0; i < lC1_names.size(); i++) {
      std::cout << "file " << i << " = <" << lC1_names[i] << ">\n";
    }

    // print raster band info
    std::cout << "Raster band is of type <" << poBand->GetRasterDataType()
	      << ">\n";
    poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
    std::cout << "Block=" << nBlockXSize << "x" << nBlockYSize << " Type="
	      << GDALGetDataTypeName(poBand->GetRasterDataType())
	      << ", ColorInterp="
	      << GDALGetColorInterpretationName( poBand->GetColorInterpretation())
	      << std::endl;

   // attempt to create mask band
   if ( ( poNewDataset->CreateMaskBand(0/*GMF_PER_DATASET*/) ) == CE_None) {
     std::cout << "Successful creation of mask\n";
   }
   else
     std::cout << "Attempt to create mask failed\n";

   // attempt to add raster band
    std::cout << "Adding raster band...\n";
    if ( ( poNewDataset->AddBand(GDT_Byte) ) == CE_None) {
      std::cout << "Successful addition\n";
    }
    else
      std::cout << "Addition failed\n";

    return EXIT_SUCCESS;

  } // CreateGDAL::operator()()

  //---------------------------------------
  // end of class CreateGDAL implementation
  //---------------------------------------

  //----------------------------------------------
  // start of class CreateGeoNetCDF implementation
  //----------------------------------------------
  /** constructor */
  CreateGeoNetCDF::CreateGeoNetCDF()
  {
    name("CreateGeoNetCDF");
    info("Tests ability to create a new GeoNetCDF dataset");
    mC_filename = "hydrology.nc";
  }

  /**
   * Initializes the test with the specified parameters and returns the status.
   *
   * @param aC1r_args vector of arguments
   * @returns status of initialization
   */
  int CreateGeoNetCDF::init(const std::vector<std::string>& aC1r_args) {
    if (aC1r_args.size() > 1) {
      mC_ErrorMsg << "test <" << name() << "> " << usage() << "\n";
      return EXIT_FAILURE;
    }
    mC_filename = aC1r_args[0];
    return EXIT_SUCCESS;
  }

  /**
   * Opens a new GeoNetCDF.
   *
   * @returns exit status of test
   */
  int CreateGeoNetCDF::operator()() {

    // netcdf_cpp test
    std::cout << "Attempting to create a NetCDF file...\n";

    // grid origin is the upper left corner
    geos::geom::Envelope lC_envelope(537255.39031473, // easting
				     535255.39031473,  // westing
				     2789354.27830428, // southing
				     2791354.27830428); // northing
    geos::geom::Coordinate lC_res(500,500);

    OGRSpatialReference lC_oSRS;

    lC_oSRS.SetWellKnownGeogCS("WGS84");
    lC_oSRS.SetUTM( 17, TRUE );
    char    *pszWKT = NULL;
    lC_oSRS.exportToWkt( &pszWKT );

    efscape::gis::GeoNetCDF lC_NcGrid(mC_filename.c_str(),
				      4,
				      4,
				      1,
				      lC_envelope,
				      pszWKT,
				      true);

    CPLFree(pszWKT);

    // verify that the grid is properly georeferenced
    std::cout << "GeoNetCDF test:";
    if ( lC_NcGrid.is_georeferenced() ) {

      std::cout << "\n\tnum_rows = " << lC_NcGrid.num_rows()
		<< "\n\tnum_cols = " << lC_NcGrid.num_cols()
		<< "\n\tnum_bands = " << lC_NcGrid.num_bands()
		<< "\n\textent = " << lC_NcGrid.envelope().toString()
		<< "\n\tprojection = " << lC_NcGrid.spatial_ref()
		<< std::endl;

      // global attributes
      ptime lC_base_date(date(1996,Feb,1),hours(0));
      lC_NcGrid.time_units(5,"days",lC_base_date);

      lC_NcGrid.dataset()->add_att("missing_value",-999.);

      // add variables
      lC_NcGrid.add_geogrid("depths", ncFloat);
      lC_NcGrid.add_geogrid("salinity", ncFloat);

      std::cout << "number of records = " << lC_NcGrid.num_recs() 
		<< std::endl
		<< "number of bands = " << lC_NcGrid.num_bands() 
		<< std::endl;

      // add local attributes
      lC_NcGrid.dataset()->get_var("depths")->add_att("long_name",
						      "mean water depth");
      lC_NcGrid.dataset()->get_var("depths")->add_att("units","mm");
      lC_NcGrid.dataset()->get_var("depths")->add_att("_FillValue",
						      (float)-999);
      lC_NcGrid.dataset()->get_var("salinity")->add_att("long_name",
							"mean salinity");
      lC_NcGrid.dataset()->get_var("salinity")->add_att("units","ppt");
      lC_NcGrid.dataset()->get_var("salinity")->add_att("_FillValue",
						      (float)-999);

      // create geogrid vector
      std::vector< efscape::gis::FloatGeogrid* > lCp1_geogrids;

      for (int i = 0; i < lC_NcGrid.dataset()->num_vars(); i++) {
	std::string lC_name = lC_NcGrid.dataset()->get_var(i)->name();
	if (!lC_NcGrid.is_georeferenced(lC_name.c_str())) {
	  std::cout << "Skipping non-raster variable <"
		    << lC_name << ">\n";
	  continue;
	}

	std::cout << "Successfully accessed variable <"
		  <<lC_name
		  << ">\n";

	efscape::gis::FloatGeogrid* lCp_geogrid = (efscape::gis::FloatGeogrid*)
	  lC_NcGrid.create_geogrid(lC_name.c_str());

	lCp1_geogrids.push_back( lCp_geogrid );
	int iGrid = lCp1_geogrids.size()-1;

	std::cout << "grid <" << lCp1_geogrids[iGrid]->name() << ">:"
		  << "\n\tnum_rows = " << lCp1_geogrids[iGrid]->num_rows()
		  << "\n\tnum_cols = " << lCp1_geogrids[iGrid]->num_cols()
		  << "\n\tnum_bands = " << lCp1_geogrids[iGrid]->num_bands()
		  << "\n\tresolution = ("
		  << lCp1_geogrids[iGrid]->resolution().x << ","
		  << lCp1_geogrids[iGrid]->resolution().y << ")"
		  << std::endl;
      }

      std::cout << "number of records = " << lC_NcGrid.num_recs() 
		<< std::endl;
      std::cout << "number of bands = " << lC_NcGrid.num_bands() 
		<< std::endl;

      // access the grid variable

      // write data
      float lf_missing = lC_NcGrid.dataset()->get_var("depths")->get_att("_FillValue")->as_double(0);
      long ll_num_recs = 2;
      for (int iDate = 0; iDate < ll_num_recs; iDate++) {

	std::cout << "At time = " << iDate << "...\n";

	for (int iGrid = 0; iGrid < lCp1_geogrids.size(); iGrid++) {

	  float lf_val = 0.;
	  std::cout << "\tAccessing grid <"
		    << lCp1_geogrids[iGrid]->name()
		    << ">...\n";

	  efscape::gis::FloatGeogrid* lCp_geogrid= lCp1_geogrids[iGrid];

	  for (int iRow = 0; iRow < lCp_geogrid->num_rows(); iRow++)
	    for (int iCol = 0; iCol < lCp_geogrid->num_cols(); iCol++) {
	      if ( (iRow == 0 && iCol == 0) || (iRow == 3 && iCol == 1) )
		(*lCp_geogrid)()[0][iRow][iCol] = lf_missing;
	      else {
		++lf_val;
		(*lCp_geogrid)()[0][iRow][iCol] =
		  iDate*1000. + iGrid*100. + lf_val;
	      }
	    }

	  std::cout << "\t\twriting data\n";
	  lC_NcGrid.write(lCp_geogrid,iDate);
	}

      }

      for (int i = 0; i < lCp1_geogrids.size(); i++)
	delete lCp1_geogrids[i];

      std::cout << "After writing data, number of records = "
		<< lC_NcGrid.num_recs()
		<< std::endl;

      ptime lC_datetime = lC_NcGrid.base_date();
      if (lC_datetime != not_a_date_time)
	std::cout << "\tbase_date=<" << lC_datetime << ">\n";

      std::cout << "\ttime unit = " << lC_NcGrid.time_unit() 
		<< std::endl
		<< "\tdelta_t = " << lC_NcGrid.delta_t() 
		<< std::endl;

      std::cout << "\tdate of 2nd record = <" << lC_NcGrid.date(1) << ">\n";

      // close and reopen file with GeoNetCDF
      lC_NcGrid.close();

      std::cout << "Closing and reopening netCDF file <"
		<< mC_filename << "> with GeoNetCDF...\n";
      lC_NcGrid.open(mC_filename.c_str(),
		     NcFile::Write);

      if (lC_NcGrid.is_open())
	std::cout << "\tAttempt to reopen netCDF file successful!\n";
      else
	std::cout << "\tAttempt to reopen netCDF file <"
		  << mC_filename << "> failed!\n";

      std::cout << "\tnum_rows = " << lC_NcGrid.num_rows()
		<< "\n\tnum_cols = " << lC_NcGrid.num_cols()
		<< "\n\tnum_bands = " << lC_NcGrid.num_bands() << "\n";

      // close and reopen file with gdal (also opened by GeoNetCDF)
      std::cout << "Opening netCDF file with GDALWrapper...\n";

      std::string lC_gridname = "depths";
      std::string mC_filename2 = "netCDF:" + mC_filename + ":" + lC_gridname;
      efscape::gis::GDALWrapper lC_GDALWrapper(mC_filename2.c_str(), false);
      if (lC_GDALWrapper.is_open()) {
	std::cout << "\tnetCDF geogrid succesfully reopened with gdal\n";

	// display GeoTransform array
	double ld1_GeoTransform[6];
	lC_GDALWrapper.dataset()->GetGeoTransform(ld1_GeoTransform);
	std::cout << "GeoTransform = {";
	for (int i = 0; i < 6; i++)
	  std::cout << " " << ld1_GeoTransform[i];
	std::cout << " }\n";

	// display the no data value
	double lf_no_data =
	  lC_GDALWrapper.dataset()->GetRasterBand(1)->GetNoDataValue();
	std::cout << "no data value = "
		  << lf_no_data
		  << "\n";

	// attempt to create float geogrid
	// note: creating a second geogrid to accomadate GeoNetCDF, which does
	// not treat the time dimension as the band dimension
	efscape::gis::FloatGeogrid* lCfp_geogrid = (efscape::gis::FloatGeogrid*)
	  lC_GDALWrapper.create_geogrid(lC_gridname.c_str());
	lCfp_geogrid->set_dataset(lC_GDALWrapper.dataset());

	efscape::gis::FloatGeogrid* lCfp_geogrid2 =
	  new efscape::gis::FloatGeogrid(lC_gridname.c_str(),
					 lCfp_geogrid->num_cols(),
					 lCfp_geogrid->num_rows(),
					 1,
					 lCfp_geogrid->envelope(),
					 lCfp_geogrid->spatial_ref());

	if (lCfp_geogrid) {
	  std::cout << "Successfully load grid data...\n";
	  std::cout << "grid <" << lCfp_geogrid->name() << ">:"
		    << "\n\tnum_rows = " << lCfp_geogrid->num_rows()
		    << "\n\tnum_cols = " << lCfp_geogrid->num_cols()
		    << "\n\tnum_bands = " << lCfp_geogrid->num_bands()
		    << "\n\tresolution = ("
		    << lCfp_geogrid->resolution().x << ","
		    << lCfp_geogrid->resolution().y << ")"
		    << std::endl;
	  
	  for (int iRow = 0; iRow < lCfp_geogrid->num_rows(); iRow++)
	    for (int iCol = 0; iCol < lCfp_geogrid->num_cols(); iCol++) {
	      (*lCfp_geogrid2)(iRow, iCol) = (*lCfp_geogrid)(0, iRow, iCol);
	      if ( (*lCfp_geogrid)(0, iRow, iCol) != lf_no_data)
		(*lCfp_geogrid2)(iRow, iCol)*=2;
	    }

	  // first attempt to write data via GeoNetCDF (should work)
	  if (lC_NcGrid.write(lCfp_geogrid2,0) > 0)
	    std::cout << "Attempt to write netcdf grid data failed!\n";
	  else
	    std::cout << "Attempt to write netcdf grid data succeed!\n";
	  lC_NcGrid.dataset()->sync();

	  efscape::gis::Geogrid* lCp_testgrid = (efscape::gis::Geogrid*)lCfp_geogrid2;
	  if (lC_NcGrid.read(lCp_testgrid,0) == 0)
	    std::cout << "Rereading netcdf data\n";
	  else
	    std::cout << "Unable to reread netcdf data\n";

	  // next, attempt to write data via GDAL (should fail!)
	  if (lCfp_geogrid->write() == 1)
	    std::cout << "Attempt to write grid data failed\n";
	  else
	    std::cout << "Attempt to write grid data succeeded\n";

	  // Attempt to re-read GDAL data (should be unchanged)
	  std::cout << "Re-reading GDAL data...\n";
	  if (lC_GDALWrapper.read(lCfp_geogrid, 0) > 0)
	    std::cout << "Attempt to re-read GDAL data failed\n";
	  else
	    std::cout << "Attempt to re-read GDAL data succeeded\n";
	  for (int iRow = 0; iRow < lCfp_geogrid->num_rows(); iRow++)
	    for (int iCol = 0; iCol < lCfp_geogrid->num_cols(); iCol++) {
	      std::cout << (*lCfp_geogrid)(iRow, iCol) << " ";
	      if (iCol == lCfp_geogrid->num_cols()-1)
		std::cout << std::endl;
	    }

	  std::cout << "\nUpdated netcdf data...\n";
	  for (int iRow = 0; iRow < lCfp_geogrid2->num_rows(); iRow++)
	    for (int iCol = 0; iCol < lCfp_geogrid2->num_cols(); iCol++) {
	      std::cout << (*lCfp_geogrid2)(iRow, iCol) << " ";
	      if (iCol == lCfp_geogrid2->num_cols()-1)
		std::cout << std::endl;
	    }

	  // close and re-open GDAL (should reflect update now)
	  lC_GDALWrapper.open(mC_filename2.c_str(), false);
	  lC_GDALWrapper.read(lCfp_geogrid, 0);
	  for (int iRow = 0; iRow < lCfp_geogrid->num_rows(); iRow++)
	    for (int iCol = 0; iCol < lCfp_geogrid->num_cols(); iCol++) {
	      std::cout << (*lCfp_geogrid)(iRow, iCol) << " ";
	      if (iCol == lCfp_geogrid->num_cols()-1)
		std::cout << std::endl;
	    }

	  // get names of files associated with the dataset (does not work
	  // with GDAL and netcdf
	  std::vector<std::string> lC1_FileNames =
	    efscape::gis::getStdStrings(lCfp_geogrid->dataset()->GetFileList());

	  std::cout << "There are " << lC1_FileNames.size() << " files\n";

	  for (int iFile = 0; iFile < lC1_FileNames.size(); iFile++)
	    std::cout << "*** GDAL file name " << iFile << " = "
		      << "<" << lC1_FileNames[iFile] << ">\n";

	  // finally, demonstrate ability to create a new netCDF file via
	  // GDAL (really uses GeoNetCDF and create a read-only GDAL dataset)
	  GDALDataset* lCp_dataset =
	    efscape::gis::createGDALDataset(lCfp_geogrid->name(), "netCDF",
					    lCfp_geogrid);
	  if (lCp_dataset != 0) {
	    std::cout << "Attempt to create netCDF dataset succeeded!\n";
	    delete lCp_dataset;
	  }
	  else
	    std::cout << "Attempt to create netCDF dataset failed!\n";

	  delete lCfp_geogrid;	// done with geogrid
	  delete lCfp_geogrid2;	// done with geogrid
	}
	else
	  std::cout << "Unable to load grid data\n";
      }
      else
	std::cout << "Unable to reopen netCDF geogrid with gdal\n";

    }
    else
      std::cout << " this netCDF grid is not properly georeferenced\n";

    return EXIT_SUCCESS;

  } // CreateGeoNetCDF::operator()()

} // namespace gistest
