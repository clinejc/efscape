// boost serialization archive definitions
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/version.hpp>

// class boost serialization export definitions
#include <adevs/adevs_serialization.hpp>

// template class implementation and wrapped model definition
#include <efscape/impl/RelogoWrapper.ipp>
#include "ZombieObserver.h"
#include "relogo/Patch.h"
// #include <ircm/Ircm.hpp>

// // Required for class Ircm
// #include <ircm/agents/IrcmAgent.hpp>
// #include <ircm/agents/TaxAgency.hpp>
// #include <ircm/agents/FilerType.hpp>
// #include <ircm/contexts/EmployersSharedContext.hpp>
// #include <ircm/contexts/PreparersSharedContext.hpp>
// #include <ircm/contexts/ZonesSharedContext.hpp>
// #include <ircm/random/IrcmRandom.hpp>
// #include <ircm/taxreturn/Imputations.hpp>
// #include <ircm/taxreturn/ReportingParameters.hpp>
// #include <ircm/taxreturn/TaxReturn.hpp>
// #include <repast_hpc/Properties.h>
// #include <ircm/IrcmObserver.hpp>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/impl/ModelType.hpp> // for specifying model metadata

#include <efscape/utils/type.hpp> // for accessing the model class name

namespace zombie
{
// library name: libzombieserver
char const gcp_libname[] = "libzombieserver";

// typedef for exported class
typedef ::efscape::impl::RelogoWrapper<ZombieObserver, repast::relogo::Patch> ZombieWrapper;

/**
   */
class ZombieType : public efscape::impl::ModelType
{
public:
  ZombieType() : efscape::impl::ModelType(efscape::utils::type<ZombieWrapper>(),
                                          "The ZombieWrapper class wraps the Repast HPC Relogo zombie model.",
                                          gcp_libname,
                                          1)
  {
    //========================================================================
    // output ports:
    // * "out": array
    //========================================================================
    Json::Value lC_portValue;
    lC_portValue = Json::Value(Json::arrayValue);
    addOutputPort("out", lC_portValue);

    //========================================================================
    // properties
    //========================================================================
    Json::Value lC_properties;
    lC_properties["stopAt"] = 10;
    lC_properties["human.count"] = 500;
    lC_properties["zombie.count"] = 5;
    lC_properties["min.x"] = -100;
    lC_properties["min.y"] = -100;
    lC_properties["max.x"] = 99;
    lC_properties["max.y"] = 99;
    lC_properties["grid.buffer"] = 1;
    lC_properties["proc.per.x"] = 1;
    lC_properties["proc.per.y"] = 1;
    lC_properties["distribution.zombie.move"] = "int_uniform, 0, 2";
    setProperties(lC_properties);

    // additional attributes
    mC_attributes["modelName"] = "ef_relogo.zombie";
  }
};

} // namespace zombie

BOOST_CLASS_IS_WRAPPER(zombie::ZombieWrapper)

efscape::impl::DEVS *createZombieWrapper(Json::Value aC_args)
{
  return (dynamic_cast<efscape::impl::DEVS *>(new zombie::ZombieWrapper(aC_args)));
}

namespace efscape
{
namespace impl
{
const bool lb_ZombieWrapper_registered =
    Singleton<ModelHomeI>::Instance().getModelFactory().registerTypeWithArgs(zombie::ZombieType().typeName(),
                                                                             createZombieWrapper,
                                                                             zombie::ZombieType().toJSON());
}
} // namespace efscape
