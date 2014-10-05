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

#ifndef _OGC_CS_IDL_
#define _OGC_CS_IDL_

#include <ogis/ogc_pt.ice> 

/**
 * Coordinate Transformation Services: Positioning: Coordinate Systems (CS)<br>
 * Based on OpenGIS Project Document 01-009 (October 19, 2000) implementation
 * specification for the CS package, CORBA profile.
 */
module cs { 
  interface CsInfo; 
  interface CsCoordinateSystem; 
  interface CsCompoundCoordinateSystem; 
  interface CsCoordinateSystemAuthorityFactory; 
  interface CsUnit; 
  interface CsDatum; 
  interface CsLinearUnit; 
  interface CsFittedCoordinateSystem; 
  interface CsEllipsoid; 
  interface CsHorizontalDatum; 
  interface CsHorizontalCoordinateSystem; 
  interface CsAngularUnit; 
  interface CsLocalDatum; 
  interface CsPrimeMeridian; 
  interface CsLocalCoordinateSystem; 
  interface CsGeographicCoordinateSystem; 
  interface CsProjection; 
  interface CsProjectedCoordinateSystem; 
  interface CsVerticalDatum; 
  interface CsGeocentricCoordinateSystem; 
  interface CsVerticalCoordinateSystem; 
  interface CsCoordinateSystemFactory; 
//   typedef long CsAxisOrientationEnum; 
  const long /* CsAxisOrientationEnum */ CsAoOther=0; 
  const long /* CsAxisOrientationEnum */ CsAoNorth=1; 
  const long /* CsAxisOrientationEnum */ CsAoSouth=2; 
  const long /* CsAxisOrientationEnum */ CsAoEast=3; 
  const long /* CsAxisOrientationEnum */ CsAoWest=4; 
  const long /* CsAxisOrientationEnum */ CsAoUp=5; 
  const long /* CsAxisOrientationEnum */ CsAoDown=6; 
//   typedef long CsDatumType; 
  const long /* CsDatumType */ CsHdMin=1000; 
  const long /* CsDatumType */ CsHdOther=1000; 
  const long /* CsDatumType */ CsHdClassic=1001; 
  const long /* CsDatumType */ CsHdGeocentric=1002; 
  const long /* CsDatumType */ CsHdMax=1999; 
  const long /* CsDatumType */ CsVdMin=2000; 
  const long /* CsDatumType */ CsVdOther=2000; 
  const long /* CsDatumType */ CsVdOrthometric=2001; 
  const long /* CsDatumType */ CsVdEllipsoidal=2002; 
  const long /* CsDatumType */ CsVdAltitudeBarometric=2003; 
  const long /* CsDatumType */ CsVdNormal=2004; 
  const long /* CsDatumType */ CsVdGeoidModelDerived=2005; 
  const long /* CsDatumType */ CsVdDepth=2006; 
  const long /* CsDatumType */ CsVdMax=2999;
  const long /* CsDatumType */ CsLdMin=10000;
  const long /* CsDatumType */ CsLdMax=32767; 
  struct CsAxisInfo { 
    long /* CsAxisOrientationEnum */ orientation; 
    string name; 
  }; 
  struct CsProjectionParameter { 
    string name; 
    double value; 
  };
  struct CsWGS84ConversionInfo { 
    double dx; 
    double dy;
    double dz; 
    double ex; 
    double ey; 
    double ez; 
    double ppm; 
    string areaOfUse; 
  }; 
  /* typedef */ sequence<CsAxisInfo> CsAxisInfoSeq; 
  /* typedef */ sequence<CsProjectionParameter> CsProjectionParameterSeq; 

