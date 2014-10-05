// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GridCoverageExchangeI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <ogis/impl/GridCoverageExchangeI.hpp>

#include <ogis/impl/GridCoverageI.hpp>
#include <ogis/impl/ogis_utils.hpp>
#include <gdal_priv.h>		// Geospatial Data Abstraction Library

namespace ogis {

  namespace impl {

    /** constructor */
    GridCoverageExchangeI::GridCoverageExchangeI() :
      mCp_DriverManager(0)
    {
      GDALAllRegister();		// register GDAL drivers

      mCp_DriverManager = GetGDALDriverManager(); // handle to singleton

      std::cout << "Grid Exchange home = <" << mCp_DriverManager->GetHome()
		<< ">\n";
    }

    /** destructor */
    GridCoverageExchangeI::~GridCoverageExchangeI() {
      std::cout << "Destroying GridCoverageExchangeI...\n";
    }

    /**
     *  Returns the number of data formats.
     *
     * @param current current method invocation
     * @returns number of grid coverage formats
     */
    ::Ice::Long
    GridCoverageExchangeI::numFormats(const Ice::Current& current)
    {
      return ( mCp_DriverManager->GetDriverCount() );
    }

    /**
     * Returns a list of metadata names
     *
     * @param current current method invocation
     * @returns a list of metadata names
     */
    ::gc::StringSeq
    GridCoverageExchangeI::metadataNames(const Ice::Current& current)
    {
      return getMetadataNames((GDALMajorObject*)mCp_DriverManager);
    }

    /**
     * Return the metadata value <name>
     *
     * @param current method invocation
     * @returns value of metadata <name>
     */
    std::string
    GridCoverageExchangeI::getMetadataValue(const std::string& lC_name,
					    const Ice::Current& current)
    {
      return mCp_DriverManager->GetMetadataItem(lC_name.c_str());
    } // GridCoverageExchangeI::getMetadataName()

    /**
     * Returns the format at the specified index.
     *
     * @param aul_index format index
     * @param current current method invocation
     * @returns format at specified index
     * @throws ::gc::GcInvalidIndex
     */
    ::gc::GcFormat
    GridCoverageExchangeI::getFormat(::Ice::Long index,
				     const Ice::Current& current)
    {
      return ::gc::GcFormat();

      // verify that the index is valid
      if (index < 0 || index >= numFormats(current)) {
	throw ::gc::GcInvalidIndex();
      }

      // get driver
      GDALDriver* lCp_driver = mCp_DriverManager->GetDriver(index);

      ::gc::GcFormat lC_GcFormat;

      // set format attributes
      lC_GcFormat.name = lCp_driver->GetDescription();
      lC_GcFormat.description =
	lCp_driver->GetMetadataItem( GDAL_DMD_LONGNAME );
      lC_GcFormat.extension =
	lCp_driver->GetMetadataItem( GDAL_DMD_EXTENSION );

      // return format
      return lC_GcFormat;

    } // GridCoverageExchangeI::getFormat(...)

    /**
     * Returns a list of sub names associated with <name>.
     *
     * @param name name of grid coverage
     * @param current current method invocation
     * @returns list of sub names
     */
    ::gc::StringSeq
    GridCoverageExchangeI::listSubNames(const ::std::string& name,
					const Ice::Current& current)
    {
      return ::gc::StringSeq();
    }

    /**
     * Creates a new GcGridCoverage from the specified name.
     *
     * @param acp_name grid coverage name
     * @param current current method invocation
     * @returns handle to GC_GridCoverage
     */
    ::gc::GcGridCoveragePtr
    GridCoverageExchangeI::createFromName(const ::std::string& name,
					  const Ice::Current& current)
    {
      GDALDataset* lCp_dataset =
	(GDALDataset*) GDALOpen(name.c_str(), GA_Update);

      gc::GcGridCoveragePtr lCp_GridCoverage = 
	new GridCoverageI(lCp_dataset);

      // return handle to CORBA grid
      return ( lCp_GridCoverage );

    } // GridCoverageExchangeI::createFileFromName(...)

    /**
     * Exports a grid coverage to a file with the specified file format
     *
     * @param grid handle to grid coverage
     * @param fileFormat desired file format
     * @param fileName target file name
     * @param current current method invocation
     */
    void
    GridCoverageExchangeI::exportTo(const ::gc::GcGridCoveragePtr& grid,
				    const ::std::string& fileFormat,
				    const ::std::string& fileName,
				    const Ice::Current& current)
    {
      if (mCp_DriverManager == 0) {
	std::cerr << "Unable to retrieve GDAL driver manager\n";
	return;
      }

      // get driver
      GDALDriver* lCp_driver =
	mCp_DriverManager->GetDriverByName(fileFormat.c_str());
      if (lCp_driver == 0) {
	std::cerr << "Unable to retrieve driver <" << fileName << ">\n";
	return;
      }

      //--------------------------------------------
      // initialize grid values: get grid dimensions
      //--------------------------------------------

    } // GridCoverageExchangeI::exportTo(...)

    /**
     *  This method creates a GridCoverage
     *
     * @param aCp_dataset handle to GDAL dataset
     */
    GridCoverageI*
    GridCoverageExchangeI::CreateGridCoverageI(const
					       boost::shared_ptr<GDALDataset>&
					       aCp_dataset)
    {
      GridCoverageI* lCp_GridCoverage = NULL;

      lCp_GridCoverage = new GridCoverageI(aCp_dataset);

      // return handle to ICE GridCoverage servant
      return ( lCp_GridCoverage );

    } // GridCoverageExchangeI::CreateGridCoverageI(...)

  } // namespace impl

} // namespace ogis
