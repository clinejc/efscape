// __COPYRIGHT_START__
// __COPYRIGHT_END__
//
// Taken from the PDF document _OpenGIS(R) Implementation Specification:
// Grid Coverage_ Revision 1.00, dated January 12, 2001
// 
// OpenGIS Project Document 01-004
// 
// The aforementioned specification contains the following copyright
// claims:
// 
// Copyright © 2000, 2001 PCI Geomatics, Inc.
// Copyright © 2000, 2001 CadcorpLtd.
// Copyright © 2000, 2001 Oracle Corporation, Inc.
// Copyright © 2000, 2001 Environmental Systems Research Institute (ESRI)
// Copyright © 2000, 2001 Intergraph Corporation
// Copyright © 2000, 2001 Laser-Scan, Ltd.
// 
// See the aforementioned specification for disclaimers and more
// information.
//
// Minor modifications made to restore readability after extraction
// from the PDF document.
//
// ChangeLog:
//   - 2009.05.31 Addded 'idempotent' and ["cpp:const"] keywords
//   - 2009.04.12 Added missing methods from specification
//     - interface CvSampleDimension
//       - string getMetadataValue(string)
//     - interface CvCoverage
//       - cs::CsUnit unit() [include "ogc_cs"]
//       - double valueOffset()
//       - double valueScale()
//       - string getMetadataValue(string)
//       - <Type>Seq evaluateAs<Type>(pt::PtCoordinatePoint point)
//   - 2007.07.04 Converted from CORBA idl to Slice
//     - removed underscores
//     - removed typedefs
//   - 2006.02.04 initial revision by Jon Cline
//

#ifndef _OGC_CV_IDL_
#define _OGC_CV_IDL_

#include <ogis/ogc_pt.ice>
#include <ogis/ogc_cs.ice>

/**
 * Grid Coverage: CvCoverage (CV) package<br>
 * Based on OpenGIS Project Document 01-004 (January 12, 2001) implementation
 * specification for Grid Coverage.
 */
module cv {

  // supporting data structure for coverages
  /**
   * A code value indicating grid value data type. This will also indicate the
   * number of bits for the data type.
   */
  enum CvSampleDimensionType {
    Cv1Bit,			/** 1 bit grid value */
    Cv8Bit,			/** byte value */
    Cv16Bit,			/** 16 bit integer value */
    Cv32Bit,			/** 32 bit integer value */
    Cv32BitReal,		/** 32 bit real value */
    Cv64BitReal			/** 64 bit real value */
  };

  /*
   * Indicates the type of color palette entry for sample dimensions which have
   * a palette.
   */
  enum CvPaletteInterpretation {
    CvGray,
    CvRGB,
    CvCMYK,
    CvHLS
  };

  /**
   * Specifies mapping of a band to a color model component.
   */
  enum CvColorInterpretation {
    CvUndefined, /** Band is not associated with a color model component */
    CvGrayIndex, /** Band is an index into a lookup table */
    CvPaletteIndex,    /** Band is a color index into a color table */
    CvRedBand, /** Bands correspond to RGB color model components (Alpha optional) */
    CvGreenBand,
    CvBlueBand,
    CvAlphaBand,
    CvHueBand,		    /** Bands correspond to HSL color model */
    CvSaturationBand,
    CvLightnessBand,
    CvCyanBand,		   /** Bands correspond to CMYK color model */
    CvMagentaBand,
    CvYellowBand,
    CvBlackBand
  };

  // These typedefs support the implementation
  sequence<bool> BooleanSeq;
  sequence<byte> ByteSeq;
  sequence<short> ShortSeq;
  sequence<int> IntSeq;
  sequence<double> DoubleSeq;

  sequence<ShortSeq> ShortSeqSeq;
  sequence< string > StringSeq;

  /**
   * Describes the data values for a coverage.
   */
  interface CvSampleDimension {

    /**
     * Returns the SampleDimension type
     *
     * @returns SampleDimension type
     */
    idempotent CvSampleDimensionType sampleDimensionType();

    /**
     * Returns the vector of SampleDimension category names associated with
     * this SampleDimension.
     *
     * @returns vector of category names
     */
    idempotent StringSeq categoryNames();

