// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <ogis/impl/CoordinateSystemI.hpp>

#include <ogr_spatialref.h>

namespace ogis {

  namespace impl {

    /**
     * constructor
     *
     * @param acp_projection projection in WKT
     */
    CsInfoI::CsInfoI(const char* acp_projection)
    {
      mCp_SpatialReference.reset( new ::OGRSpatialReference(acp_projection) );
    }

    ::std::string
    CsInfoI::name(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::authority(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::authorityCode(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::alias(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::abbreviation(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::remarks(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::WKT(const Ice::Current& current)
    {
      return ::std::string();
    }

    ::std::string
    CsInfoI::XML(const Ice::Current& current)
    {
      return ::std::string();
    }

  } // namespace impl

} // namespace ogis
