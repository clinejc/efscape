//=============================================================================
// __COPYRIGHT_START__
// __COPYRIGHT_END__
//=============================================================================
#ifndef _OGC_SF_IDL_
#define _OGC_SF_IDL_

/**
 * Taken from the PDF document _OpenGIS(R) Simple Features
 * Specification For CORBA_, Revision 1.1, dated June 2, 1999
 * and subsequently modified by Chuck Taylor, September 20, 2005
 * (see file "ogis-simple.idl").<br>
 * <br>
 * Significant modifications (conversion from CORBA IDL into ZeroC
 * Internet Connection Engine (ICE) IDL) have been made in order to
 * support an implementation based on ICE and OGR. Additional interfaces
 * may be implemented as needed.<br>
 * <br>
 * The aforementioned specification contains the following copyright
 * claims:<br>
 * <br>
 * Copyright 1997, 1998, 1999 Bentley Systems, Inc.<br>
 * Copyright 1997, 1998, 1999 Environmental Systems Research Institute<br>
 * Copyright 1997, 1998, 1999 Genasys II, Inc.<br>
 * Copyright 1997, 1998, 1999 Oracle Corporation<br>
 * Copyright 1997, 1998, 1999 University of California, Los Angeles.<br>
 * Copyright 1997, 1998, 1999 Sun Microsystems, Inc.<br>
 * <br>
 * See the aforementioned specification for disclaimers and more
 * information.
 *
 * @author Jon C. Cline <clinej@stanfordalumni.org>
 * @version 0.03 created 16 Aug 2007, revised 31 May 2009
 *
 * ChangeLog:
 *   - 2009.05.31 Addded 'idempotent' and ["cpp:const"] keywords
 *   - 2007.08.19 Created stub interfaces for Feature, FeatureType, and Geometry
 *   - 2007.08.19 Converted WKSGeometry data structures.
 *   - 2007.08.16 Created ogc_sf.ice
 */
module ogis {

  //----------------------------------------------------------------------
  // Forward declarations of interfaces
  //----------------------------------------------------------------------
  interface Geometry;
  interface FeatureType;
  interface FeatureIterator;


  //----------------------------------------------------------------------
  // Common structures
  //----------------------------------------------------------------------
  sequence<FeatureType*> FeatureTypeSeq;


  //----------------------------------------------------------------------
  // Well-known Structures
  //----------------------------------------------------------------------

  struct WKSPoint {
    double x;
    double y;
    double z;			// added to support 2.5D extension
  };

  /* typedef */ sequence<WKSPoint> WKSPointSeq;
  /* typedef */ sequence<WKSPoint> WKSLineString;
  /* typedef */ sequence<WKSLineString> WKSLineStringSeq;
  /* typedef */ sequence<WKSPoint> WKSLinearRing;
  /* typedef */ sequence<WKSLinearRing> WKSLinearRingSeq;


  struct WKSLinearPolygon {
    WKSLinearRing externalBoundary;
    WKSLinearRingSeq internalBoundaries;
  };


  /* typedef */ sequence <WKSLinearPolygon> WKSLinearPolygonSeq;

  /**
   * List of well know structures for geometry types. It extends the original
   * Simple Feature CORBA specification to include additional types supported
   * by OGR. There is a 1-to-1 mapping with OGR OGRwkbGeometryType.
   */
  enum WKSType {
    WKSUnknown,			/** unkown type, non-standard (OGR) */
    WKSPointType,		/** 0-dimensional geometric object */
    WKSLineStringType,		/** 1-dimensional geometric object with linear interpolation between Points */
    WKSLinearPolygonType,	/** planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries */
    WKSMultiPointType,		/** GeometryCollection of Points */
    WKSMultiLineStringType,	/** GeometryCollection of LineStrings */
    WKSMultiLinearPolygonType,	/** GeometryCollection of Polygons */
    WKSCollectionType,		/** geometric object that is a collection of 1 or more geometric objects */
    WKSNone,			/** non-standard, for pure attribute records */
    WKSLinearRingType,

