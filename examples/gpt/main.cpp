#include <cstdlib>
#include <iostream>
#include <efscape/impl/adevs_config.hpp>

#include "job.h"
#include "job.hpp"

#include "proc.h"
#include "proc.hpp"

#include "genr.h"
#include "genr.hpp"

#include "transd.h"
#include "transd.hpp"

using namespace std;

#include <adevs/adevs_cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

#include <map>

namespace cereal
{
  
  template <class Archive> 
  struct specialize<Archive, adevs::Atomic<PortValue>, cereal::specialization::non_member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, adevs::Network<PortValue>, cereal::specialization::non_member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, genr, cereal::specialization::member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, proc, cereal::specialization::member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, transd, cereal::specialization::member_serialize> {};

  template <class Archive> 
  struct specialize<Archive, adevs::Digraph<job>, cereal::specialization::non_member_load_save> {};

  template<class Archive>
  void serialize(Archive & ar, adevs::Digraph<job>::nodeplus& node)
  {
    ar( cereal::make_nvp("model", node.model),
	cereal::make_nvp("port", node.port) );
  }

}

#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
CEREAL_REGISTER_TYPE(adevs::Devs<PortValue>);
CEREAL_REGISTER_TYPE(adevs::Atomic<PortValue>);
CEREAL_REGISTER_TYPE(adevs::Network<PortValue>);
CEREAL_REGISTER_TYPE(adevs::Digraph<job>);
CEREAL_REGISTER_TYPE(genr);
CEREAL_REGISTER_TYPE(proc);
CEREAL_REGISTER_TYPE(transd);
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Devs<PortValue>,
				     adevs::Atomic<PortValue> );
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Devs<PortValue>,
				     adevs::Network<PortValue> );
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Network<PortValue>,
				     adevs::Digraph<job> );
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Atomic<PortValue>,
				     genr );
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Atomic<PortValue>,
				     proc );
CEREAL_REGISTER_POLYMORPHIC_RELATION(adevs::Atomic<PortValue>,
				     transd );

int main() 
{
  //
  // 2017-06-20: testing cereal serialization
  //
  cereal::JSONOutputArchive ar( std::cout );

  /// Get experiment parameters
  double g, p, t;
  cout << "Genr period: ";
  cin >> g;
  cout << "Proc time: ";
  cin >> p;
  cout << "Observation time: ";
  cin >> t;
  
  /// Create and connect the atomic components using a digraph model.
  std::shared_ptr<adevs::Digraph<job> > model(new adevs::Digraph<job>());
  efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH();
  std::shared_ptr<efscape::impl::DEVS> lCp_rootmodel( lCp_digraph );
      
  genr* gnr = new genr(g);
  gpt::genr* lCp_gnr = new gpt::genr(g);
  
  transd* trnsd = new transd(t);
  gpt::transd* lCp_trnsd = new gpt::transd(t);
  
  proc* prc = new proc(p);
  gpt::proc* lCp_prc = new gpt::proc(p);

  /// Add the components to the digraph
  model->add(gnr);
  model->add(trnsd);
  model->add(prc);

  lCp_digraph->add(lCp_gnr);
  lCp_digraph->add(lCp_trnsd);
  lCp_digraph->add(lCp_prc);

  /// Establish component coupling
  model->couple(gnr, gnr->out, trnsd, trnsd->ariv);
  model->couple(gnr, gnr->out, prc, prc->in);
  model->couple(prc, prc->out, trnsd, trnsd->solved);
  model->couple(trnsd, trnsd->out, gnr, gnr->stop);
  
  lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_trnsd, lCp_trnsd->ariv);
  lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_prc, lCp_prc->in);
  lCp_digraph->couple(lCp_prc, lCp_prc->out, lCp_trnsd, lCp_trnsd->solved);
  lCp_digraph->couple(lCp_trnsd, lCp_trnsd->out, lCp_gnr, lCp_gnr->stop);

  /// Create a simulator for the model and run it until
  /// the model passivates.
  adevs::Simulator<PortValue> sim(model.get());
  // adevs::Simulator<efscape::impl::IO_Type> lCp_sim( lCp_digraph.get() );

  while (sim.nextEventTime() < DBL_MAX) {
    ar(cereal::make_nvp("proc", *prc) );
    std::cout << std::endl;
    
    sim.execNextEvent();
  }
  /// Done!

  // while (lCp_sim.nextEventTime() < DBL_MAX) {
  //   lCp_sim.execNextEvent();
  // }

  ///
  /// testing cereal serialization
  ///
  /// first with the "non-efscape" version of the model
  ar( cereal::make_nvp("model", model) );

  ///
  /// now with the "efscape" version of the model
  ///
  typedef efscape::impl::DEVS COMPONENT;
  typedef std::shared_ptr<efscape::impl::DEVS> ComponentSPtr;
  typedef std::set<COMPONENT*> ComponentPtrSet;
  typedef std::set<ComponentSPtr> ComponentSPtrSet;

  ///
  /// Test #1: serializing a single atomic model
  ///
  // ComponentSPtr lCp_gnr2 = lCp_digraph->getComponent(lCp_gnr);
  // std::cout << "\nTest #1:\n";
  // ar( cereal::make_nvp("gpt_genr", lCp_gnr2) );

  ///
  /// Test #2: serializing a set of models
  ///
  // ComponentPtrSet lCCp_models;
  // ComponentSPtrSet lCCsp_models;
  // lCp_digraph->getComponents( lCCp_models );

  // for (auto iter = lCCp_models.begin();
  //      iter != lCCp_models.end();
  //      iter++) {
  //   ComponentSPtr lCp_devs = digraph.getComponent(*iter);
  //   lCCsp_models.insert(lCp_devs);
  efscape::impl::saveAdevsToJSON(lCp_rootmodel,std::cout);

  return 0;
}
