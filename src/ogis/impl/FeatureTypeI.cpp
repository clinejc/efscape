// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <ogis/impl/FeatureTypeI.hpp>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * @param aCp_FeatureDefn handle to feature definition
     */
    FeatureTypeI::FeatureTypeI(OGRFeatureDefn* aCp_FeatureDefn) :
      mCp_FeatureDefn(aCp_FeatureDefn)
    {
    }

    /** destructor */
    FeatureTypeI::~FeatureTypeI() {}

    /**
     * Returns feature type name.
     *
     * @param current method invocation
     * @returns feature type name
     */
    ::std::string
    FeatureTypeI::getTypeName(const Ice::Current& current) const
    {
      if (mCp_FeatureDefn == 0)
	return ::std::string();

      return mCp_FeatureDefn->GetName();
    }

    /**
     * Returns sequence of feature type ancestors.
     *
     * @param current method invocation
     * @returns sequence of feature type ancestors
     */
    ::ogis::FeatureTypeSeq
    FeatureTypeI::getAncestors(const Ice::Current& current) const
    {
      return ::ogis::FeatureTypeSeq();
    }

    /**
     * Returns sequence of feature type children.
     *
     * @param current method invocation
     * @returns sequence of feature type children
     */
    ::ogis::FeatureTypeSeq
    FeatureTypeI::getChildren(const Ice::Current& current) const
    {
      return ::ogis::FeatureTypeSeq();
    }

    /**
     * Returns the number of attributes.
     *
     * @param current method invocation
     * @returns number of attributes
     */
    ::Ice::Int
    FeatureTypeI::getNumberOfAttributes(const Ice::Current& current) const
    {
      if (mCp_FeatureDefn == 0)
	return 0;

      return mCp_FeatureDefn->GetFieldCount();
    }

    /**
     * Returns the attribute type of field <index>.
     *
     * @param index field index
     * @param current method invocation
     * @returns attribute type of field
     */
    ::ogis::AttributeType
    FeatureTypeI::getAttributeType(::Ice::Int index,
				   const Ice::Current& current) const
    {
      ::ogis::AttributeType lC_AttributeType; // default

      if (mCp_FeatureDefn == 0)
	return lC_AttributeType;

      OGRFieldDefn* lCp_FieldDefn = mCp_FeatureDefn->GetFieldDefn(index);
      if (lCp_FieldDefn == 0)
	return lC_AttributeType;

      // note: there should be a 1:1 mapping of data types from OGR to OGIS
      lC_AttributeType.name = lCp_FieldDefn->GetNameRef();
      lC_AttributeType.type = (::ogis::SFDataType)lCp_FieldDefn->GetType();
      return lC_AttributeType;
    }

    /**
     * Returns the attribute index of field <name>.
     *
     * @param name name of attribute
     * @param current method invocation
     * @returns index of attribute (-1 if not found)
     */
    ::Ice::Int
    FeatureTypeI::getAttributeIndex(const ::std::string& name,
				    const Ice::Current& current) const
    {
      if (mCp_FeatureDefn == 0)
	return -1;

      return mCp_FeatureDefn->GetFieldIndex(name.c_str());
    }

    /**
     * Returns WKS geometry type.
     *
     * @param current method invocation
     * @returns WKS geometry type
     */
    ::ogis::WKSType
    FeatureTypeI::getGeometryType(const Ice::Current& current) const
    {
      ::ogis::WKSType le_WKSType = ::ogis::WKSUnknown;

      if (mCp_FeatureDefn == 0)
	return le_WKSType;

      OGRwkbGeometryType le_GeometryType = mCp_FeatureDefn->GetGeomType();

      switch(le_GeometryType) {
      case wkbPoint:
	le_WKSType = ::ogis::WKSPointType;
	break;
      case wkbMultiPoint:
	le_WKSType = ::ogis::WKSMultiPointType;
	break;
      case wkbLineString:
	le_WKSType = ::ogis::WKSLineStringType;
	break;
      case wkbMultiLineString:
	le_WKSType = ::ogis::WKSMultiLineStringType;
	break;
      case wkbPolygon:
	le_WKSType = ::ogis::WKSLinearPolygonType;
	break;
      case wkbMultiPolygon:
	le_WKSType = ::ogis::WKSMultiLinearPolygonType;
	break;
      case wkbGeometryCollection :
	le_WKSType = ::ogis::WKSCollectionType;
	break;
      case wkbNone :
	le_WKSType = ::ogis::WKSNone;
	break;
      case wkbLinearRing :
	le_WKSType = ::ogis::WKSLinearRingType;
	break;
      case wkbPoint25D:
	le_WKSType = ::ogis::WKSPoint25DType;
	break;
      case wkbMultiPoint25D:
	le_WKSType = ::ogis::WKSMultiPoint25DType;
	break;
      case wkbLineString25D:
	le_WKSType = ::ogis::WKSLineString25DType;
	break;
      case wkbMultiLineString25D:
	le_WKSType = ::ogis::WKSMultiLineString25DType;
	break;
      case wkbPolygon25D:
	le_WKSType = ::ogis::WKSLinearPolygon25DType;
	break;
      case wkbMultiPolygon25D:
	le_WKSType = ::ogis::WKSMultiLinearPolygon25DType;
	break;
      case wkbGeometryCollection25D:
	le_WKSType = ::ogis::WKSCollection25DType;
	break;
      default:
	;
      }
      return le_WKSType;
    }

  } // namespace impl

} // namespace ogis
