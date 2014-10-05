// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_GEOMETRY_I_HPP
#define OGIS_IMPL_GEOMETRY_I_HPP

#include <ogis/ogc_sf.h>
#include <ogr_geometry.h>

#include <loki/Factory.h>
#include <loki/Functor.h>
#include <loki/Singleton.h>
#include <boost/scoped_ptr.hpp>

namespace ogis {

  namespace impl {

    // forward declarations
    class GeometryI;

    /**
     * This emplate function create a GeometryI object of type T
     *
     * @param aCp_geometry underlying OGR geometry
     * @param ab_IsOwner whether this object owns the associated OGR geometry
     * @returns handle to new GeometryI object of derived type T
     */
    template <class T>
    GeometryI* createGeometry(OGRGeometry* aCp_geometry,
			      bool ab_IsOwner) {
      return (GeometryI*)new T(aCp_geometry,ab_IsOwner);
    }

    /**
     * Defines an implementation of the ogis::Geometry interface that uses the
     * OGRGeometry class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 23 Apr 2009
     */
    class GeometryI : virtual public ::ogis::Geometry
    {
    public:

      GeometryI(OGRGeometry* aCp_geometry,
		bool ab_IsOwner = false);
      virtual ~GeometryI();

      //
      // ICE Geometry interface
      //
      virtual ::ogis::GeometryPrx getBoundry(const Ice::Current&) const;

      virtual ::Ice::Int getDimension(const Ice::Current&) const;

      virtual ::Ice::Int getCoordinateDimension(const Ice::Current&) const;

      virtual ::ogis::Envelope getEnvelope(const Ice::Current&) const;

      virtual ::ogis::GeometryPrx getConvexHall(const Ice::Current&) const;

      virtual bool isEmpty(const Ice::Current&) const;

      virtual bool isSimple(const Ice::Current&) const;

      virtual bool is3D(const Ice::Current&) const;

      virtual ::std::string asText(const Ice::Current&) const;

      //
      // local accessor/mutator/creator methods
      //
      static GeometryI* createGeometry(OGRGeometry* aCp_geometry,
				       bool ab_IsOwner = false);

      static ::ogis::GeometryPrx
      createGeometryPrx(OGRGeometry* aCp_geometry,
			bool ab_IsOwner,
			const Ice::Current& aCr_current);

      virtual const OGRGeometry& getType() const = 0;

    protected:

      /** handle to OGR geometry */
      OGRGeometry* mCp_geometry;

      /** whether this object owns the OGR geometry object */
      bool mb_IsOwner;

    private:

      GeometryI();		// disable default constructor

    };


    class PointI;

    /**
     * Defines an implementation of the ogis::Point interface that uses the
     * OGRPoint class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 23 Apr 2009
     */
    class PointI : virtual public ::ogis::Point,
		   virtual public GeometryI
    {
    public:

      PointI(OGRGeometry* aCp_geometry,
	     bool ab_IsOwner = false);

      //
      // ICE Point interface
      //
      virtual double getX(const Ice::Current&) const;
      virtual double getY(const Ice::Current&) const;
      virtual double getZ(const Ice::Current&) const;

      const OGRGeometry& getType() const;
      static const OGRGeometry& Type();

    private:

      PointI();			// disable default constructor

      /** associated OGRGeometry type */
      static boost::scoped_ptr< ::OGRGeometry> mSCp_type;

    };				// class PointI


    /**
     * Defines an implementation of the ogis::GeometryCollection interface that
     * uses the OGRGeometryCollection class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 08 May 2009
     */
    class GeometryCollectionI : virtual public ::ogis::GeometryCollection,
				virtual public GeometryI
    {
    public:

      GeometryCollectionI(OGRGeometry* aCp_geometry);

      //
      // ICE GeometryCollection
      //
      virtual ::Ice::Int getNumberOfElements(const Ice::Current&);
      virtual ::ogis::GeometryIteratorPtr createIterator(const Ice::Current&);


      /**
       * Defines an implementation of the ogis::GeometryIterator interface that
       * is tied to an OGRGeometryCollection object.
       *
       * @author Jon Cline <clinej@stanfordalumni.org>
       * @version 0.0.1 created 08 May 2009
       */
      class Iterator : virtual public ::ogis::GeometryIterator
      {
      public:

	Iterator(OGRGeometryCollection* aCp_collection);

	//
	// ICE GeometryIterator
	//
	virtual bool more(const Ice::Current&);
	virtual ::ogis::GeometryPtr current(const Ice::Current&);
	virtual ::ogis::GeometryPtr next(const Ice::Current&);
	virtual void advance(const Ice::Current&);
	virtual void reset(const Ice::Current&);

      private:

	/** handle to collection */
	OGRGeometryCollection* mCp_collection;

	/** handle to geometry */
	OGRGeometry* mCp_OGRGeometry;

	/** index of current geometry */
	int mi_index;

	Iterator();	// disable default constructor
      };

    private:

      GeometryCollectionI();	// disable default constructor

    };				// class GeometryCollectionI

    /**
     * Defines an implementation of the ogis::MultiPoint interface that
     * uses the OGRMultiPoint class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 26 May 2009
     */
    class MultiPointI : virtual public ::ogis::MultiPoint,
			virtual public GeometryCollectionI
    {
    public:

      MultiPointI(OGRGeometry* aCp_geometry);

    private:

      MultiPointI();		// disable default constructor
      
    };				// class MultiPointI


    /**
     * This template instantiates a factory for GeometryI classes
     */
    typedef ::Loki::SingletonHolder
    <
      ::Loki::Factory< GeometryI, std::string, LOKI_TYPELIST_2( OGRGeometry*, bool )  >
      > TheGeometryFactory;

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_GEOMETRY_I_HPP
