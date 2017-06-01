// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/wrapper.hpp>

#include <simplesim/BasicModel.hh>
#include <simplesim/SimpleGenerator.hh>
#include <simplesim/SimpleObserver.hh>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/impl/adevs_json.hpp>
#include <efscape/utils/type.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <fstream>

BOOST_CLASS_EXPORT(efscape::impl::ClockI)
BOOST_CLASS_EXPORT(simplesim::SimpleGenerator)
BOOST_CLASS_EXPORT(simplesim::SimpleObserver)

namespace simplesim {

  char const gcp_libname[] = "simplesim";

  /**
   * Creates SimpleGenerator class metadata
   *
   * @returns metadata in a JSON property tree
   */
  boost::property_tree::ptree create_SimpleGenerator_info() {
    boost::property_tree::ptree lC_ptree;
    lC_ptree.put("info",
                 "A simple generator");
    lC_ptree.put("library", std::string(gcp_libname) );

    return lC_ptree;
  }
  
  const bool lb_generator_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerType<SimpleGenerator>(efscape::utils::type<SimpleGenerator>(),
				  create_SimpleGenerator_info());

  /**
   * Creates SimpleObserver class metadata
   *
   * @returns metadata in a JSON property tree
   */
  boost::property_tree::ptree create_SimpleObserver_info() {
    boost::property_tree::ptree lC_ptree;
    lC_ptree.put("info",
                 "A simple observer");
    lC_ptree.put("library", std::string(gcp_libname) );

    return lC_ptree;
  }
  
  const bool lb_observer_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerType<SimpleGenerator>(efscape::utils::type<SimpleObserver>(),
				  create_SimpleObserver_info());

} // namespace simplesim
