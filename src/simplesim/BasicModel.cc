// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/wrapper.hpp>

#include "simplesim/BasicModel.hh"
#include "simplesim/SimpleGenerator.hh"
#include "simplesim/SimpleObserver.hh"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>
#include <efscape/impl/ModelFactory.ipp>

#include <fstream>

BOOST_CLASS_EXPORT(simplesim::SimpleGenerator)
BOOST_CLASS_EXPORT(simplesim::SimpleObserver)

namespace simplesim {

  char const gcp_libname[] = "simplesim";


  /**
   * Creates a new CccgsemWrapper model
   *
   * @returns a handle to a new CcgsemWrapper
   */
  efscape::impl::DEVS* create_BasicModel() {
    return (efscape::impl::DEVS*)new SimpleGenerator();
  }

  const bool registered = efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    GetModelFactory().
      RegisterModel<SimpleGenerator>("A simple generator",
				     simplesim::gcp_libname);


} // end of namespace simplesim
