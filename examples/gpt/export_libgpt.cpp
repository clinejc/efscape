#include "job.hpp"
#include "genr.hpp"
#include "proc.hpp"
#include "transd.hpp"

#include <json/json.h>

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

#include <efscape/utils/type.hpp>

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

#include <boost/function.hpp>

namespace gpt {

  // library name: gpt
  char const gcp_libname[] = "gpt";

  /**
   * Creates a gpt coupled model.
   *
   * @param aC_args arguments embedded in JSON
   * @returns handle to a gpt model
   */
  efscape::impl::DEVS* createGptCoupledModel(Json::Value aC_args) {

    // (default) parameters
    double g = 1;
    double p = 2;
    double t = 10;

    if (aC_args.isObject()) {
      Json::Value lC_attribute = aC_args["genr_period"];
      if (lC_attribute.isDouble()) {
	g = lC_attribute.asDouble();
      } else {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Unable to parse attribute  <genr_period>")
      }
      
      lC_attribute = aC_args["proc_period"];
      if (lC_attribute.isDouble()) {
	p = lC_attribute.asDouble();
      } else {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Unable to parse attribute  <proc_period>")
      }
      
      lC_attribute = aC_args["observation_time"];
      if (lC_attribute.isDouble()) {
	t = lC_attribute.asDouble();
      } else {
	LOG4CXX_ERROR(efscape::impl::ModelHomeI::getLogger(),
		      "Unable to parse attribute  <observation_time>")
      }
    }
    
    /// Create and connect the atomic components using a digraph model.
    efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH();

    gpt::genr* lCp_gnr = new gpt::genr(g);
    gpt::transd* lCp_trnsd = new gpt::transd(t);
    gpt::proc* lCp_prc = new gpt::proc(p);

    /// Add the components to the digraph
    lCp_digraph->add(lCp_gnr);
    lCp_digraph->add(lCp_trnsd);
    lCp_digraph->add(lCp_prc);

    /// Establish component coupling
    lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_trnsd, lCp_trnsd->ariv);
    lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_prc, lCp_prc->in);
    lCp_digraph->couple(lCp_prc, lCp_prc->out, lCp_trnsd, lCp_trnsd->solved);
    lCp_digraph->couple(lCp_trnsd, lCp_trnsd->out, lCp_gnr, lCp_gnr->stop);

    return lCp_digraph;
  }

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

  // register gpt coupled model
  const bool lb_gpt_registered =
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().
    getModelFactory().
    registerTypeWithArgs(std::string("gpt::gpt_coupled_model"),
			 createGptCoupledModel);
}
