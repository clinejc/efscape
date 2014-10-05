// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SampleDimensionI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <ogis/impl/SampleDimensionI.hpp>

#include <gdal_priv.h>		// gdal is used to implement sample dimension
#include <ogis/impl/ogis_utils.hpp>
#include <vector>

// mapping between GDALDatatype and cv::CV_SampleDimensionType
const cv::CvSampleDimensionType ge1_DatatypeMap[] =
  {
    cv::Cv1Bit,		// GDT_Unknown = 0 (invalid mapping)
    cv::Cv8Bit,		// GDT_Byte = 1
    cv::Cv16Bit,		// GDT_UInt16 = 2
    cv::Cv16Bit,		// GDT_Int16 = 3
    cv::Cv32Bit,		// GDT_UInt32 = 4
    cv::Cv32Bit,		// GDT_Int32 = 5
    cv::Cv32BitReal,		// GDT_Float32 = 6
    cv::Cv64BitReal,		// GDT_Float64 = 7
  };

namespace ogis {

  namespace impl {

    /** constructor */
    SampleDimensionI::SampleDimensionI() :
      mCp_RasterBand(0)
    {}

    /**
     * constructor
     *
     * @param aCp_RasterBand GDAL raster band
     */
    SampleDimensionI::SampleDimensionI(GDALRasterBand* aCp_RasterBand) :
      mCp_RasterBand(aCp_RasterBand)
    {}

    /** destructor */
    SampleDimensionI::~SampleDimensionI() {}

    /**
     * Returns the description of SampleDimension (to be implemented).
     *
     * @param current method invocation
     * @returns string containing description of SampleDimension
     */
    std::string
    SampleDimensionI::description(const Ice::Current& current)
    {
      return mCp_RasterBand->GetDescription();
    }

    /**
     * Returns the SampleDimension type
     *
     * @param current method invocation
     * @returns SampleDimension type
     */
    ::cv::CvSampleDimensionType
    SampleDimensionI::sampleDimensionType(const Ice::Current& current)
    {
      return ge1_DatatypeMap[(int)mCp_RasterBand->GetRasterDataType()];
    }

    /**
     * Returns the vector of SampleDimension category names associated with
     * this SampleDimension.
     *
     * @param current method invocation
     * @returns vector of category names
     */
    ::cv::StringSeq
    SampleDimensionI::categoryNames(const Ice::Current& current)
    {
      // get the GDALRasterBand category list
      char **papszCategoryNames = mCp_RasterBand->GetCategoryNames();

      // copy the category names to vector of strings
      ::cv::StringSeq lC1_CategoryNames;
      int li_length = CSLCount(papszCategoryNames);
      for (int i = 0; i < li_length; i++)
	lC1_CategoryNames.push_back(CSLGetField(papszCategoryNames,i));

      return lC1_CategoryNames;

    } // SampleDimensionI::categoryNames()

    /**
     * Returns the color interpretation
     *
     * @param current method invocation
     * @returns the color interpretation associated with this SampleDimension
     */
    ::cv::CvColorInterpretation
    SampleDimensionI::colorInterpretation(const Ice::Current& current)
    {
      GDALColorInterp le_ColorInterp =
	mCp_RasterBand->GetColorInterpretation();

      switch (le_ColorInterp) {

      case GCI_GrayIndex:	// Greyscale
	return cv::CvGrayIndex;

      case GCI_PaletteIndex: 	// Paletted (see associated color table)
	return cv::CvPaletteIndex;

      case GCI_RedBand:		// Red band of RGBA image
	return cv::CvRedBand;

      case GCI_GreenBand: 	// Green band of RGBA image
	return cv::CvGreenBand;

      case GCI_BlueBand: 	// Blue band of RGBA image
	return cv::CvBlueBand;

      case GCI_AlphaBand: 	// Alpha (0=transparent, 255=opaque)
	return cv::CvAlphaBand;

      case GCI_HueBand: 	// Hue band of HLS image
	return cv::CvHueBand;

      case GCI_SaturationBand: 	// Saturation band of HLS image
	return cv::CvSaturationBand;

      case GCI_LightnessBand: 	// Lightness band of HLS image
	return cv::CvLightnessBand;

      case GCI_CyanBand: 	// Cyan band of CMYK image
	return cv::CvCyanBand;

      case GCI_MagentaBand: 	// Magenta band of CMYK image
	return cv::CvMagentaBand;

      case GCI_YellowBand: 	// Yellow band of CMYK image
	return cv::CvYellowBand;

      case GCI_BlackBand: 	// Black band of CMLY image
	return cv::CvBlackBand;

      case GCI_YCbCr_YBand: 	// Y Luminance
	return cv::CvUndefined; // missing from cv interface

      case GCI_YCbCr_CbBand: 	// Cb Chroma
	return cv::CvUndefined; // missing from cv interface

      case GCI_YCbCr_CrBand: 	// Cr Chroma
	return cv::CvUndefined; // missing from cv interface

	//       case GCI_Max:		// Max current value (=16, bug: duplicate case)
	// 	return cv::CvUndefined; // missing from cv interface

      default:
	;
      }

      return cv::CvUndefined;

    } // SampleDimensionI::colorInterpretation()

