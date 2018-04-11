// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscape_serialization.cpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__

// boost serialization archive definitions
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/assume_abstract.hpp>

// class boost serialization export definitions
#include <adevs_serialization.hpp>
#include <efscape/impl/adevs_decorator_serialization.hpp>

#include <efscape/impl/SimRunner.hpp>

// definitions for accessing the model home
#include <efscape/impl/ModelHomeI.hpp>

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

BOOST_CLASS_IS_WRAPPER(efscape::impl::SimRunner)
BOOST_SERIALIZATION_SPLIT_FREE(efscape::impl::SimRunner)

BOOST_CLASS_EXPORT(efscape::impl::DEVS)
BOOST_CLASS_EXPORT(efscape::impl::ATOMIC)
BOOST_CLASS_EXPORT(efscape::impl::NetworkModel)
BOOST_CLASS_EXPORT(efscape::impl::ModelDecorator)

namespace boost {
 namespace serialization {
   template<class Archive>
   void serialize(Archive & ar, efscape::impl::DIGRAPH::nodeplus& node,
		   const unsigned int version)
   {
     ar & BOOST_SERIALIZATION_NVP(node.model); // serialize model
     ar & BOOST_SERIALIZATION_NVP(node.port); // serialize port
   }
 }
}
BOOST_CLASS_EXPORT(efscape::impl::DIGRAPH)
BOOST_CLASS_EXPORT(efscape::impl::ModelWrapperBase)

BOOST_CLASS_EXPORT(efscape::impl::CellDevs)
BOOST_CLASS_EXPORT(efscape::impl::CellModelBase)

// BOOST_CLASS_EXPORT(efscape::impl::EntityI)
// BOOST_CLASS_EXPORT(efscape::impl::AdevsModel)
BOOST_CLASS_EXPORT(efscape::impl::SimRunner)

namespace efscape {
  namespace impl {

    /**
     * This function attemps to serialize and save an adevs model hierarchy via
     * the boost serialization library.
     *
     * @param aCp_model handle to model (reference)
     * @param acp_filename file name
     */
    void saveAdevsToXML(const adevs::Devs<IO_Type>* aCp_model,
			const char* acp_filename)
    {
      // make an archive
      std::ofstream ofs(acp_filename);
      saveAdevsToXML(aCp_model, ofs);
      ofs.close();
    }

    /**
     * This function attemps to serialize and save an adevs model hierarchy via
     * the boost serialization library.
     *
     * @param aCp_model handle to model (reference)
     * @param aCr_ostream output stream
     */
    void saveAdevsToXML(const adevs::Devs<IO_Type>* aCp_model,
			std::ostream& aCr_ostream)
    {
      // make an archive
      try {
	// assert(aCr_ostream.good());
	boost::archive::xml_oarchive oa(aCr_ostream);

	oa << boost::serialization::make_nvp("efscape",aCp_model);

	// aCr_ostream << "</boost_serialization>";
      } catch(...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Exception encountered during serialization of adevs model");
      }
    }

    /**
     * This function attempts to load an adevs model hierarchy serialized with
     * the boost serialization library
     *
     * @param acp_filename file name
     * @returns handle to loaded model
     */
    adevs::Devs<IO_Type>* loadAdevsFromXML(const char* acp_filename)
    {
      std::ifstream ifs(acp_filename);
      return loadAdevsFromXML(ifs);
    }

    /**
     * This function attempts to load an adevs model hierarchy serialized with
     * the boost serialization library.
     *
     * @param aCr_istream reference to input stream
     * @returns handle to loaded model
     */
    adevs::Devs<IO_Type>* loadAdevsFromXML(std::istream& aCr_istream)
    {
      adevs::Devs<IO_Type>* lCp_model = nullptr;
      try {
	boost::archive::xml_iarchive ia(aCr_istream);
	ia >> BOOST_SERIALIZATION_NVP(lCp_model);
      } catch(...) {
	LOG4CXX_ERROR(ModelHomeI::getLogger(),
		      "Exception encountered during deserialization of adevs model");
      }
      return lCp_model;
    }

  } // namespace impl

}   // namespace efscape
