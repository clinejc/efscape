// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CoverageI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <ogis/impl/CoverageI.ipp>

#include <gdal_priv.h>		// gdal is used to implement coverage
#include <ogis/impl/SampleDimensionI.hpp>
#include <ogis/impl/ogis_utils.hpp>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * @param aCp_dataset handle to GDAL dataset
     */
    CoverageI::CoverageI(GDALDataset* aCp_dataset) :
      mCp_dataset(aCp_dataset),
      mC_CoverageName("Coverage")
    {
    }

    /**
     * constructor
     *
     * @param aCp_dataset smart handle to GDAL dataset
     */
    CoverageI::CoverageI(const boost::shared_ptr<GDALDataset>& aCp_dataset) :
      mCp_dataset(aCp_dataset),
      mC_CoverageName("Coverage")
    {
    }

    /** destructor */
    CoverageI::~CoverageI() {
    }

    //-----------------------------
    // ICE services: implementation
    //-----------------------------

    /**
     * Returns the coverage name
     *
     * @param current method invocation
     * @returns coverage name
     */
    std::string CoverageI::coverageName(const Ice::Current& current) const
    {
      return mC_CoverageName;
    }

    /**
     * The number of sample dimensions in the coverage. For grid coverages, a
     * sample dimension is a band.
     *
     * @param current method invocation
     * @returns number of SampleDimensions
     */
    Ice::Long CoverageI::numSampleDimensions (const Ice::Current& current) const
    {
      return mCp1_SampleDimensions.size();
    }

    /**
     * The names of each dimension in the coverage. Typically these names are
     * "x", "y", "z" and "t".
     *
     * @param current method invocation
     * @returns vector of dimension
     */
    ::cv::StringSeq CoverageI::dimensionNames(const Ice::Current& current) const
    {
      return mC1_DimensionNames;
    }

    /**
     * Returns the SampleDimension <ai_index>
     *
     * @param current method invocation
     * @param ai_index index of SampleDimension
     * @returns handle to specified SampleDimension
     */
    cv::CvSampleDimensionPrx
    CoverageI::getSampleDimension(::Ice::Long ai_index,
				  const Ice::Current& current)
    {
      if (ai_index < 0 || ai_index >= mCp1_SampleDimensions.size())
	throw cv::CvInvalidIndex();

      cv::CvSampleDimensionPrx lCp_SampleDimensionPrx =
	cv::CvSampleDimensionPrx::uncheckedCast(current.adapter
						->addWithUUID(mCp1_SampleDimensions[ai_index].get()));

      return lCp_SampleDimensionPrx;
    }

    /**
     * Returns the SampleDimension implementation <ai_index>
     *
     * @param ai_index
     * @param current method invocation
     * @returns handle to specified SampleDimension implementation
     */
    SampleDimensionIPtr
    CoverageI::getSampleDimensionI( int index )
    {
      if (index < 0 || index >= mCp1_SampleDimensions.size())
	return SampleDimensionIPtr();
      
      return mCp1_SampleDimensions[index];
    }

    /**
     * Sets the dimension names
     *
     * @param aCr_names vector of dimension names
     */
    void CoverageI::dimensionNames ( const std::vector<std::string>&
				     aC1r_names )
    {
      mC1_DimensionNames = aC1r_names;
    }

    /**
     * Number of grid coverages which the grid coverage was derived from.
     *
     * @param current method invocation
     * @returns number of grid coverage sources
     */
    Ice::Int
    CoverageI::numSources (const Ice::Current& current) const
    {
      return 1;			// default: a single grid coverage source
    }

    /**
     * List of metadata keywords for a coverage.
     *
     * @param current method invocation
     * @returns vector of metadata names
     */
    ::cv::StringSeq
    CoverageI::metadataNames(const Ice::Current& current)
    {
      return getMetadataNames((GDALMajorObject*)mCp_dataset.get());
    }

    /**
     * Returns the metadata value <name>
     *
     * @param current method invocation
     * @returns value of metadata <name>
     */
    std::string
    CoverageI::getMetadataValue(const std::string& lC_name,
				const Ice::Current& current)
    {
      return mCp_dataset->GetMetadataItem(lC_name.c_str());
    } // SampleDimensionI::getMetadataName()

    /**
     * This specifies the coordinate system used when accessing a coverage or
     * grid coverage with the "evaluate" methods.
     *
     * @param current method invocation
     * @returns coordinate system
     */
    ::cs::CsCoordinateSystemPrx
    CoverageI::coordinateSystem(const Ice::Current& current)
    {
      return 0;
    }

    /**
     * Returns the envelope of the coverage extent.
     *
     * @param current method invocation
     * @returns envelope
     */
    ::pt::PtEnvelope
    CoverageI::envelope(const Ice::Current& current)
    {
      // get grid dimensions
      int li_nZSize = mCp_dataset->GetRasterCount();
      int li_nYSize = mCp_dataset->GetRasterYSize();
      int li_nXSize = mCp_dataset->GetRasterXSize();

      double ld1_GeoTransform[6];
      mCp_dataset->GetGeoTransform(ld1_GeoTransform);

      // grid origin is the upper left corner
      double ld_minX = ld1_GeoTransform[0];
      double ld_maxY = ld1_GeoTransform[3];

      // grid resolution
      double ld_dx = ld1_GeoTransform[1];
      double ld_dy = ld1_GeoTransform[5];

      double ld_maxX = ld_minX + ( ld_dx * ((double)li_nXSize) );
      double ld_minY = ld_maxY + ( ld_dy * ((double)li_nYSize) );

      // create a new pt::PT_Envelope object
      pt::PtEnvelope lC_PtEnvelope;

      // note: coordinates (default: 2D) are row, col (i.e. y, x)
      ::Ice::Long dim = 2;
      lC_PtEnvelope.minCP.ord.resize(dim);
      lC_PtEnvelope.maxCP.ord.resize(dim);

      // copy envelope state
      lC_PtEnvelope.minCP.ord[0] = ld_minX;
      lC_PtEnvelope.minCP.ord[1] = ld_minY;
      lC_PtEnvelope.maxCP.ord[0] = ld_maxX;
      lC_PtEnvelope.maxCP.ord[1] = ld_maxY;

      return lC_PtEnvelope; // return envelope

    } // CoverageI::envelope(...)

    /**
     * Return the value vector for a given point in the coverage.
     *
     * @param aCr_point Point at which to find the grid values
     * @param current method invocation
     * @returns value vector for a given point in the coverage
     */
    ::cv::BooleanSeq
    CoverageI::evaluateAsBoolean(const ::pt::PtCoordinatePoint& aCr_point,
				 const Ice::Current& current)
    {
      return this->evaluate<bool>(aCr_point);
    }

    /**
     * Return the value vector for a given point in the coverage.
     *
     * @param aCr_point Point at which to find the grid values
     * @param current method invocation
     * @returns value vector for a given point in the coverage
     */
    ::cv::ByteSeq 
    CoverageI::evaluateAsByte(const ::pt::PtCoordinatePoint& aCr_point,
			      const Ice::Current& current)
    {
      return this->evaluate<Ice::Byte>(aCr_point);
    }

    /**
     * Return the value vector for a given point in the coverage.
     *
     * @param aCr_point Point at which to find the grid values
     * @param current method invocation
     * @returns value vector for a given point in the coverage
     */
    ::cv::IntSeq
    CoverageI::evaluateAsInt(const ::pt::PtCoordinatePoint& aCr_point,
					 const Ice::Current& current)
    {
      return this->evaluate<int>(aCr_point);
    }

    /**
     * Return the value vector for a given point in the coverage.
     *
     * @param aCr_point Point at which to find the grid values
     * @param current method invocation
     * @returns value vector for a given point in the coverage
     */
    ::cv::DoubleSeq
    CoverageI::evaluateAsDouble(const ::pt::PtCoordinatePoint& aCr_point,
				const Ice::Current& current)
    {
      return this->evaluate<double>(aCr_point);
    }

  } // end of namespace impl

} // end of namespace ogis