  /**
   */
  interface CsInfo 
  { 
    /* readonly attribute */ string name(); 
    /* readonly attribute */ string authority(); 
    /* readonly attribute */ string authorityCode(); 
    /* readonly attribute */ string alias(); 
    /* readonly attribute */ string abbreviation(); 
    /* readonly attribute */ string remarks(); 
    /* readonly attribute */ string WKT(); 
    /* readonly attribute */ string XML(); 
  };
  interface CsCoordinateSystem extends CsInfo 
  { 
    /* readonly attribute */ long dimension(); 
    /* readonly attribute */ pt::PtEnvelope defaultEnvelope(); 
    CsAxisInfo getAxis(long dimension); 
    CsUnit getUnits(long dimension); 
  }; 
  interface CsCompoundCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsCoordinateSystem headCS(); 
    /* readonly attribute */ CsCoordinateSystem tailCS(); 
  }; 
  interface CsCoordinateSystemAuthorityFactory 
  { 
    /* readonly attribute */ string authority(); 
    CsProjectedCoordinateSystem createProjectedCoordinateSystem(string 
								 code); 
    CsGeographicCoordinateSystem createGeographicCoordinateSystem(string 
								   code); 
    CsHorizontalDatum createHorizontalDatum(string code); 
    CsEllipsoid createEllipsoid(string code); 
    CsPrimeMeridian createPrimeMeridian(string code); 
    CsLinearUnit createLinearUnit(string code); 
    CsAngularUnit createAngularUnit(string code); 
    CsVerticalDatum createVerticalDatum(string code); 
    CsVerticalCoordinateSystem createVerticalCoordinateSystem(string 
							       code); 
    CsCompoundCoordinateSystem createCompoundCoordinateSystem(string 
							       code); 
    CsHorizontalCoordinateSystem createHorizontalCoordinateSystem(string 
								   code); 
    string descriptionText(string code); 
    string geoidFromWKTName(string wkt); 
    string wktGeoidName(string geoid); 
  }; 
  interface CsUnit extends CsInfo 
  { 
  }; 
  interface CsDatum extends CsInfo 
  { 
    /* readonly attribute */ long /* CsDatumType */ datumType(); 
  }; 
  interface CsLinearUnit extends CsUnit 
  { 
    /* readonly attribute */ double metersPerUnit(); 
  }; 
  interface CsFittedCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsCoordinateSystem baseCoordinateSystem(); 
    /* readonly attribute */ string toBase(); 
  };
  interface CsEllipsoid extends CsInfo 
  { 
    /* readonly attribute */ double semiMajorAxis(); 
    /* readonly attribute */ double semiMinorAxis(); 
    /* readonly attribute */ double inverseFlattening(); 
    /* readonly attribute */ bool ivfDefinitive(); 
    /* readonly attribute */ CsLinearUnit axisUnit(); 
  }; 
  interface CsHorizontalDatum extends CsDatum 
  { 
    /* readonly attribute */ CsEllipsoid ellipsoid(); 
    /* readonly attribute */ CsWGS84ConversionInfo WGS84Parameters(); 
  }; 
  interface CsHorizontalCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsHorizontalDatum horizontalDatum(); 
  }; 
  interface CsAngularUnit extends CsUnit 
  { 
    /* readonly attribute */ double radiansPerUnit(); 
  }; 
  interface CsLocalDatum extends CsDatum 
  { 
  }; 
  interface CsPrimeMeridian extends CsInfo 
  { 
    /* readonly attribute */ double longitude(); 
    /* readonly attribute */ CsAngularUnit angularUnit(); 
  }; 
  interface CsLocalCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsLocalDatum localDatum(); 
  }; 
  interface CsGeographicCoordinateSystem extends CsHorizontalCoordinateSystem 
  { 
    /* readonly attribute */ CsAngularUnit angularUnit(); 
    /* readonly attribute */ CsPrimeMeridian primeMeridian(); 
    /* readonly attribute */ long numConversionToWGS84(); 
    CsWGS84ConversionInfo getWGS84ConversionInfo(long index); 
  }; 
  interface CsProjection extends CsInfo 
  { 
    /* readonly attribute */ long numParameters(); 
    /* readonly attribute */ string className(); 
    CsProjectionParameter getParameter(long index); 
  }; 
  interface CsProjectedCoordinateSystem extends CsHorizontalCoordinateSystem 
  { 
    /* readonly attribute */ CsGeographicCoordinateSystem 
    geographicCoordinateSystem(); 
    /* readonly attribute */ CsLinearUnit linearUnit(); 
    /* readonly attribute */ CsProjection projection(); 
  }; 
  interface CsVerticalDatum extends CsDatum 
  { 
  }; 
  interface CsGeocentricCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsHorizontalDatum horizontalDatum(); 
    /* readonly attribute */ CsLinearUnit linearUnit(); 
    /* readonly attribute */ CsPrimeMeridian primeMeridian(); 
  }; 
  interface CsVerticalCoordinateSystem extends CsCoordinateSystem 
  { 
    /* readonly attribute */ CsVerticalDatum verticalDatum(); 
    /* readonly attribute */ CsLinearUnit verticalUnit(); 
  }; 
  interface CsCoordinateSystemFactory 
  { 
    CsCoordinateSystem createFromXML(string xml); 
    CsCoordinateSystem createFromWKT(string wellKnownText);

    CsCompoundCoordinateSystem
    createCompoundCoordinateSystem(string 
				   name,
				   CsCoordinateSystem head,
				   CsCoordinateSystem tail); 

    CsFittedCoordinateSystem
    createFittedCoordinateSystem(string name,
				 CsCoordinateSystem base,
				 string toBaseWKT,
				 CsAxisInfoSeq arAxes); 

    CsLocalCoordinateSystem
    createLocalCoordinateSystem(string name,
				CsLocalDatum datum,
				CsUnit unit,
				CsAxisInfoSeq arAxes);
 
    CsEllipsoid
    createEllipsoid(string name,
		    double semiMajorAxis,
		    double semiMinorAxis,
		    CsLinearUnit linearUnit); 

    CsEllipsoid
    createFlattenedSphere(string name,
			  double semiMajorAxis,
			  double inverseFlattening,
			  CsLinearUnit linearUnit); 

    CsProjectedCoordinateSystem
    createProjectedCoordinateSystem(string name,
				    CsGeographicCoordinateSystem gcs,
				    CsProjection projection,
				    CsLinearUnit linearUnit,
				    CsAxisInfo axis0,
				    CsAxisInfo axis1); 

    CsProjection createProjection(string name,
				   string wktProjectionClass,
				   CsProjectionParameterSeq parameters); 

    CsHorizontalDatum
    createHorizontalDatum(string name,
			  long /* CsDatumType */ horizontalDatumType,
			  CsEllipsoid ellipsoid,
			  CsWGS84ConversionInfo toWGS84); 

    CsPrimeMeridian createPrimeMeridian(string name,
					 CsAngularUnit angularUnit,
					 double longitude); 

    CsGeographicCoordinateSystem
    createGeographicCoordinateSystem(string name,
				     CsAngularUnit angularUnit,
				     CsHorizontalDatum horizontalDatum,
				     CsPrimeMeridian primeMeridian,
				     CsAxisInfo axis0,
				     CsAxisInfo axis1); 

    CsLocalDatum createLocalDatum(string name,
				   long /* CsDatumType */ localDatumType); 

    CsVerticalDatum createVerticalDatum(string name,
					 long /* CsDatumType */ verticalDatumType); 

    CsVerticalCoordinateSystem
    createVerticalCoordinateSystem(string name,
				   CsVerticalDatum verticalDatum,
				   CsLinearUnit verticalUnit,
				   CsAxisInfo axis); 
  }; 
};

#endif  // _OGC_CS_IDL_
