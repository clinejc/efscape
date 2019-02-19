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
  { //========================================================================
    // input ports:
    //========================================================================
    addInputPort("setup_in", Json::Value(Json::nullValue));

    //========================================================================
    // output ports:
    // * "properties_out": object
    // * "patches_out": array of objects
    // * "breeds_out": array of turtle breeds (types)
    // * "turtles_out": array of objects
    //========================================================================
    // properties_out
    Json::Value lC_portValue = Json::Value(Json::objectValue);
    addOutputPort("properties_out", lC_portValue);

    // patches_out
    Json::Value lC_agentArray = Json::Value(Json::arrayValue);

    Json::Value lC_agentAttributes = Json::Value(Json::objectValue);
    lC_agentAttributes["id"] = "";
    lC_agentAttributes["type"] = 0;
    lC_agentAttributes["pxCor"] = 0;
    lC_agentAttributes["pyCor"] = 0;
    lC_agentArray[0] = lC_agentAttributes;
    addOutputPort("patches_out", lC_agentArray);

    // breeds_out
    Json::Value lC_breedArray = Json::Value(Json::arrayValue);
    Json::Value lC_breed;
    lC_breed["type"] = "human";
    lC_breed["id"] = 0;
    lC_breedArray[0] = lC_breed;
    lC_breed["type"] = "zombie";
    lC_breed["id"] = 1;
    lC_breedArray[1] = lC_breed;
    addOutputPort("breeds_out", lC_breedArray);

    // turtles_out
    lC_agentAttributes["xCor"] = 0;
    lC_agentAttributes["yCor"] = 0;
    lC_agentArray[0] = lC_agentAttributes;
    addOutputPort("turtles_out", lC_agentArray);

    //========================================================================
    // properties
    //========================================================================
    Json::Value lC_properties;
    lC_properties["stop.at"] = 10;
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

    // portrayal list for visualization
    Json::Value lC_visualization;
    Json::Value lC_portrayal;

    lC_portrayal["Color"] = "blue";
    lC_portrayal["Shape"] = "circle";
    lC_portrayal["r"] = 3;
    lC_visualization["human"] = lC_portrayal;

    lC_portrayal["Color"] = "red";
    lC_portrayal["Shape"] = "circle";
    lC_portrayal["r"] = 3;
    lC_visualization["zombie"] = lC_portrayal;
    
    mC_attributes["visualization"] = lC_visualization;

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
