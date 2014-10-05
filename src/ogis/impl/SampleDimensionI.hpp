// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SampleDimensionI.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_SAMPLEDIMENSION_I_HPP
#define OGIS_IMPL_SAMPLEDIMENSION_I_HPP

#include <ogis/ogc_cv.h>

#include <boost/shared_ptr.hpp>
#include <string>

// forward declarations
class GDALRasterBand;
class GDALMajorObject;

namespace ogis {

  namespace impl {

    /**
     * Implements the cv::CvSampleDimension interface. Wraps a GDALRasterBand,
     * which provides the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 5.00 created 10 Oct 2000 revised 31 May 2009
     *
     * ChangeLog:
     *   - 2009.04.16 Moved back to ogis::impl
     *   - 2009.04.12 Update ICE interface and corresponding implementation
     *   - 2007.09.04 Class name changed to SampleDimensionI
     *   - 2007.09.04 Moved from ogis::impl to efscape::ogis
     *   - 2007.09.03 Converted from CORBA to ICE.
     */
    class SampleDimensionI : virtual public ::cv::CvSampleDimension
    {

    public:

      SampleDimensionI(GDALRasterBand* aCp_RasterBand);

      virtual ~SampleDimensionI();

      //
      // ICE methods
      //
      virtual
      ::cv::CvSampleDimensionType sampleDimensionType(const Ice::Current&);

      virtual std::string description(const Ice::Current&);

      virtual ::cv::StringSeq categoryNames(const Ice::Current&);

      virtual
      ::cv::CvColorInterpretation colorInterpretation(const Ice::Current&);

      virtual
      ::cv::CvPaletteInterpretation paletteInterpretation(const Ice::Current&);

      virtual ::cv::ShortSeqSeq palette(const Ice::Current&);

      virtual ::Ice::Double minimumValue(const Ice::Current&);

      virtual ::Ice::Double maximumValue(const Ice::Current&);

      virtual ::Ice::Double noDataValue(const Ice::Current&);

      virtual ::cs::CsUnitPtr units(const Ice::Current&);

      virtual ::Ice::Double valueOffset(const Ice::Current&);

      virtual ::Ice::Double valueScale(const Ice::Current&);

      virtual ::cv::StringSeq metadataNames(const Ice::Current&);

      virtual ::std::string getMetadataValue(const ::std::string&,
					     const Ice::Current&);
    protected:

      // constructors (not for public consumption)
      SampleDimensionI();
      SampleDimensionI(const SampleDimensionI& aCr_other);

      /** handle to raster band */
      GDALRasterBand* mCp_RasterBand;
    };

    // typedefs
    typedef boost::shared_ptr<SampleDimensionI> SampleDimensionIPtr;

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_SAMPLEDIMENSION_I_HPP
