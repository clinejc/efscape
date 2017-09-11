// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscape_cereal.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

// c++ cereal definitions
#include <adevs/adevs_cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

#include <efscape/impl/efscape_cereal.hpp>

// efscape adevs definitions
#include <efscape/impl/adevs_config.hpp>
#include <efscape/impl/SimRunner.hpp>

namespace cereal
{

  template <class Archive>
  struct specialize<Archive, efscape::impl::ATOMIC, cereal::specialization::non_member_serialize> {};

  template <class Archive>
  struct specialize<Archive, efscape::impl::NETWORK, cereal::specialization::non_member_serialize> {};

  template <class Archive>
  struct specialize<Archive, efscape::impl::DIGRAPH, cereal::specialization::non_member_load_save> {};

  template<class Archive>
  void serialize(Archive & ar, efscape::impl::DIGRAPH::nodeplus& node)
  {
    ar( cereal::make_nvp("model", node.model),
	cereal::make_nvp("port", node.port) );
  }

  template <class Archive>
  struct specialize<Archive, efscape::impl::ModelWrapperBase, cereal::specialization::non_member_load_save> {};

  template <class Archive>
  struct specialize<Archive, efscape::impl::SimRunner, cereal::specialization::member_serialize> {};

}

#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(efscape::impl::DEVS);
CEREAL_REGISTER_TYPE(efscape::impl::ATOMIC);
CEREAL_REGISTER_TYPE(efscape::impl::NETWORK);
CEREAL_REGISTER_TYPE(efscape::impl::DIGRAPH);
CEREAL_REGISTER_TYPE(efscape::impl::ModelWrapperBase);
CEREAL_REGISTER_TYPE(efscape::impl::SimRunner);
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::DEVS,
				     efscape::impl::ATOMIC );
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::DEVS,
				     efscape::impl::NETWORK );
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::NETWORK,
				     efscape::impl::DIGRAPH );
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::ATOMIC,
				     efscape::impl::ModelWrapperBase );
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::ModelWrapperBase,
				     efscape::impl::SimRunner );

// model factory definitions
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

namespace efscape {
  namespace impl {

    /**
     * This function attemps to serialize and save an adevs model hierarchy via
     * the cereal serialization library JSON archive
     *
     * @param aCp_model handle to model (reference)
     * @param aCr_ostream output stream
     */
    void saveAdevsToJSON(const DEVSPtr& aCp_model,
			 std::ostream& aCr_ostream)
     {
      // make an archive
      assert(aCr_ostream.good());
      cereal::JSONOutputArchive oa( aCr_ostream );

      oa( cereal::make_nvp("efscape",aCp_model) );
    }

    /**
     * This function attempts to load an adevs model hierarchy serialized with
     * the boost serialization library.
     *
     * @param aCr_istream reference to input stream
     * @returns handle to loaded model
     */
    DEVSPtr loadAdevsFromJSON(std::istream& aCr_istream)
    {
      DEVSPtr lCp_model;
      assert(aCr_istream.good());
      cereal::JSONInputArchive ia( aCr_istream );

      ia( cereal::make_nvp("efscape",lCp_model) );

      return lCp_model;
    }

  } // namespace impl

} // namespace efscape
