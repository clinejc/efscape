// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <ogis/impl/FeatureCollectionI.hpp>

#include <Ice/Ice.h>
#include <ogis/impl/FeatureI.hpp>
#include <ogis/impl/FeatureTypeI.hpp>
#include <ogis/impl/GeometryI.hpp>

namespace ogis {

  namespace impl {

    /**
     * constructor\n\n
     *
     * Note: the OGR layer is owned by the OGRDataSource.
     *
     * @param aCp_layer handle to feature layer
     * @param aC_name layer name
     */
    FeatureCollectionI::FeatureCollectionI(OGRLayer* aCp_layer,
					   const char* acp_name) :
      mCp_layer(aCp_layer),
      mC_name(acp_name)
    {
      // 2009.05.26: current supporting only MultiPoint collections
      mCp_geometry.reset(new OGRMultiPoint);
    }

    /** destructor */
    FeatureCollectionI::~FeatureCollectionI() {}

    /**
     * Returns the feature type.
     *
     * @param current method invocation
     * @returns feature type
     */
    ::ogis::FeatureTypePrx
    FeatureCollectionI::getFeatureType(const Ice::Current& current) const
    {
      if (mCp_layer == 0)
	return 0;

      // create FeatureType proxy and return it
      ::ogis::FeatureTypePtr lCp_FeatureType =
	  new FeatureTypeI(mCp_layer->GetLayerDefn());

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
    FeatureCollectionI::getID(const Ice::Current& current) const
    {
      return mC_name;
    }

    /**
     * Returns the default geometry.
     *
     * @param current method invocation
     * @returns geometry
     */
    ::ogis::GeometryPrx
    FeatureCollectionI::getDefaultGeometry(const Ice::Current& current) const
    {
      return 0;
    }

    /**
     * Sets the default geometry (currently not implemented)
     *
     * @param aGeometry geometry
     * @param current method invocation
     */
    void
    FeatureCollectionI::setDefaultGeometry(const ::ogis::GeometryPrx& aGeometry,
					   const Ice::Current& current)
    {
    }

    /**
     * Gets the evelope of the bounds for this feature collection.
     *
     * @param current method invocation
     * @returns envelope of bounds
     */
    ::ogis::Envelope
    FeatureCollectionI::getBounds(const Ice::Current& current) const
    {
      ::ogis::Envelope lC_envelope;

      if (mCp_layer == 0)
	return lC_envelope;

      OGREnvelope lC_OGREnvelope;
      mCp_layer->GetExtent(&lC_OGREnvelope);

      lC_envelope.minm.x = lC_OGREnvelope.MinX;
      lC_envelope.minm.y = lC_OGREnvelope.MinY;
      lC_envelope.maxm.x = lC_OGREnvelope.MaxX;
      lC_envelope.maxm.y = lC_OGREnvelope.MaxY;

      return lC_envelope;
    }

    /**
     * Returns the number of attributes.
     *
     * @param current method invocation
     * @returns number of attributes
     */
    ::Ice::Int
    FeatureCollectionI::getNumberOfAttributes(const Ice::Current& current) const
    {
      if (mCp_layer == 0)
	return 0;

      return mCp_layer->GetLayerDefn()->GetFieldCount();
    }

    /**
     * Returns an attribute value given the index (not implemented).\n
     * OGRLayer lacks an interface for accessing fields (attributes).
     *
     * @param index attribute index
     * @param current method invocation
     * @returns attribute value
     */
    ::ogis::AttributeValuePtr
    FeatureCollectionI::getAttribute(::Ice::Int index,
				     const Ice::Current& current) const
    {
      // note: OGRLayer lacks an interface for accessing fields
      return 0;
    }

    /**
     * Returns an attribute value given the name (not implemented).\n
     * OGRLayer lacks an interface for accessing fields (attributes).
     *
     * @param name attribute name
     * @param current method invocation
     * @returns attribute value
     */
    ::ogis::AttributeValuePtr
    FeatureCollectionI::getAttributeByName(const ::std::string& name,
					   const Ice::Current& current) const
    {
      return 0;
    }

    /**
     * Sets the attribute value by index (not implemented).
     *
     * @param index attribute index
     * @param current method invocation
     */
    void
    FeatureCollectionI::setAttribute(::Ice::Int position,
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
    std::string
    FeatureCollectionI::getStyleString(const Ice::Current& current) const
    {
      return std::string();
    }

    // end of ICE Feature implementation
    // start of ICE FeatureCollection implementation

    /**
     * Returns the number of elements.
     *
     * @param current method invocation
     * @returns number of elements
     */ 
   ::Ice::Int
    FeatureCollectionI::getNumberOfElements(const Ice::Current& current) const
    {
      if (mCp_layer == 0)
	return 0;

      return mCp_layer->GetFeatureCount();
    }

    /**
     * Create an iterator for this collection.
     *
     * @param current method invocation
     * @returns feature collection iterator
     */ 
    ::ogis::FeatureIteratorPrx
    FeatureCollectionI::createIterator(const Ice::Current& current) const
    {
      if (mCp_layer == 0)
	return 0;

      // create FeatureIterator proxy and return it
      ::ogis::FeatureIteratorPtr lCp_FeatureIterator =
	  new FeatureCollectionI::Iterator(mCp_layer, current);

      ::ogis::FeatureIteratorPrx lCp_FeatureIteratorPrx =
	  ::ogis::FeatureIteratorPrx::uncheckedCast(current.adapter
						    ->addWithUUID(lCp_FeatureIterator));

      return lCp_FeatureIteratorPrx;
    }

    // end of class FeatureCollectionI implementation
    // start of class FeatureCollectionI::Iterator implementation

    /**
     * constructor
     *
     * @param aCp_collection handle to collection
     * @param current method invocation
     */
    FeatureCollectionI::Iterator::Iterator(OGRLayer* aCp_collection,
					   const Ice::Current& current) :
      mCp_collection(aCp_collection),
      mCp_current(0),
      mCp_next(0)
    {
      if (mCp_collection == 0)
	return;

      mCp_collection->ResetReading(); // set to beginning of collection

      OGRFeature* lCp_OGRFeature = mCp_collection->GetNextFeature();
      if (lCp_OGRFeature == 0)
	return;

      // set proxy for current feature
      ::ogis::FeaturePtr lCp_feature = new FeatureI(lCp_OGRFeature);
      mCp_current =
	::ogis::FeaturePrx::uncheckedCast(current.adapter
					  ->addWithUUID(lCp_feature));

      // set proxy for next feature
      if ( (lCp_OGRFeature = mCp_collection->GetNextFeature()) == 0)
	return;

      lCp_feature = new FeatureI(lCp_OGRFeature);
      mCp_next =
	::ogis::FeaturePrx::uncheckedCast(current.adapter
					  ->addWithUUID(lCp_feature));
    }

    /**
     * Returns whether this iterator is done.
     *
     * @param current method invocation
     * @returns whether this iterator is done
     */
    bool
    FeatureCollectionI::Iterator::more(const Ice::Current& current) const
    {
      if (mCp_collection == 0)
	return false;

      return (mCp_next != 0);
    }

    /**
     * Returns feature currently pointed to.
     *
     * @param current method invocation
     * @returns feature currently pointed to.
     */
    ::ogis::FeaturePrx
    FeatureCollectionI::Iterator::current(const Ice::Current& current) const
    {
      if (mCp_collection == 0 || mCp_current == 0)
	throw ::ogis::IteratorInvalid();

      return mCp_current;
    }

    /**
     * Advances the iterators internal pointer to the next feature.
     *
     * @param aCr_feature proxy for next feature
     * @param aCr_current method invocation
     * @returns whether there is a next feature
     */
    bool
    FeatureCollectionI::Iterator::next(::ogis::FeaturePrx& aCr_feature,
				       const Ice::Current& aCr_current)
    {
      this->advance(aCr_current);

      aCr_feature = this->mCp_current; // note: this is the feature proxy

      return (aCr_feature != 0);
    }

    /**
     * Advances the iterator.
     *
     * @param current method invocation
     */
    void
    FeatureCollectionI::Iterator::advance(const Ice::Current& current)
    {
      if (mCp_collection == 0 || mCp_current == 0)
	throw ::ogis::IteratorInvalid();

      mCp_current = mCp_next;

      OGRFeature* lCp_OGRFeature = mCp_collection->GetNextFeature();
      if (lCp_OGRFeature == 0) {
	mCp_next = 0;
	return;
      }

      ::ogis::FeaturePtr lCp_feature = new FeatureI(lCp_OGRFeature);
      mCp_next =
	::ogis::FeaturePrx::uncheckedCast(current.adapter
					  ->addWithUUID(lCp_feature));
    }

    /**
     * Resets the iterator to the beginning of the collection
     *
     * @param current method invocation
     */
    void
    FeatureCollectionI::Iterator::reset(const Ice::Current& current)
    {
      if (mCp_collection == 0)
	throw ::ogis::IteratorInvalid();

      mCp_collection->ResetReading(); // set to beginning of collection

      OGRFeature* lCp_OGRFeature = mCp_collection->GetNextFeature();
      if (lCp_OGRFeature == 0)
	throw ::ogis::IteratorInvalid();

      // set proxy for current feature
      ::ogis::FeaturePtr lCp_feature = new FeatureI(lCp_OGRFeature);
      mCp_current =
	::ogis::FeaturePrx::uncheckedCast(current.adapter
					  ->addWithUUID(lCp_feature));

      // set proxy for next feature
      if ( (lCp_OGRFeature = mCp_collection->GetNextFeature()) == 0) {
	mCp_next = 0;
	return;
      }
      mCp_next =
	::ogis::FeaturePrx::uncheckedCast(current.adapter
					  ->addWithUUID(lCp_feature));
    }

    // end of class FeatureCollectionI::Iterator implementation

  } // namespace impl

} // namespace ogis
