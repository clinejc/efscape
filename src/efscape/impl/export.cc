// __COPYRIGHT_START__
// Package Name : efscape
// File Name : export.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

// boost serialization archive definitions
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/assume_abstract.hpp>

// class boost serialization export definitions
#include <efscape/impl/adevs_serialization.hh>
#include <efscape/impl/EntityI.hh>
#include <efscape/impl/AdevsModel.hh>

// model factory definitions
#include <efscape/impl/ModelHomeI.hh>
#include <efscape/impl/ModelHomeSingleton.hh>
#include <efscape/impl/ModelFactory.ipp>

#include <fstream>

//----------------------------
// adevs serialization exports
//----------------------------
BOOST_CLASS_IS_WRAPPER(efscape::impl::DEVS)
// BOOST_IS_ABSTRACT(efscape::impl::DEVS)
BOOST_CLASS_IS_WRAPPER(efscape::impl::CellDevs)

BOOST_CLASS_IS_WRAPPER(efscape::impl::ATOMIC)
// BOOST_IS_ABSTRACT(efscape::impl::ATOMIC)
BOOST_CLASS_IS_WRAPPER(efscape::impl::CellModelBase)

BOOST_CLASS_IS_WRAPPER(efscape::impl::ModelWrapperBase)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::ModelWrapperBase)
// BOOST_IS_ABSTRACT(efscape::impl::AtomicWrapperBase)

BOOST_CLASS_IS_WRAPPER(efscape::impl::CellWrapperBase)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::CellWrapperBase)
// BOOST_IS_ABSTRACT(efscape::impl::CellWrapperBase)

BOOST_CLASS_IS_WRAPPER(efscape::impl::NetworkModel)
// BOOST_IS_ABSTRACT(efscape::impl::NetworkModel)

BOOST_CLASS_IS_WRAPPER(efscape::impl::ModelDecorator)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::ModelDecorator)

BOOST_CLASS_IS_WRAPPER(efscape::impl::SIMPLEDIGRAPH)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::SIMPLEDIGRAPH)

BOOST_CLASS_IS_WRAPPER(efscape::impl::DIGRAPH)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::DIGRAPH)

BOOST_CLASS_IS_WRAPPER(efscape::impl::CELLSPACE)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::CELLSPACE)

BOOST_CLASS_EXPORT(efscape::impl::DEVS)
BOOST_CLASS_EXPORT(efscape::impl::ATOMIC)
BOOST_CLASS_EXPORT(efscape::impl::NetworkModel)
BOOST_CLASS_EXPORT(efscape::impl::ModelDecorator)
BOOST_CLASS_EXPORT(efscape::impl::DIGRAPH)
BOOST_CLASS_EXPORT(efscape::impl::ModelWrapperBase)

BOOST_CLASS_EXPORT(efscape::impl::CellDevs)
BOOST_CLASS_EXPORT(efscape::impl::CellModelBase)

BOOST_CLASS_EXPORT(efscape::impl::EntityI)
BOOST_CLASS_EXPORT(efscape::impl::AdevsModel)

namespace efscape {
  namespace impl {

    /**
     * This function attemps to serialize and save an adevs model hierarchy via
     * the boost serialization library.
     *
     * @param aCp_model handle to model (reference)
     * @param acp_filename file name
     */
    void saveAdevs(const adevs::Devs<IO_Type>* aCp_model,
		   const char* acp_filename)
    {
      // make an archive
      std::ofstream ofs(acp_filename);
      saveAdevs(aCp_model, ofs);
      ofs.close();
    }

    /**
     * This function attemps to serialize and save an adevs model hierarchy via
     * the boost serialization library.
     *
     * @param aCp_model handle to model (reference)
     * @param aCr_ostream output stream
     */
    void saveAdevs(const adevs::Devs<IO_Type>* aCp_model,
		   std::ostream& aCr_ostream)
    {
      // make an archive
      assert(aCr_ostream.good());
      boost::archive::xml_oarchive oa(aCr_ostream);

      oa << boost::serialization::make_nvp("efscape",aCp_model);

      // aCr_ostream << "</boost_serialization>";
    }

    /**
     * This function attempts to load an adevs model hierarchy serialized with
     * the boost serialization library
     *
     * @param acp_filename file name
     * @returns handle to loaded model
     */
    adevs::Devs<IO_Type>* loadAdevs(const char* acp_filename)
    {
      std::ifstream ifs(acp_filename);
      return loadAdevs(ifs);
    }

    /**
     * This function attempts to load an adevs model hierarchy serialized with
     * the boost serialization library.
     *
     * @param aCr_istream reference to input stream
     * @returns handle to loaded model
     */
    adevs::Devs<IO_Type>* loadAdevs(std::istream& aCr_istream)
    {
      adevs::Devs<IO_Type>* lCp_model;
//       std::ifstream ifs(acp_filename);
      assert(aCr_istream.good());
      boost::archive::xml_iarchive ia(aCr_istream);
      ia >> BOOST_SERIALIZATION_NVP(lCp_model);
      return lCp_model;
    }

    //---------------
    // register class
    //---------------
    const bool
    lb_AdevsModel_registered =
	    Singleton<ModelHomeI>::Instance().
	    GetModelFactory().RegisterModel<AdevsModel>("{\"info\": \"A generic root model\"}",
							"{}",
							gcp_libname);

  } // namespace impl

}   // namespace efscape
