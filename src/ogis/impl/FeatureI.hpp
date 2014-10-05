// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_FEATURE_I_HPP
#define OGIS_IMPL_FEATURE_I_HPP

#include <ogis/ogc_sf.h>
#include <ogr_feature.h>

#include <boost/shared_ptr.hpp>

namespace ogis {

  namespace impl {

    /**
     * Defines an implementation of the ogis::Feature interface that uses the
     * OGRFeature class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.2 created 23 Apr 2009, revised 09 May 2009
     *
     * ChangeLog:
     *   - 2009.05.09 Added smart pointer
     *   - 2009.04.23 created class FeatureI
     */
    class FeatureI : virtual public ::ogis::Feature
    {
    public:

      FeatureI(OGRFeature* aCp_feature);
      virtual ~FeatureI();

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
							   const Ice::Current&) const;

      virtual void setAttribute(::Ice::Int,
				const ::ogis::AttributeValuePrx&,
				const Ice::Current&);

      virtual ::std::string getStyleString(const Ice::Current&) const;

    protected:

      /** handle to feature */
      boost::shared_ptr<OGRFeature> mCp_feature;

    private:

      FeatureI();		// disable default constructor

    };				// class FeatureI

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_FEATURE_I_HPP