    /**
     * Returns the description of SampleDimension.
     *
     * @returns string containing description of SampleDimension
     */
    idempotent string description();

    /**
     * Returns the color interpretation.
     *
     * @returns the color interpretation associated with this SampleDimension
     */
    idempotent CvColorInterpretation colorInterpretation();

    /**
     * Returns the palette interpretation.
     *
     * @returns the palette interpretation associated with the SampleDimension
     */
    idempotent CvPaletteInterpretation paletteInterpretation();

    /**
     * Returns the palette.
     *
     * @returns the palette associated with the SampleDimension
     */
    idempotent ShortSeqSeq palette();

    /**
     * Returns the minimum value.
     *
     * @returns the minimum value
     */
    idempotent double minimumValue();

    /**
     * Returns the maximum value.
     *
     * @returns the maximum values
     */
    idempotent double maximumValue();

    /**
     * Returns the no data value.
     *
     * @returns no data value
     */
    idempotent double noDataValue();

    /**
     * The unit information for this sample dimension. This interface typically
     * is provided with grid coverages which represent digital elevation data.
     * Note: description borrowed from the specification.
     *
     * @returns units information
     */
    idempotent cs::CsUnit units();

    /**
     * Offset is the value to add to grid values for this sample dimension.
     * This attribute is typically used when the sample dimension represents
     * elevation data. The default for this value is 0.
     *
     * @returns value offset
     */
    idempotent double valueOffset();

    /**
     * Scale is the value which is multiplied to grid values for this sample
     * dimension. This attribute is typically used when the sample dimension
     * represents elevation data. The default for this value is 1
     *
     * @returns value scale
     */
    idempotent double valueScale();

    /**
     * The list of metadata keywords for a sample dimension.
     *
     * @returns vector of metadata names
     */
    idempotent StringSeq metadataNames();

    /**
     * Retrieve the metadata value for a given metadata name.
     *
     * @returns value of metadata <name>
     */
    idempotent string getMetadataValue(string name);
  };
  sequence<CvSampleDimension*> SampleDimensionSeq;

  //---------------------------------------------------------------------------
  // interface Coverage
  //---------------------------------------------------------------------------
  exception CvPointOutsideCoverage {};
  exception CvInvalidIndex {};
  /**
   * Abstract interface providing access to an OpenGIS coverage.<br>
   * <br>
   * The essential property of coverage is to be able to generate a value for
   * any point within its domain. 
   */
  interface CvCoverage {
    /**
     * Name of coverage.
     *
     * @returns name of coverage
     */
    ["cpp:const"] idempotent string coverageName();

    /**
     * The number of sample dimensions in the coverage. For grid coverages, a
     * sample dimension is a band.
     *
     * @returns number of SampleDimensions
     */
    ["cpp:const"] idempotent long numSampleDimensions();

    /**
     * The names of each dimension in the coverage. Typically these names are
     * "x", "y", "z" and "t".
     *
     * @returns vector of dimension
     */
    ["cpp:const"] idempotent StringSeq dimensionNames();

    /**
     * Number of grid coverages which the grid coverage was derived from.
     *
     * @returns number of grid coverage sources
     */
    ["cpp:const"] idempotent int numSources();

    /**
     * List of metadata keywords for a coverage.
     *
     * @param current method invocation
     * @returns vector of metadata names
     */
    idempotent StringSeq metadataNames();

    /**
     * This specifies the coordinate system used when accessing a coverage or
     * grid coverage with the "evaluate" methods.
     *
     * @returns coordinate system
     */
    idempotent ::cs::CsCoordinateSystem* coordinateSystem();

    idempotent pt::PtEnvelope envelope();

    idempotent string getMetadataValue(string name);
    idempotent
    CvSampleDimension* getSampleDimension(long
					  index) throws CvInvalidIndex;
    idempotent
    BooleanSeq evaluateAsBoolean(pt::PtCoordinatePoint coord);
    idempotent
    ByteSeq evaluateAsByte(pt::PtCoordinatePoint coord);
    idempotent
    IntSeq evaluateAsInt(pt::PtCoordinatePoint coord);
    idempotent
    DoubleSeq evaluateAsDouble(pt::PtCoordinatePoint coord);
  };

};

#endif  // _OGC_CV_IDL_