    WKSPoint25DType,		/** 2.5D extension as per 99-402 */
    WKSLineString25DType,	/** 2.5D extension as per 99-402 */
    WKSLinearPolygon25DType,	/** 2.5D extension as per 99-402 */
    WKSMultiPoint25DType,	/** 2.5D extension as per 99-402 */
    WKSMultiLineString25DType,	  /** 2.5D extension as per 99-402 */
    WKSMultiLinearPolygon25DType, /** 2.5D extension as per 99-402 */
    WKSCollection25DType,	/** 2.5D extension as per 99-402 */
  };

  /**
   * near-equivalent to the ’CoordinateGeometry of the spec’
   * \n\n
   * Replaces the union WKGeometry of the OpenGIS Simple Features CORBA idl.
   */ 
  class WKSGeometry {
    WKSType type;
  };
  sequence<WKSGeometry> WKSGeometrySeq;

  class WKSPointValue extends WKSGeometry {
    // type = WKSPointType
    WKSPoint point;
  };

  class WKSMultiPointValue extends WKSGeometry {
    // type = WKSMultiPointType
    WKSPointSeq multiPoint;
  };

  class WKSLineStringValue extends WKSGeometry {
    // type = WKSLineStringType
    WKSLineString lineString;
  };

  class WKSMultiLineStringValue extends WKSGeometry {
    // type =  WKSMultiLineStringType
    WKSLineStringSeq multiLineString;
  };

  class WKSLinearRingValue extends WKSGeometry {
    // type = WKSLinearRingType:
    WKSLinearRing linearRing;
  };

  class WKSLinearPolygonValue extends WKSGeometry {
    // type = WKSLinearPolygonType
    WKSLinearPolygon linearPolygon;
  };

  class WKSMultiLinearPolygonValue extends WKSGeometry {
    // type = WKSMultiLinearPolygonType
    WKSLinearPolygonSeq multiLinearPolygon;
  };

  class WKSCollectionValue extends WKSGeometry {
    // type = WKSCollectionType
    WKSGeometrySeq collection;
  };

  struct Envelope {
    WKSPoint minm;
    WKSPoint maxm;
  };

  //----------------------------------------------------------------------
  // Attribute Interface
  //----------------------------------------------------------------------
  sequence<byte> ByteSeq;
  sequence<int> IntegerSeq;
  sequence<double> RealSeq;
  sequence<string> StringSeq;

  /**
   * enum SFDataType -- types correspond with OGR OGRFieldType
   */
  enum SFDataType {
    /** Simple 32bit integer */
    SFInteger
  };
//     SFIntegerList,		// List of 32bit integers
//     SFReal,			// Double Precision floating point
//     SFRealList,			// List of doubles
//     SFString,			// String of ASCII chars
//     SFStringList,		// Array of strings
//     SFWideString,		// deprecated
//     SFWideStringList,		// deprecated
//     SFBinary,			// Raw Binary data
//     SFDate,			// Date
//     SFTime,			// Time
//     SFDateTime			// Date and Time 
//   };

  /**
   * class AttributeValue -- corresponds with OGR class Field.
   */
  class AttributeValue {
    SFDataType type;
  };

  class IntegerValue extends AttributeValue {
    // type = SFInteger
    int asInteger;
  };

  class IntegerListValue extends AttributeValue {
    // type = SFIntegerList
    IntegerSeq asIntegerList;
  };

  class RealValue extends AttributeValue {
    // type = SFReal
    int asReal;
  };

  class RealListValue extends AttributeValue {
    // type = SFRealList
    RealSeq asRealList;
  };

  class StringValue extends AttributeValue {
    // type = SFString
    string asString;
  };

  class StringListValue extends AttributeValue {
    // type = SFStringList
    StringSeq asStringList;
  };

  class BinaryValue extends AttributeValue {
    // type = SFBinary
    ByteSeq asBinary;
  };

  class DateValue extends AttributeValue {
    // type = SFDate
    int year;
    int month;			// 1 - 12
    int day;			// 1 - 31
  };

  class TimeValue extends AttributeValue {
    // type = SFTime
    int hour;			// 0 - 23
    int minute;			// 0 - 59
    int second;			// 0 - 59
    int offset;			// offset from UTC: -12 - +12
  };

  class DateTimeValue extends AttributeValue {
    // type = SFDateTime
    int year;
    int month;			// 1 - 12
    int day;			// 1 - 31
    int hour;			// 0 - 23
    int minute;			// 0 - 59
    int second;			// 0 - 59
    int offset;			// offset from UTC: -12 - +12
  };

