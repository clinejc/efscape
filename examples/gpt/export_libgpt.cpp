// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include "job.hpp"
#include "genr.hpp"
#include "proc.hpp"
#include "transd.hpp"

#include <adevs/adevs_cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

namespace cereal
{
  template <class Archive> 
  struct specialize<Archive, gpt::genr, cereal::specialization::member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, gpt::proc, cereal::specialization::member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, gpt::transd, cereal::specialization::member_serialize> {};
}

#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
CEREAL_REGISTER_TYPE(gpt::genr);
CEREAL_REGISTER_TYPE(gpt::proc);
CEREAL_REGISTER_TYPE(gpt::transd);
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::ATOMIC,
				     gpt::genr);
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::ATOMIC,
				     gpt::proc);
CEREAL_REGISTER_POLYMORPHIC_RELATION(efscape::impl::ATOMIC,
				     gpt::transd);

namespace gpt {

  // library name: gpt
  char const gcp_libname[] = "gpt";

  // register genr model
  const bool lb_genr_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerType<genr>(efscape::utils::type<genr>());

  // register proc model
  const bool lb_proc_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerType<proc>(efscape::utils::type<proc>());
  
  // register transd model
  const bool lb_transd_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerType<transd>(efscape::utils::type<transd>());
}
