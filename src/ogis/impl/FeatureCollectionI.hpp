// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_FEATURECOLLECTION_I_HPP
#define OGIS_IMPL_FEATURECOLLECTION_I_HPP

#include <ogis/ogc_sf.h>
#include <ogrsf_frmts.h>

#include <boost/scoped_ptr.hpp>
#include <string>

namespace ogis {

  namespace impl {

    /**
     * Defines an implementation of the ogis::FeatureCollection interface that
     * uses the OGRLayer class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 23 Apr 2009
     */
    class FeatureCollectionI : virtual public ::ogis::Feature,
			       virtual public ::ogis::FeatureCollection
    {
    public:

      FeatureCollectionI(OGRLayer* aCp_layer, const char* acp_name);
      virtual ~FeatureCollectionI();

      //
      // ICE Feature interface
      //
      virtual ::ogis::FeatureTypePrx getFeatureType(const Ice::Current&) const;

      virtual ::std::string getID(const Ice::Current&) const;

      virtual ::ogis::GeometryPrx getDefaultGeometry(const Ice::Current&) const;

      virtual void setDefaultGeometry(const ::ogis::GeometryPrx&,
				      const Ice::Current&);

      virtual ::ogis::Envelope getBounds(const Ice::Current&) const;

      virtual ::Ice::Int getNumberOfAttributes(const Ice::Current&) const;

      virtual ::ogis::AttributeValuePtr getAttribute(::Ice::Int,
						     const Ice::Current&) const;

      virtual ::ogis::AttributeValuePtr getAttributeByName(const ::std::string&,
							   const Ice::Current&) 
	const;

      virtual void setAttribute(::Ice::Int,
				const ::ogis::AttributeValuePrx&,
				const Ice::Current&);

      virtual ::std::string getStyleString(const Ice::Current&) const;

      //
      // ICE FeatureCollection interface
      //
      virtual ::Ice::Int getNumberOfElements(const Ice::Current&) const;

      virtual ::ogis::FeatureIteratorPrx createIterator(const Ice::Current&)
	const;

    protected:

      /** handle to feature layer */
      OGRLayer* mCp_layer;

      /** handle to feature geometry */
      boost::scoped_ptr<OGRGeometryCollection> mCp_geometry;

      /** layer name */
      std::string mC_name;

    private:

      FeatureCollectionI();	// disable default constructor


    public:

      /**
       * Defines an implementation of the ogis::FeatureIterator interface that
       * uses the OGRLayer class for the underlying implementation.
       *
       * @author Jon Cline <clinej@stanfordalumni.org>
       * @version 0.0.1 created 09 May 2009
       */
      class Iterator : virtual public ::ogis::FeatureIterator
      {
      public:

	Iterator(OGRLayer* aCp_collection, const Ice::Current& aCr_current);

	//
	// ICE FeatureIterator
	//
	virtual bool more(const Ice::Current&) const;
	virtual ::ogis::FeaturePrx current(const Ice::Current&) const;
	virtual bool next(::ogis::FeaturePrx&,
			  const Ice::Current&);
					
	virtual void advance(const Ice::Current&);
	virtual void reset(const Ice::Current&);

      private:

	/** handle to collection */
	OGRLayer* mCp_collection;

	/** handle to current feature*/
	::ogis::FeaturePrx mCp_current;

	/** handle to next feature */
	::ogis::FeaturePrx mCp_next;

	Iterator();		// disable default constructor

      };			// class FeatureCollectionI::Iterator

    };				// class FeatureCollectionI

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_FEATURECOLLECTION_I_HPP
