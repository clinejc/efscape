#include <cstdlib>
#include <iostream>
#include <efscape/impl/efscapelib.hpp>
#include <efscape/impl/SimRunner.hpp>

#include "job.h"
#include "job.hpp"

#include "proc.h"
#include "proc.hpp"

#include "genr.h"
#include "genr.hpp"

#include "transd.h"
#include "transd.hpp"

using namespace std;

#include <adevs_cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

#include <map>
#include <sstream>
#include <fstream>

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
  /// Get experiment parameters
  double g, p, t;
  cout << "Genr period: ";
  cin >> g;
  cout << "Proc time: ";
  cin >> p;
  cout << "Observation time: ";
  cin >> t;

  /// "non-efscape" version of the gpt coupled model
  {
    /// Create and connect the atomic components using a digraph model.
    genr* gnr = new genr(g); 
    transd* trnsd = new transd(t);
    proc* prc = new proc(p);

    std::shared_ptr<adevs::Digraph<job> > model(new adevs::Digraph<job>());

    /// Add the components to the digraph
    model->add(gnr);
    model->add(trnsd);
    model->add(prc);

    /// Establish component coupling
    model->couple(gnr, gnr->out, trnsd, trnsd->ariv);
    model->couple(gnr, gnr->out, prc, prc->in);
    model->couple(prc, prc->out, trnsd, trnsd->solved);
    model->couple(trnsd, trnsd->out, gnr, gnr->stop);

    /// Create a simulator for the model and run it until
    /// the model passivates.
    adevs::Simulator<PortValue> sim(model.get());

    std::cout << "Running the \"non-efscape\" version of the gpt model..\n";
    while (sim.nextEventTime() < DBL_MAX) {
      sim.execNextEvent();
    }
    /// Done!

    ///
    /// testing cereal serialization with the "non-efscape" version
    ///
    std::cout << "Testing cereal serialization with \"non=efscape\" version...\n";
    std::ofstream os00("gpt.json");
    cereal::JSONOutputArchive ar00( os00 );

    ar00( cereal::make_nvp("model", model) );
  }

  /// "efscape" version of gpt coupled model
  {
    /// Create and connect the atomic components using a digraph model.
    gpt::genr* lCp_gnr = new gpt::genr(g);
    gpt::transd* lCp_trnsd = new gpt::transd(t);
    gpt::proc* lCp_prc = new gpt::proc(p);

    efscape::impl::DIGRAPH* lCp_digraph = new efscape::impl::DIGRAPH();

    /// Add the components to the digraph
    lCp_digraph->add(lCp_gnr);
    lCp_digraph->add(lCp_trnsd);
    lCp_digraph->add(lCp_prc);

    /// Establish component coupling
    lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_trnsd, lCp_trnsd->ariv);
    lCp_digraph->couple(lCp_gnr, lCp_gnr->out, lCp_prc, lCp_prc->in);
    lCp_digraph->couple(lCp_prc, lCp_prc->out, lCp_trnsd, lCp_trnsd->solved);
    lCp_digraph->couple(lCp_trnsd, lCp_trnsd->out, lCp_gnr, lCp_gnr->stop);

    /// Add additional component coupling to root model output
    lCp_digraph->couple(lCp_trnsd, lCp_trnsd->log,
    			lCp_digraph, "gpt_log");
 
    /// Create a simulator for the model and run it until
    /// the model passivates.
    efscape::impl::DEVSPtr lCp_rootmodel(lCp_digraph);
      
    efscape::impl::SimRunner* lCp_simRunner =
      new efscape::impl::SimRunner();
    lCp_simRunner->setWrappedModel(lCp_rootmodel);

    lCp_rootmodel.reset(lCp_simRunner);   
    adevs::Simulator<efscape::impl::IO_Type> lCp_sim(lCp_rootmodel.get());
    
    std::cout << "Running the \"efscape\" version of the gpt model...\n";
    while (lCp_sim.nextEventTime() < DBL_MAX) {
      lCp_sim.execNextEvent();

      adevs::Bag< efscape::impl::IO_Type > lCC_output;
      efscape::impl::get_output(lCC_output, lCp_rootmodel.get());
      adevs::Bag< efscape::impl::IO_Type>::iterator
      	i = lCC_output.begin();
      for ( ; i != lCC_output.end(); i++) {
	std::cout << "Processing event on port<"
		  << (*i).port << ">...\n";

	if ((*i).port == "gpt_log") {
	  try {
	    Json::Value lC_messages =
	      boost::any_cast<Json::Value>( (*i).value );
	    std::cout << "rootmodel output=>"
		      << lC_messages << std::endl;
	  }
	  catch(const boost::bad_any_cast &) {
	    std::cout << "Unable to cast input as <Json::Value>\n";
	  }
	}
      }	// if ((*i).model ==...
    }
    /// Done!

    ///
    /// testing cereal serialization with the "efscape" version
    ///
    std::cout << "Testing cereal serialization with \"efscape\" version...\n";

    /// save the model data to a string stream
    stringstream ss;
    {
      efscape::impl::saveAdevsToJSON(lCp_rootmodel,ss);
      
      /// And save the data from the original model to a file
      std::ofstream os01("gpt_efscape01.json");
      os01 << ss.str() << std::endl;
    }

    /// clone the original model by loading the model data from the string stream
    efscape::impl::DEVSPtr lCp_modelclone;

    {
      lCp_modelclone = efscape::impl::loadAdevsFromJSON(ss);

      /// save the data from the "cloned" model to another file
      std::ofstream os02("gpt_efscape02.json");
      efscape::impl::saveAdevsToJSON(lCp_modelclone, os02);
      os02 << std::endl;
    }
  }
  
 



  return 0;
}
