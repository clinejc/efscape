// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_FEATURETYPE_I_HPP
#define OGIS_IMPL_FEATURETYPE_I_HPP

#include <ogis/ogc_sf.h>
#include <ogr_feature.h>

namespace ogis {

  namespace impl {

    /**
     * Defines an implementation of the ogis::FeatureType interface that uses
     * the OGRFeatureDefn class for the underlying implementation.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 23 Apr 2009
     */
    class FeatureTypeI : virtual public FeatureType
    {
    public:

      FeatureTypeI(OGRFeatureDefn* aCp_FeatureDefn);
      virtual ~FeatureTypeI();

      //
      // ICE FeatureType interface
      //

      virtual ::std::string getTypeName(const Ice::Current&) const;

      virtual ::ogis::FeatureTypeSeq getAncestors(const Ice::Current&) const;

      virtual ::ogis::FeatureTypeSeq getChildren(const Ice::Current&) const;

      virtual ::Ice::Int getNumberOfAttributes(const Ice::Current&) const;

      virtual ::ogis::AttributeType getAttributeType(::Ice::Int,
						     const Ice::Current&) const;

      virtual ::Ice::Int getAttributeIndex(const ::std::string&,
					   const Ice::Current&) const;

      virtual ::ogis::WKSType getGeometryType(const Ice::Current&) const;

    protected:

      /** handle to OGR feature definition*/
      OGRFeatureDefn* mCp_FeatureDefn;

    private:

      FeatureTypeI();		// disable default constructor

    };				// class FeatureTypeI

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_FEATURETYPE_I_HPP