  /**
   * struct AttributeType -- roughly corresponds with OGR class FieldDefn
   * and the ogis SF struct PropertyDef.
   */
  struct AttributeType {
    string name;
    SFDataType type;
//     boolean isRequired;
  };
  sequence<AttributeType> AttributeTypeSeq;

  //----------------------------------------------------------------------
  // Feature Interface
  //----------------------------------------------------------------------
  /**
   * interface Feature -- a minimal subset of the intersection between\n
   *    1) interface Feature (OGC sfcorba 1999.06.02)\n
   *    2) interface org.geotools.Feature (Java, http://www.geotools.org), and\n
   *    3) class OGRFeature (C++, http://www.gdal.org)
   */
  interface Feature {
    ["cpp:const"] idempotent
    FeatureType* getFeatureType();

    ["cpp:const"] idempotent
    string getID();

    ["cpp:const"] idempotent
    Geometry* getDefaultGeometry();

    idempotent
    void setDefaultGeometry(Geometry* aGeometry);

    ["cpp:const"] idempotent
    Envelope getBounds();

    ["cpp:const"] idempotent
    int getNumberOfAttributes();

    ["cpp:const"] idempotent
    AttributeValue getAttribute(int index);

    ["cpp:const"] idempotent
    AttributeValue getAttributeByName(string name);

    idempotent
    void setAttribute(int position, AttributeValue* val);

    ["cpp:const"] idempotent
    string getStyleString();
  };

  //----------------------------------------------------------------------
  // FeatureType Interface
  //----------------------------------------------------------------------
  exception FeatureTypeException {};
  exception InheritanceUnsupported extends FeatureTypeException {};
  exception PropertyDefInvalid extends FeatureTypeException {};

  /**
   * interface FeatureType -- a minimal subset of the intersection between\n
   * the OGC sfcorba, GeoTools, and OGR specifications/APIs
   */
  interface FeatureType {

    // feature type name
    ["cpp:const"] idempotent
    string getTypeName();

    // feature type parents
    ["cpp:const"] idempotent
    FeatureTypeSeq getAncestors()
      throws InheritanceUnsupported;

    // feature type children
    ["cpp:const"] idempotent FeatureTypeSeq getChildren()
      throws InheritanceUnsupported;

    ["cpp:const"] idempotent
    int getNumberOfAttributes();

    ["cpp:const"] idempotent
    AttributeType getAttributeType(int index);

    ["cpp:const"] idempotent
    int getAttributeIndex(string name);

    ["cpp:const"] idempotent
    WKSType getGeometryType();
  };


  //----------------------------------------------------------------------
  // FeatureCollection Interface
  //----------------------------------------------------------------------
  interface FeatureCollection extends Feature {
    ["cpp:const"] idempotent
    int getNumberOfElements();

    ["cpp:const"] idempotent
    FeatureIterator* createIterator();
  };

  //----------------------------------------------------------------------
  // FeatureIterator Interface
  //----------------------------------------------------------------------
  exception IteratorInvalid {};
  exception PositionInvalid {};

  /**
   * interface FeatureIterator
   */
  interface FeatureIterator {
    ["cpp:const"] idempotent
    bool more();

    ["cpp:const"] idempotent
    Feature* current()
      throws IteratorInvalid,PositionInvalid;

    bool next(out Feature* aFeature)
      throws IteratorInvalid,PositionInvalid;
    void advance()
      throws IteratorInvalid,PositionInvalid;

    idempotent
    void reset()
      throws IteratorInvalid;
  };

  /**
   * Represents a cross-section of the geoapi api and OGR.
   */
  interface Geometry {
    /**
     * Computes boundary for this geometry.
     *
     * @return geometry of boundary
     */
    ["cpp:const"] idempotent
    Geometry* getBoundry();

    /**
     * Returns the dimension of this geometry.
     *
     * @return dimension of this geometry
     */
    ["cpp:const"] idempotent
    int getDimension();

    /**
     * Returns the dimension of the coordinates of this geometry.
     *
     * @return dimension of the coordinates of this geometry
     */
    ["cpp:const"] idempotent
    int getCoordinateDimension();

