// __COPYRIGHT_START__
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_COORDINATESYSTEM_I_HPP
#define OGIS_IMPL_COORDINATESYSTEM_I_HPP

#include <ogis/ogc_cs.h>

#include <Ice/Ice.h>
#include <boost/scoped_ptr.hpp>

// forward declaration
class OGRSpatialReference;

namespace ogis {

  namespace impl {

    /**
     * Implements the ICE ::cs::Info interface.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 3 June 2009
     */
    class CsInfoI : virtual public ::cs::CsInfo
    {
    public:
      CsInfoI(const char* acp_projection);

      //
      // ICE CvInfo interface
      //
      virtual ::std::string name(const Ice::Current&);

      virtual ::std::string authority(const Ice::Current&);

      virtual ::std::string authorityCode(const Ice::Current&);

      virtual ::std::string alias(const Ice::Current&);

      virtual ::std::string abbreviation(const Ice::Current&);

      virtual ::std::string remarks(const Ice::Current&);

      virtual ::std::string WKT(const Ice::Current&);


      virtual ::std::string XML(const Ice::Current&);

    protected:

      boost::scoped_ptr< ::OGRSpatialReference> mCp_SpatialReference;
    };

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_COORDINATESYSTEM_I_HPP
