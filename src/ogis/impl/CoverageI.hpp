// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CoverageI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_COVERAGE_I_HPP
#define OGIS_IMPL_COVERAGE_I_HPP

#include <ogis/ogc_cv.h>

#include <Ice/Ice.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

// forward declarations
class GDALDataset;

namespace ogis {

  namespace impl {

    class SampleDimensionI;

    typedef boost::shared_ptr<SampleDimensionI> SampleDimensionIPtr;

    /**
     * Implements the cv::CV_Coverage interface. Wraps a GDALDataset, which
     * provides the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 4.01 created 10 Oct 2000 revised 28 May 2009
     *
     * ChangeLog:
     *   - 2009.05.28 Changed mutator method name
     *       - from "void coverage_name(const char*)"
     *       - to "void coverageName(const char*)"
     *   - 2009.04.16 Moved back to ogis::impl
     *   - 2009.04.14 Updated to support expanded Coverage ICE interface
     *   - 2007.09.04 Class name changed to CoverageI
     *   - 2007.09.04 Moved from ogis::impl to efscape::ogis
     *   - 2007.09.03 Converted from CORBA to ICE.
     */
    class CoverageI : virtual public cv::CvCoverage
    {

    public:
 
      CoverageI (GDALDataset* aCp_dataset);
      CoverageI (const boost::shared_ptr<GDALDataset>& aCp_dataset);
      virtual ~CoverageI ();

      //
      // ICE services
      //
      virtual std::string coverageName(const Ice::Current&) const;

      virtual ::Ice::Long numSampleDimensions(const Ice::Current&) const;

      virtual ::cv::StringSeq dimensionNames(const Ice::Current&) const;

      virtual ::Ice::Int numSources(const Ice::Current&) const;

      virtual ::cv::StringSeq metadataNames(const Ice::Current&);

      virtual ::std::string getMetadataValue(const ::std::string&,
					     const Ice::Current&);

      virtual ::pt::PtEnvelope envelope(const Ice::Current&);

      virtual ::cs::CsCoordinateSystemPrx coordinateSystem(const Ice::Current&);

      virtual ::cv::CvSampleDimensionPrx
      getSampleDimension(::Ice::Long, const Ice::Current&);

      virtual ::cv::BooleanSeq evaluateAsBoolean(const ::pt::PtCoordinatePoint&,
						 const Ice::Current&);

      virtual ::cv::ByteSeq evaluateAsByte(const ::pt::PtCoordinatePoint&,
					   const Ice::Current&);

      virtual ::cv::IntSeq evaluateAsInt(const ::pt::PtCoordinatePoint&,
					 const Ice::Current&);

      virtual ::cv::DoubleSeq evaluateAsDouble(const ::pt::PtCoordinatePoint&,
					       const Ice::Current&);

      //------------------------
      // local (non-ICE) methods
      //------------------------

      /** @returns coverage name */
      const char* coverageName() const { return mC_CoverageName.c_str(); }

      /**
       * Sets the coverage name
       *
       * @param acp_name new coverage name
       */
      void coverageName( const char* acp_name ) {
	mC_CoverageName = acp_name;
      }

      void dimensionNames( const std::vector<std::string>& aC1r_names );
      SampleDimensionIPtr getSampleDimensionI( int ai_index );

    protected:

      template <typename Type>
      std::vector<Type> evaluate(const ::pt::PtCoordinatePoint&);

      /** handle to GDAL dataset (used for implementation) */
      boost::shared_ptr<GDALDataset> mCp_dataset;

      /** vector of dimension names */
      std::vector<std::string> mC1_DimensionNames;

      /** vector of sample dimension */
      std::vector<SampleDimensionIPtr> mCp1_SampleDimensions;

    private:

      CoverageI();

      std::string mC_CoverageName;

    };

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_COVERAGE_I_HPP
