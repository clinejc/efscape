// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <ogis/impl/GeometryI.hpp>

#include <Ice/Ice.h>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * @param aCp_geometry handle to OGR geometry
     * @param ab_IsOwner whether this object owns the underlying geometry
     */
    GeometryI::GeometryI(OGRGeometry* aCp_geometry, bool ab_IsOwner) :
      mCp_geometry(aCp_geometry),
      mb_IsOwner(ab_IsOwner)
    {
    }

    /** destructor */
    GeometryI::~GeometryI() {
      if (mb_IsOwner && mCp_geometry != 0)
	delete mCp_geometry;
    }

    /**
     * Create an OGIS geometry from an OGR geometry.
     *
     * @param aCp_geometry handle to OGR Geometry
     * @param ab_IsOwner whether this object owns the underlying geometry
     * @returns a handle to a OGIS Geometry wrapping the OGR Geometry
     */
    GeometryI* GeometryI::createGeometry(OGRGeometry* aCp_geometry,
					 bool ab_IsOwner)
    {
      if (aCp_geometry == 0)
	return 0;

      // create GeometryI object from the geometry factory
      GeometryI* lCp_GeometryI = TheGeometryFactory::Instance().
	CreateObject(aCp_geometry->getGeometryName(),
		     aCp_geometry,
		     ab_IsOwner);

      return lCp_GeometryI;
    }

    /**
     * Create an OGIS geometry from an OGR geometry.
     *
     * @param aCp_geometry handle to OGR Geometry
     * @param ab_IsOwner whether this object owns the underlying geometry
     * @param current method invocation
     * @returns a handle to a OGIS Geometry wrapping the OGR Geometry
     */
    ::ogis::GeometryPrx
    GeometryI::createGeometryPrx(OGRGeometry* aCp_geometry,
				 bool ab_IsOwner,
				 const Ice::Current& current)
    {
      GeometryI* lCp_GeometryI =
	GeometryI::createGeometry(aCp_geometry,
				  ab_IsOwner);
      if (lCp_GeometryI == 0)
	return 0;

      // activate geometry servant
      ::ogis::GeometryPrx lCp_GeometryPrx =
	  ::ogis::GeometryPrx::uncheckedCast(current.adapter
					     ->addWithUUID(lCp_GeometryI));

      return lCp_GeometryPrx;
    }

    /**
     * Returns the Geometry of the boundary of this object.
     *
     * @param current method invocation
     * @returns Geometry of the boundary of this object.
     */
    ::ogis::GeometryPrx
    GeometryI::getBoundry(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return 0;

      return ( GeometryI::createGeometryPrx(mCp_geometry->getBoundary(),
					    true,
					    current) );
    }

    /**
     * Returns the dimension of this object.
     *
     * @param current method invocation
     * @returns dimension of this object
     */
    ::Ice::Int
    GeometryI::getDimension(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return 0;

      return mCp_geometry->getDimension();
    }

    /**
     * Returns the dimension of the coordinates of this object.
     *
     * @param current method invocation
     * @returns dimension of the coordinates of this object
     */
    ::Ice::Int
    GeometryI::getCoordinateDimension(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return 0;

      return mCp_geometry->getCoordinateDimension();
    }

    /**
     * Returns the bounding envelope for this geometry.
     *
     * @param current method invocation
     * @returns the bounding envelope for this geometry.
     */
    ::ogis::Envelope
    GeometryI::getEnvelope(const Ice::Current& current) const {
      if (mCp_geometry == 0)
	return ::ogis::Envelope();

      OGREnvelope lC_OGREnvelope;
      mCp_geometry->getEnvelope(&lC_OGREnvelope);

      ::ogis::Envelope lC_envelope;
      lC_envelope.minm.x = lC_OGREnvelope.MinX;
      lC_envelope.minm.y = lC_OGREnvelope.MinY;
      lC_envelope.maxm.x = lC_OGREnvelope.MaxX;
      lC_envelope.maxm.y = lC_OGREnvelope.MaxY;
      return lC_envelope;
      
    } // GeometryI::getEnvelope(const Ice::Current&)

    /**
     * Returns a Geometry representing the convex hull of this Geometry.
     *
     * @param current method invocation
     * @returns Geometry representing the convex hull of this Geometry
     */
    ::ogis::GeometryPrx
    GeometryI::getConvexHall(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return 0;

      return ( GeometryI::createGeometryPrx(mCp_geometry->ConvexHull(),
					    true,
					    current) );
    }


    /**
     * Returns true if the Geometry is the empty set.
     *
     * @returns true if the Geometry is the empty set
     */
    bool
    GeometryI::isEmpty(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return true;

      return ( mCp_geometry->IsEmpty() );
    }

    /**
     * Returns true if the Geometry has no anomalous geometric points, such as
     * self intersection or self tangency.
     *
     * @returns true if the Geometry has no anomalous geometric points
     */
    bool
    GeometryI::isSimple(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return true;

      return ( mCp_geometry->IsSimple() );
    }

    /**
     * Returns true if this geometric object has z coordinate values.
     *
     * @returns true if this geometric object has z coordinate values
     */
    bool
    GeometryI::is3D(const Ice::Current& current) const
    {
      return false;
    }

    /**
     * Exports this geometric object to a specific Well-known Text
     * Representation of Geometry. 
     *
     * @returns Well-known Text Representation  of this Geometry
     */
    ::std::string
    GeometryI::asText(const Ice::Current& current) const
    {
      if (mCp_geometry == 0)
	return std::string();

      char** ppszDstText;
      if (mCp_geometry->exportToWkt(ppszDstText) != OGRERR_NONE)
	return std::string();

      return std::string();
    }

    // end of class GeometryI implementation
    // start of class PointI implementation

    // instantiate class data
    boost::scoped_ptr< ::OGRGeometry> PointI::mSCp_type(new OGRPoint);

    /**
     * constructor
     *
     * @param aCp_point handle to OGR point
     */
    PointI::PointI(OGRGeometry* aCp_geometry, bool ab_IsOwner) :
      GeometryI(aCp_geometry, ab_IsOwner)
    {
    }

    /**
     * Returns reference to associated OGRGeometry.
     *
     * @returns reference to associated OGRGeometry.
     */
    const OGRGeometry& PointI::Type() {
      if (mSCp_type.get() == 0)
	mSCp_type.reset(new OGRPoint);

      return *mSCp_type;
    }

    /**
     * Returns reference to associated OGRGeometry.
     *
     * @returns reference to associated OGRGeometry.
     */
    const OGRGeometry& PointI::getType() const {
      return PointI::Type();
    }
    /**
     * Returns the X coordinate of the point.
     *
     * @param current method invocation
     * @returns X coordinate
     */
    double PointI::getX(const Ice::Current& current) const
    {
      double ld_x = 0;
      OGRPoint* lCp_OGRPoint = dynamic_cast<OGRPoint*>(mCp_geometry);
      if (lCp_OGRPoint != 0)
	ld_x = lCp_OGRPoint->getX();

      return ld_x;
    }

    /**
     * Returns the Y coordinate of the point.
     *
     * @param current method invocation
     * @returns Y coordinate
     */
    double PointI::getY(const Ice::Current& current) const
    {
      double ld_y = 0;
      OGRPoint* lCp_OGRPoint = dynamic_cast<OGRPoint*>(mCp_geometry);
      if (lCp_OGRPoint != 0)
	ld_y = lCp_OGRPoint->getY();

      return ld_y;
    }

    /**
     * Returns the Z coordinate of the point.
     *
     * @param current method invocation
     * @returns Z coordinate
     */
    double PointI::getZ(const Ice::Current& current) const
    {
      double ld_z = 0;
      OGRPoint* lCp_OGRPoint = dynamic_cast<OGRPoint*>(mCp_geometry);
      if (lCp_OGRPoint != 0)
	ld_z = lCp_OGRPoint->getZ();

      return ld_z;
    }

    // register PointI geometry with factory
    const bool registered = TheGeometryFactory::Instance().
      Register(PointI::Type().getGeometryName(),
	       createGeometry<PointI>);

    // end of class PointI implementation
    // start of class GeometryCollectionI implementation

    /**
     * constructor
     *
     * @param aCp_geometry handle to OGR GeometryCollection
     */
    GeometryCollectionI::GeometryCollectionI(OGRGeometry* aCp_geometry) :
      GeometryI(aCp_geometry)
    {
    }

    /**
     * Returns the number of elements in this collection.
     *
     * @param current method invocation
     * @returns number of elements
     */
    ::Ice::Int
    GeometryCollectionI::getNumberOfElements(const Ice::Current& current)
    {
      return 0;
    }

    /**
     * Returns an iterator for this collection.
     *
     * @param current method invocation
     * @returns iterator for this collection
     */
    ::ogis::GeometryIteratorPtr
    GeometryCollectionI::createIterator(const Ice::Current& current)
    {
      return 0;
    }

    // end of class GeometryCollectionI implementation
    // start of class GeometryCollectionI::GeometryIterator implementation

    /**
     * constructor
     *
     * @param aCp_collection handle to collection
     */
    GeometryCollectionI::Iterator::Iterator(OGRGeometryCollection*
					    aCp_collection) :
      mCp_collection(aCp_collection),
      mCp_OGRGeometry(0),
      mi_index(0)
    {
      if (mCp_collection != 0)
	return;

      if (mCp_collection->getNumGeometries() == 0)
	return;

      mCp_OGRGeometry = mCp_collection->getGeometryRef(mi_index);
    }

    /**
     * Returns whether this iterator is done.
     *
     * @param current method invocation
     * @returns whether this iterator is done
     */
    bool
    GeometryCollectionI::Iterator::more(const Ice::Current& current)
    {
      if (mCp_collection == 0)
	return false;
	
      return (mi_index < (mCp_collection->getNumGeometries()) );
    }

    /**
     * Returns geometry currently pointed to.
     *
     * @param current method invocation
     * @returns geometry currently pointed to.
     */
    ::ogis::GeometryPtr
    GeometryCollectionI::Iterator::current(const Ice::Current& current)
    {
      if (mCp_collection == 0)
	throw ::ogis::IteratorInvalid();

      if (mi_index >= mCp_collection->getNumGeometries())
	throw ::ogis::PositionInvalid();

      mCp_OGRGeometry = mCp_collection->getGeometryRef(mi_index);

      if (mCp_OGRGeometry == 0)
	return 0;

      return GeometryI::createGeometry(mCp_OGRGeometry,false);
    }

    /**
     * Returns the next geometry.
     *
     * @param current method invocation
     * @returns next geometry.
     */
    ::ogis::GeometryPtr
    GeometryCollectionI::Iterator::next(const Ice::Current& current)
    {
      this->advance(current);

      if (mi_index == mCp_collection->getNumGeometries())
	return 0;

      return this->current(current);
    }

    /**
     * Advances the iterator.
     *
     * @param current method invocation
     */
    void
    GeometryCollectionI::Iterator::advance(const Ice::Current& current)
    {
      if (mCp_collection == 0)
	throw ::ogis::IteratorInvalid();

      if (mi_index >= mCp_collection->getNumGeometries())
	throw ::ogis::PositionInvalid();

      ++mi_index;
    }

    /**
     * Resets the iterator to the beginning of the collection
     *
     * @param current method invocation
     */
    void
    GeometryCollectionI::Iterator::reset(const Ice::Current& current)
    {
      if (mCp_collection == 0)
	throw ::ogis::IteratorInvalid();

      mi_index = 0;
      mCp_OGRGeometry = mCp_collection->getGeometryRef(mi_index);
    }

    // end of class GeometryCollectionI::Iterator implementation
    // start of class MultiPointI implementation

    /**
     * constructor
     *
     * @param aCp_geometry handle to OGRMultiPoint
     */
    MultiPointI::MultiPointI(OGRGeometry* aCp_geometry) :
      GeometryCollectionI(aCp_geometry),
      GeometryI(aCp_geometry)
    {
    }

    // end of class MultiPointI implementation

  } // namespace impl

} // namespace ogis
