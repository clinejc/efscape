// __COPYRIGHT_START__
// __COPYRIGHT_END__
//
// Taken from the PDF document _OpenGIS(R) Implementation Specification:
// Coordinate Transformation Services_ Revision 1.0, dated October 19, 2000
//
// OpenGIS Project Document 01-009<br>
//
// PT package, CORBA profile.<br>
// Copyright (c) OpenGIS Consortium Thursday, October 19, 2000.
//
// Minor modifications made to restore readability after extraction
// from the PDF document (including the insertation of comments).
//
// ChangeLog:
//   - 2009.06.02 cleaned up for slice2html
//   - 2007.07.04 Converted from CORBA idl to Slice
//     - removed underscores
//     - removed typedefs
//   - 2006.02.04 initial revision by Jon Cline
//

#ifndef _OGC_PT_IDL_
#define _OGC_PT_IDL_

/**
 * Coordinate Transformation Services: Positioning (PT)<br>
 * Based on OpenGIS Project Document 01-009 (October 19, 2000) implementation
 * specification for the PT package, CORBA profile.
 */
module pt {

  sequence<double> DoubleSeq;
  sequence<DoubleSeq> DoubleSeqSeq;

  /**
   * A position defined by a list of numbers.
   */
  struct PtCoordinatePoint {
    DoubleSeq ord;	  /** The ordinates of the coordinate point */
  };

  /**
   * A box defined by two positions.
   */
  struct PtEnvelope {
    PtCoordinatePoint minCP; /** Point containing minimum ordinate values */
    PtCoordinatePoint maxCP; /** Point containing maximum ordinate values */
  };

  /**
   * A two dimensional array of numbers.
   */
  struct PtMatrix {
    /**
     * Elements of the matrix. The elements should be stored in a rectangular
     * two dimensional array
     */
    DoubleSeqSeq elt; 		
  }; 
};

#endif  // _OGC_PT_IDL_
