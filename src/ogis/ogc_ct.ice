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

#ifndef _OGC_CT_IDL_
#define _OGC_CT_IDL_

#include <ogis/ogc_pt.ice>
#include <ogis/ogc_cs.ice> 

/**
 * Coordinate Transformation Services: Positioning: Coordinate Transformations
 * (CT)<br>
 * Based on OpenGIS Project Document 01-009 (October 19, 2000) implementation
 * specification for the CT package, CORBA profile.
 */
module ct {
  interface CtMathTransform; 
  interface CtCoordinateTransformationAuthorityFactory; 
  interface CtCoordinateTransformationFactory; 
  interface CtCoordinateTransformation; 
  interface CtMathTransformFactory; 
//   typedef long CT_DomainFlags; 
  const long /* CT_DomainFlags */ CtDfInside=1; 
  const long /* CT_DomainFlags */ CtDfOutside=2; 
  const long /* CT_DomainFlags */ CtDfDiscontinuous=4; 
//   typedef long CT_TransformType; 
  const long /* CT_TransformType */ CtTtOther=0; 
  const long /* CT_TransformType */ CtTtConversion=1; 
  const long /* CT_TransformType */ CtTtTransformation=2; 
  const long /* CT_TransformType */ CtTtConversionAndTransformation=3; 
  struct CtParameter { 
    string name; 
    double value; 
  }; 
  /* typedef */ sequence<double> DoubleSeq; 
  /* typedef */ sequence<CtParameter> CtParameterSeq; 
  interface CtMathTransform 
  { 
    /* readonly attribute */ string WKT(); 
    /* readonly attribute */ string XML(); 
    /* readonly attribute */ long dimSource(); 
    /* readonly attribute */ long dimTarget(); 
    /* readonly attribute */ bool identity(); 
    long /* CT_DomainFlags */ getDomainFlags(DoubleSeq ord); 
    DoubleSeq getCodomainConvexHull(DoubleSeq ord); 
    pt::PtCoordinatePoint transform(pt::PtCoordinatePoint cp); 
    DoubleSeq transformList(DoubleSeq ord); 
    pt::PtMatrix derivative(pt::PtCoordinatePoint cp); 
    CtMathTransform inverse(); 
  }; 
  interface CtCoordinateTransformationAuthorityFactory 
  { 
    /* readonly attribute */ string authority(); 
    CtCoordinateTransformation createFromTransformationCode(string code); 
    CtCoordinateTransformation

    createFromCoordinateSystemCodes(string sourceCode,
				    string targetCode); 
  }; 
  interface CtCoordinateTransformationFactory 
  { 
    CtCoordinateTransformation
    createFromCoordinateSystems(cs::CsCoordinateSystem sourceCS,
				cs::CsCoordinateSystem targetCS); 
  }; 
  interface CtCoordinateTransformation 
  { 
    /* readonly attribute */ string name(); 
    /* readonly attribute */ string authority(); 
    /* readonly attribute */ string authorityCode(); 
    /* readonly attribute */ string remarks(); 
    /* readonly attribute */ string areaOfUse(); 
    /* readonly attribute */ long /* CT_TransformType */ transformType(); 
    /* readonly attribute */ cs::CsCoordinateSystem sourceCS(); 
    /* readonly attribute */ cs::CsCoordinateSystem targetCS(); 
    /* readonly attribute */ CtMathTransform mathTransform();
  }; 
  interface CtMathTransformFactory 
  { 
    CtMathTransform createAffineTransform(pt::PtMatrix matrix); 

    CtMathTransform
    createConcatenatedTransform(CtMathTransform transform1,
				CtMathTransform transform2); 

    CtMathTransform
    createPassThroughTransform(long firstAffectedOrdinate,
			       CtMathTransform subTransform); 

    CtMathTransform
    createParameterizedTransform(string classification,
				 CtParameterSeq parameters); 

    CtMathTransform createFromWKT(string wellKnownText); 
    CtMathTransform createFromXML(string xml); 
    bool isParameterAngular(string parameterName); 
    bool isParameterLinear(string parameterName); 
  }; 
};

#endif  // _OGC_CT_IDL_