    /**
     * Returns the palette interpretation.
     *
     * @param current method invocation
     * @returns the palette interpretation associated with the SampleDimension
     */
    ::cv::CvPaletteInterpretation
    SampleDimensionI::paletteInterpretation(const Ice::Current& current)
    {
      // get color table from GDAL raster band
      GDALColorTable* lCp_ColorTable = mCp_RasterBand->GetColorTable();

      // if the color table exists, get the palette interpretation info
      if (lCp_ColorTable != NULL) {
	GDALPaletteInterp le_PaletteInterp =
	  lCp_ColorTable->GetPaletteInterpretation();

	switch(le_PaletteInterp) {

	case GPI_Gray:
	  return cv::CvGray;

	case GPI_RGB:
	  return cv::CvRGB;

	case GPI_CMYK:
	  return cv::CvCMYK;

	case GPI_HLS:
	  return cv::CvHLS;

	default:		// should not happen
	  ;
	} // switch(le_PaletteInterp...

      }	// if (lCp_ColorTable...

      return cv::CvRGB;	// default palette interpretation

    } // SampleDimensionI::paletteInterpretation(...)

    /**
     * Returns the palette.
     *
     * @param current method invocation
     * @returns the palette associated with the SampleDimension
     */
    cv::ShortSeqSeq SampleDimensionI::palette(const Ice::Current& current)
    {
      // get color table from GDAL raster band
      GDALColorTable* lCp_ColorTable = mCp_RasterBand->GetColorTable();

      // create a 2-d array of shorts to return color table
      cv::ShortSeqSeq ls2_palette;

      if (lCp_ColorTable != NULL) {

	ls2_palette.resize(lCp_ColorTable->GetColorEntryCount());

	for (int i = 0; i < ls2_palette.size(); i++) {
	  const GDALColorEntry* lCp_Entry = lCp_ColorTable->GetColorEntry(i);
	  ls2_palette[i].resize(4);
	  ls2_palette[i][0] = lCp_Entry->c1;
	  ls2_palette[i][1] = lCp_Entry->c2;
	  ls2_palette[i][2] = lCp_Entry->c3;
	  ls2_palette[i][3] = lCp_Entry->c4;

	} // for (int i = 0...

      }	// if (lCp_ColorTable...

      return ls2_palette;

    } // SampleDimensionI::palette()

    // minimum, maximum, and no data values
    /**
     * Returns the minimum value.
     *
     * @param current method invocation
     * @returns the minimum value
     */
    ::Ice::Double
    SampleDimensionI::minimumValue(const Ice::Current& current)
    {
      return mCp_RasterBand->GetMinimum();
    }

    /**
     * Returns the maximum value.
     *
     * @param current method invocation
     * @returns the maximum values
     */
    ::Ice::Double
    SampleDimensionI::maximumValue(const Ice::Current& current)
    {
      return mCp_RasterBand->GetMaximum();
    }

    /**
     * Returns the no data value.
     *
     * @param current method invocation
     * @returns no data value
     */
    ::Ice::Double
    SampleDimensionI::noDataValue(const Ice::Current& current)
    {
      return mCp_RasterBand->GetNoDataValue();
    }

    /**
     * The unit information for this sample dimension. This interface typically
     * is provided with grid coverages which represent digital elevation data.
     * Note: description borrowed from the specification.
     *
     * @param current method invocation
     * @returns units information
     */
    ::cs::CsUnitPtr
    SampleDimensionI::units(const Ice::Current& current)
    {
      return ::cs::CsUnitPtr();
    }

    /**
     * Offset is the value to add to grid values for this sample dimension.
     * This attribute is typically used when the sample dimension represents
     * elevation data. The default for this value is 0.
     *
     * @param current method invocation
     * @returns value offset
     */
    ::Ice::Double
    SampleDimensionI::valueOffset(const Ice::Current& current)
    {
      return mCp_RasterBand->GetOffset();
    }

    /**
     * Scale is the value which is multiplied to grid values for this sample
     * dimension. This attribute is typically used when the sample dimension
     * represents elevation data. The default for this value is 1
     *
     * @param current method invocation
     * @returns value scale
     */
    ::Ice::Double
    SampleDimensionI::valueScale(const Ice::Current& current)
    {
      return mCp_RasterBand->GetScale();
    }

    /**
     * The list of metadata keywords for a sample dimension.
     *
     * @param current method invocation
     * @returns vector of metadata names
     */
    ::cv::StringSeq
    SampleDimensionI::metadataNames(const Ice::Current& current)
    {
      return getMetadataNames((GDALMajorObject*)this);
    } // SampleDimensionI::metadataNames()

    /**
     * Retrieve the metadata value for a given metadata name.
     *
     * @param current method invocation
     * @returns value of metadata <name>
     */
    std::string
    SampleDimensionI::getMetadataValue(const std::string& lC_name,
				       const Ice::Current& current)
    {
      return mCp_RasterBand->GetMetadataItem(lC_name.c_str());
    } // SampleDimensionI::getMetadataName()

  } // namespace impl

} // namespace ogis

