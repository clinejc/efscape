//
// ogc_gc.idl
//
// Taken from the PDF document _OpenGIS(R) Implementation Specification:
// Grid Coverage_ Revision 1.00, dated January 12, 2001
//
// OpenGIS Project Document 01-004
//
// Minor modifications made to restore readability after extraction
// from the PDF document.
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
// ChangeLog:
//   - 2009.05.31 Addded 'idempotent' and ["cpp:const"] keywords
//   - 2009.04.12 Added missing methods from specification
//     - interface GcGridCoverage
//       - string getMetadataValue
//       - Consolidated [get/set]DatablockAs[short/long|float/double]
//     - interface GcGridCoverageExchange
//       - string getMetadataValue
//   - 2007.07.04 Converted from CORBA idl to Slice
//     - removed underscores
//     - removed typedefs
//   - 2006.02.04 initial revision by Jon Cline
//

#ifndef _OGC_GC_IDL_
#define _OGC_GC_IDL_

#include <ogis/ogc_cv.ice>
#include <ogis/ogc_ct.ice>

/**
 * Grid Coverage: GcGridCoverage (GC) package<br>
 * Based on OpenGIS Project Document 01-004 (January 12, 2001) implementation
 * specification for Grid Coverage.
 */
module gc {

  struct range {
    long min;
    long max;
  };
  sequence<range> GcGridRange;

  struct GcGridGeometry {
    GcGridRange gridRange;
    ::ct::CtMathTransform gridToCoordinateSystem;
  };

  // These typedefs support the implementation
  sequence<bool> BooleanSeq;
  sequence<byte> ByteSeq;
  sequence<int> IntSeq;
  sequence<double> DoubleSeq;

  sequence<string> StringSeq;

  //---------------------------------------------------------------------------
  // interface GcGridCoverage : CvCoverage
  //---------------------------------------------------------------------------
  exception InvalidRange {};

  /**
   * Represents the basic implementation which provides access to grid coverage
   * data.
   */
  interface GcGridCoverage extends cv::CvCoverage {

    idempotent GcGridGeometry gridGeometry();

    //------------------------------------------------
    // grid services from open gis grid implementation
    //------------------------------------------------

    // methods for transferring grid data en masse
    idempotent
    void getDataBlockAsBoolean( GcGridRange range,
				out BooleanSeq data ) throws InvalidRange;
    idempotent
    void getDataBlockAsByte( GcGridRange range,
			     out ByteSeq data ) throws InvalidRange;
    idempotent
    void getDataBlockAsInt( GcGridRange range,
			    out IntSeq data ) throws InvalidRange;
    idempotent
    void getDataBlockAsDouble( GcGridRange range,
			       out DoubleSeq data ) throws InvalidRange;

    idempotent
    void setDataBlockAsBoolean( GcGridRange range,
				BooleanSeq data ) throws InvalidRange;
    idempotent
    void setDataBlockAsByte( GcGridRange range,
			     ByteSeq data ) throws InvalidRange;
    idempotent
    void setDataBlockAsInt( GcGridRange range,
			    IntSeq data ) throws InvalidRange;
    idempotent
    void setDataBlockAsDouble( GcGridRange range,
			       DoubleSeq data ) throws InvalidRange;
  };

  //---------------------------------------------------------------------------
  // interface GcGridCoverageExchange
  //---------------------------------------------------------------------------
  exception GcInvalidIndex {};
  exception GcCannotCreateGridCoverage {};

  struct GcFormat {
    string name;
    string description;
    string extension;
  };

  /**
   * Provides support for creation of grid coverages from persistent formats
   * as well as exporting a grid coverage to a persistent formats.
   */
  interface GcGridCoverageExchange {

    idempotent long numFormats();
    idempotent StringSeq metadataNames();
    idempotent string getMetadataValue(string name);
    idempotent
    GcFormat getFormat(long index ) throws GcInvalidIndex;

    idempotent StringSeq listSubNames( string name );

    idempotent
    GcGridCoverage
    createFromName(string name) throws GcCannotCreateGridCoverage;

    void exportTo(GcGridCoverage grid, string fileFormat, string fileName);
  };

};

#endif  // _OGC_GC_IDL_