    /**
     * Returns the bounding envelope for this geometry.
     *
     * @return bounding envelope for this geometry.
     */
    ["cpp:const"] idempotent
    Envelope getEnvelope();

    /**
     * Returns a Geometry representing the convex hull of this Geometry.
     *
     * @return Geometry representing the convex hull of this Geometry
     */
    ["cpp:const"] idempotent
    Geometry* getConvexHall();

    // geometric
    /**
     * Returns true if the Geometry is the empty set.
     *
     * @return true if the Geometry is the empty set
     */
    ["cpp:const"] idempotent
    bool isEmpty();

    /**
     * Returns true if the Geometry has no anomalous geometric points, such as
     * self intersection or self tangency.
     *
     * @return true if the Geometry has no anomalous geometric points
     */
     ["cpp:const"] idempotent
    bool isSimple();

    /**
     * Returns true if this geometric object has z coordinate values.
     *
     * @returns true if this geometric object has z coordinate values
     */
     ["cpp:const"] idempotent
    bool is3D();

    /**
     * Exports this geometric object to a specific Well-known Text
     * Representation of Geometry. 
     *
     * @return Well-known Text Representation  of this Geometry
     */
    ["cpp:const"] idempotent
    string asText();
  };

  interface GeometryIterator;

  /**
   * interface GeometryCollection
   */
  interface GeometryCollection extends Geometry {
    ["cpp:const"] idempotent
    int getNumberOfElements();

    ["cpp:const"] idempotent
    GeometryIterator* createIterator();
  };

  /**
   * interface GeometryIterator
   */
  interface GeometryIterator {
    ["cpp:const"] idempotent
    bool more();

    ["cpp:const"] idempotent
    Geometry* current()
      throws IteratorInvalid,PositionInvalid;

    bool next(Geometry* aGeometry)
      throws IteratorInvalid,PositionInvalid;
    void advance()
      throws IteratorInvalid,PositionInvalid;

    idempotent void reset()
      throws IteratorInvalid;

  };

  /**
   * interface Point
   */
  interface Point extends Geometry {
    ["cpp:const"] idempotent
    double getX();

    ["cpp:const"] idempotent
    double getY();

    ["cpp:const"] idempotent
    double getZ();
  };

  /**
   * interface MultiPoint
   */
  interface MultiPoint extends GeometryCollection {
  };


  //----------------------------------------------------------------------
  // Curve interface
  //----------------------------------------------------------------------
  exception OutOfDomain {};

  /**
   * interface Curve
   */
  interface Curve extends Geometry {
    ["cpp:const"] idempotent double getLength();
    ["cpp:const"] idempotent Point* getStartPoint();
    ["cpp:const"] idempotent Point* getEndPoint();
    ["cpp:const"] idempotent bool isPlanar();
    ["cpp:const"] idempotent Point* value(double r)
      throws OutOfDomain;
  };

  //----------------------------------------------------------------------
  // LineString interface
  //----------------------------------------------------------------------
  exception InvalidIndex {};
  exception MinimumPoints {};

  /**
   * interface LineString
   */
  interface LineString extends Curve {
    ["cpp:const"] idempotent long getNumPoints();
  };

  /**
   * interface LinearRing
   */
  interface LinearRing extends LineString {
  };

  /**
   * interface MultiCurve
   */
  interface MultiCurve extends GeometryCollection {
    ["cpp:const"] idempotent double getLength();
  };

  /**
   * interface MultiLineString
   */
  interface MultiLineLineString extends MultiCurve {
  };

  //----------------------------------------------------------------------
  // Surface interface
  //----------------------------------------------------------------------
  /**
   * interface Surface
   */
  interface Surface extends Geometry {
    ["cpp:const"] idempotent double getArea();
    ["cpp:const"] idempotent Point* getCentroid();
    ["cpp:const"] idempotent Point* getPointOnSurface();
    ["cpp:const"] idempotent bool isPlanar();
  };

  //----------------------------------------------------------------------
  // Polygon interface
  //----------------------------------------------------------------------
  /**
   * interface Polygon
   */
  interface Polygon extends Surface {
    ["cpp:const"] idempotent LinearRing* getExteriorRing();
    ["cpp:const"] idempotent MultiCurve* getInteriorRings();
  };

};

#endif  // _OGC_SF_IDL_
