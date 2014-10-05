// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <ogis/impl/FeatureI.hpp>

#include <Ice/Ice.h>
#include <ogis/impl/FeatureTypeI.hpp>
#include <ogis/impl/GeometryI.hpp>
#include <sstream>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * \n\n
     * Note; the caller is not responsbile for managing the underlying feature.
     *
     * @param aCp_feature handle to OGR feature
     */
    FeatureI::FeatureI(OGRFeature* aCp_feature) :
      mCp_feature(aCp_feature)
    {
    }

    /** destructor */
    FeatureI::~FeatureI() {}

    /**
     * Returns the feature type.
     *
     * @param current method invocation
     * @returns feature type
     */
    ::ogis::FeatureTypePrx
    FeatureI::getFeatureType(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return 0;

      // create FeatureType proxy and return it
      ::ogis::FeatureTypePtr lCp_FeatureType =
	  new FeatureTypeI(mCp_feature->GetDefnRef());

      ::ogis::FeatureTypePrx lCp_FeatureTypePrx =
	  ::ogis::FeatureTypePrx::uncheckedCast(current.adapter
						->addWithUUID(lCp_FeatureType));

      return lCp_FeatureTypePrx;
    }

    /**
     * Returns the feature ID as a string.
     *
     * @param current method invocation
     * @return feature ID
     */
    ::std::string
    FeatureI::getID(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return ::std::string();

      // write the FID (long) to a string	
      std::ostringstream lC_buffer;
      lC_buffer << mCp_feature->GetFID();
      return lC_buffer.str();
    }

    /**
     * Returns the default geometry.
     *
     * @param current method invocation
     * @returns geometry
     */
    ::ogis::GeometryPrx
    FeatureI::getDefaultGeometry(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return 0;

      return ( GeometryI::createGeometryPrx(mCp_feature->GetGeometryRef(),
					    false,
					    current) );
    }

    /**
     * Sets the default geometry (currently not implemented)
     *
     * @param aGeometry geometry
     * @param current method invocation
     */
    void
    FeatureI::setDefaultGeometry(const ::ogis::GeometryPrx& aGeometry,
				 const Ice::Current& current)
    {
    }

    /**
     * Returns the bounding envelope for this feature.
     *
     * @param current method invocation
     * @returns the bounding envelope for this feature.
     */
    ::ogis::Envelope
    FeatureI::getBounds(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return ::ogis::Envelope();

      OGREnvelope lC_OGREnvelope;
      OGRGeometry* lCp_geometry = mCp_feature->GetGeometryRef();
      if (lCp_geometry != 0) {
	lCp_geometry->getEnvelope(&lC_OGREnvelope);
      }

      ::ogis::Envelope lC_envelope;
      lC_envelope.minm.x = lC_OGREnvelope.MinX;
      lC_envelope.minm.y = lC_OGREnvelope.MinY;
      lC_envelope.maxm.x = lC_OGREnvelope.MaxX;
      lC_envelope.maxm.y = lC_OGREnvelope.MaxY;
      return lC_envelope;

    } // FeatureI::setDefaultGeometry(...)

    /**
     * Returns the number of attributes.
     *
     * @param current method invocation
     * @returns number of attributes
     */
    ::Ice::Int
    FeatureI::getNumberOfAttributes(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return 0;

      return mCp_feature->GetFieldCount();
    }

    /**
     * Returns an attribute value given the index.
     *
     * @param index attribute index
     * @param current method invocation
     * @returns attribute value
     */
    ::ogis::AttributeValuePtr
    FeatureI::getAttribute(::Ice::Int index,
			   const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return 0;

      OGRFieldDefn* lCp_FieldDefn = mCp_feature->GetFieldDefnRef(index);
      if (lCp_FieldDefn == 0)
	return 0;

      ::ogis::AttributeValuePtr lCp_value;
      int li_size;
      switch (lCp_FieldDefn->GetType()) {
      case OFTInteger:
	{
	  ::ogis::IntegerValue* lCp_Integer =
	    new ::ogis::IntegerValue;
	  lCp_Integer->asInteger =
	    mCp_feature->GetFieldAsInteger(index);
	  break;
	}
      case OFTIntegerList:
	{
	  ::ogis::IntegerListValue* lCp_IntegerList =
	    new ::ogis::IntegerListValue;
	  const int* li1_data = 
	    mCp_feature->GetFieldAsIntegerList(index, &li_size);
	  if (li_size > 0) {
	    lCp_IntegerList->asIntegerList.resize(li_size);
	    for (int i = 0; i < li_size; i++)
	      lCp_IntegerList->asIntegerList[i] = li1_data[i];
	  }
	  break;
	}
      case OFTReal:
	{
	  ::ogis::RealValue* lCp_Real =
	    new ::ogis::RealValue;
	  lCp_Real->asReal =
	    mCp_feature->GetFieldAsDouble(index);
	  break;
	}
      case OFTRealList:
	{
	  ::ogis::RealListValue* lCp_RealList =
	    new ::ogis::RealListValue;
	  const double* ld1_data = 
	    mCp_feature->GetFieldAsDoubleList(index, &li_size);
	  if (li_size > 0) {
	    lCp_RealList->asRealList.resize(li_size);
	    for (int i = 0; i < li_size; i++)
	      lCp_RealList->asRealList[i] = ld1_data[i];
	  } 
	  break;
	}
      case OFTString:
	{
	  ::ogis::StringValue* lCp_String =
	    new ::ogis::StringValue;
	  lCp_String->asString =
	    mCp_feature->GetFieldAsString(index);
	  break;
	}
      case OFTStringList:
	{
	  ::ogis::StringListValue* lCp_StringList =
	    new ::ogis::StringListValue;
	  char** papszStrings = 
	    mCp_feature->GetFieldAsStringList(index);
	  int li_length = CSLCount(papszStrings);
	  if (li_size > 0) {
	    lCp_StringList->asStringList.resize(li_size);
	    for (int i = 0; i < li_size; i++)
	      lCp_StringList->asStringList[i] = CSLGetField(papszStrings, i);
	  } 
	  break;
	}
      case OFTDateTime:
	{
	  ::ogis::DateTimeValue* lCp_DateTime =
	    new ::ogis::DateTimeValue;
	  mCp_feature->GetFieldAsDateTime(index,
					  &lCp_DateTime->year,
					  &lCp_DateTime->month,
					  &lCp_DateTime->day,
					  &lCp_DateTime->hour,
					  &lCp_DateTime->minute,
					  &lCp_DateTime->second,
					  &lCp_DateTime->offset);
	  break;
	}
      default:
	;
      }

      return lCp_value;

    } // FeatureI::getAttribute(::Ice::Int, const Ice::Current&)

    /**
     * Returns an attribute value given the name.
     *
     * @param name attribute name
     * @param current method invocation
     * @returns attribute value
     */
    ::ogis::AttributeValuePtr
    FeatureI::getAttributeByName(const ::std::string& name,
				 const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return 0;

      return this->getAttribute(mCp_feature->GetFieldIndex(name.c_str()),
				current);
    }

    /**
     * Sets the attribute value by index (not implemented).
     *
     * @param index attribute index
     * @param current method invocation
     */
    void
    FeatureI::setAttribute(::Ice::Int position,
			   const ::ogis::AttributeValuePrx& val,
			   const Ice::Current& current)
    {
    }

    /**
     * Returns the style string (OGR format) for this feature.
     *
     * @param current method invocation
     * @returns style string (OGR format)
     */
    std::string FeatureI::getStyleString(const Ice::Current& current) const
    {
      if (mCp_feature.get() == 0)
	return "";

      return mCp_feature->GetStyleString();
    }

  } // namespace impl

} // namespace ogis
