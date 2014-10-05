// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_serialization.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_ADEVS_SERIALIZATION_HH
#define EFSCAPE_IMPL_ADEVS_SERIALIZATION_HH

#include <efscape/impl/adevs_config.hh>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/wrapper.hpp>

namespace boost {

  namespace serialization {

    /**
     * Free function for serialization of the adevs::Devs<X> class.
     *
     * @param ar reference to archive type
     * @param devs reference to DEVS model
     * @param version version number
     */
    template<class Archive, class X>
    void serialize(Archive & ar, adevs::Devs<X>& devs,
		   const unsigned int version)
    {
    }

    /**    
     * Free function for serialization of the adevs::Atomic<X> class.
     *
     * @param ar reference to archive type
     * @param atomic reference to ATOMIC model
     * @param version version number
     */
    template<class Archive, class X>
    void serialize(Archive & ar, adevs::Atomic<X>& atomic,
		   const unsigned int version)
    {
      // invoke serialization of the parent class
      ar & boost::serialization::make_nvp("Devs",
					  boost::serialization::base_object<adevs::Devs<X> >(atomic) );
    }

    /**
     * Free function for serialization save of the adevs::ModelWrapper<X,Y>
     * class.
     *
     * @param ar reference to archive type
     * @param wrapper reference to ModelWrapper model
     * @param version version number
     */
    template<class Archive, class ExternalType, class InternalType>
    void save(Archive & ar,
	      const adevs::ModelWrapper<ExternalType,InternalType>& wrapper,
	      const unsigned int version)
    {
      // invoke serialization save of the parent class
      ar & boost::serialization::make_nvp("Atomic",
					  boost::serialization::base_object<adevs::Atomic<ExternalType> >(wrapper) );

      // invoke serialization save of tL for wrapped model
      double tL = wrapper.getTime();
      ar & boost::serialization::make_nvp("tL", tL);

      // leave serialization of wrapped model to derived classes
    }

    /**
     * Free function for serialization load of the adevs::ModelWrapper<X,Y>
     * class.
     *
     * @param ar reference to archive type
     * @param wrapper reference to ModelWrapper model
     * @param version version number
     */
    template<class Archive, class ExternalType, class InternalType>
    void load(Archive & ar,
	      adevs::ModelWrapper<ExternalType,InternalType>& wrapper,
	      const unsigned int version)
    {
      // invoke serialization load of the parent class
      ar & boost::serialization::make_nvp("Atomic",
					  boost::serialization::base_object<adevs::Atomic<ExternalType> >(wrapper) );

      // invoke serialization load of tL for wrapped model
      double tL;
      ar & boost::serialization::make_nvp("tL", tL);
      wrapper.setTime(tL);

      // leave serialization of wrapped model to derived classes
    }

    /**
     * Free function for serialization save of the adevs::Network<X> class.
     *
     * @param ar reference to archive type
     * @param network reference to NETWORK model
     * @param version version number
     */
    template<class Archive, class X>
    void serialize(Archive & ar, adevs::Network<X>& network,
		   const unsigned int version)
    {
      // invoke serialization of the parent class
      ar & boost::serialization::make_nvp("Devs",
					  boost::serialization::base_object<adevs::Devs<X> >(network) );
    }

    /**
     * Free function for serialization save of the adevs::ModelDecorator<X>
     * class.
     *
     * @param ar reference to archive type
     * @param decorator reference to ModelDecorator model
     * @param version version number
     */
    template<class Archive>
    void save(Archive & ar, const efscape::impl::ModelDecorator& decorator,
	      const unsigned int version)
    {
      // invoke serialization save of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK >(decorator) );

      // save wrapped model
      const efscape::impl::DEVS* lCp_model = decorator.getWrappedModel();
      ar  & boost::serialization::make_nvp("WrappedModel",lCp_model);
    }

    /**
     * Free function for serialization load of the adevs::ModelDecorator<X>
     * class.
     *
     * @param ar reference to archive type
     * @param atomic reference to ModelDecorator model
     * @param version version number
     */
    template<class Archive>
    void load(Archive & ar, efscape::impl::ModelDecorator& decorator,
	      const unsigned int version)
    {
      // invoke serialization load of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK >(decorator) );

      // save wrapped model
      efscape::impl::DEVS* lCp_model;
      ar  & boost::serialization::make_nvp("WrappedModel",lCp_model);
      decorator.setWrappedModel(lCp_model);
    }

    /**
     * Free function for serialization save of the SIMPLEDIGRAPH class.
     *
     * @param ar reference to archive type
     * @param digraph reference to SIMPLEDIGRAPH model
     * @param version version number
     */
    template<class Archive>
    void save(Archive & ar, const efscape::impl::SIMPLEDIGRAPH& digraph,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK>(digraph) );

      //-------------------
      // 2) save sub models
      //-------------------
      // get digraph components
      typedef efscape::impl::DEVS COMPONENT;
      typedef adevs::Set<const COMPONENT*> ComponentSet;
      ComponentSet lCCp_models;
      digraph.getComponents(lCCp_models);

      // copy adevs::Set<> to std::set<>
      typedef std::set<const COMPONENT*> ComponentStlSet;
      ComponentStlSet models;
      for (ComponentSet::iterator iter = lCCp_models.begin();
      	   iter != lCCp_models.end();
      	   iter++)
      	models.insert(*iter);

      // save component models
      ar & BOOST_SERIALIZATION_NVP(models);

      //--------------------
      // 3) save model graph
      //--------------------
      // get graph
      typedef std::map< const COMPONENT*, adevs::Bag<const COMPONENT*> > GRAPH;
      GRAPH lC_graph;
      digraph.getGraph(lC_graph);

      // copy adevs graph to stl graph
      typedef std::map< const COMPONENT*, std::set<const COMPONENT*> > StlGraph;
      StlGraph graph;		// graph to serialize
      for (GRAPH::iterator graph_iter = lC_graph.begin();
      	   graph_iter != lC_graph.end(); graph_iter++) {

      	for (adevs::Bag<const COMPONENT*>::iterator model_iter = graph_iter->second.begin();
      	     model_iter != graph_iter->second.end();
      	     model_iter++) {
      	  graph[graph_iter->first].insert(*model_iter);
      	}
      }

      // save graph
      ar & BOOST_SERIALIZATION_NVP(graph);

    } // serialization save of SIMPLEDIGRAPH class

    /**
     * Free function for serialization load of the SIMPLEDIGRAPH class.
     *
     * @param ar reference to archive type
     * @param digraph reference to SIMPLEDIGRAPH model
     * @param version version number
     */
    template<class Archive>
    void load(Archive & ar, efscape::impl::SIMPLEDIGRAPH& digraph,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK>(digraph) );

      // 2) load component models
      typedef efscape::impl::DEVS COMPONENT;
      typedef std::set<COMPONENT*> ComponentStlSet;
      ComponentStlSet models;
      ar & BOOST_SERIALIZATION_NVP(models);

      // add component models to digraph
      for (ComponentStlSet::iterator iter = models.begin();
	   iter != models.end();
	   iter++)
	digraph.add(*iter);

      // 3) add model couplings
      typedef std::map< COMPONENT*, std::set<COMPONENT*> > StlGraph;
      StlGraph graph;		// graph to serialize

      // load graph
      ar & BOOST_SERIALIZATION_NVP(graph);

      for (StlGraph::iterator graph_iter = graph.begin() ;
	   graph_iter != graph.end(); graph_iter++) {
	for (typename std::set<efscape::impl::DEVS*>::iterator model_iter =
	       graph_iter->second.begin();
	     model_iter != graph_iter->second.end();
	     model_iter++) {

	  digraph.couple(graph_iter->first, *model_iter);
	} // for (typename std::set<Node>
      }	// for (typename Graph::iterator = ...

    } // serialization load of SIMPLEDIGRAPH

    /**
     * Free function for serialization of the DIGRAPH::node struct.
     *
     * @param ar reference to archive type
     * @param node reference to graph node
     * @param version version number
     */
    template<class Archive>
    void serialize(Archive & ar, efscape::impl::DIGRAPH::node& node,
		   const unsigned int version)
    {
      ar & BOOST_SERIALIZATION_NVP(node.model); // serialize model
      ar & BOOST_SERIALIZATION_NVP(node.port); // serialize port
    }

    /**
     * Free function for serialization save of the DIGRAPH class.
     *
     * @param ar reference to archive type
     * @param digraph reference to DIGRAPH model
     * @param version version number
     */
    template<class Archive>
    void save(Archive & ar, const efscape::impl::DIGRAPH& digraph,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK>(digraph) );

      //-------------------
      // 2) save sub models
      //-------------------
      // get digraph components
      typedef adevs::Set<const efscape::impl::DEVS*> ComponentSet;
      ComponentSet lCCp_models;
      digraph.getComponents(lCCp_models);

      // copy adevs::Set<> to std::set<>
      typedef std::set<const efscape::impl::DEVS*> ComponentStlSet;
      ComponentStlSet models;
      for (ComponentSet::iterator iter = lCCp_models.begin();
	   iter != lCCp_models.end();
	   iter++)
	models.insert(*iter);

      // save component models
      ar & BOOST_SERIALIZATION_NVP(models);

      //--------------------
      // 3) save model graph
      //--------------------
      // get graph
      typedef efscape::impl::DIGRAPH::node NODE;
      typedef std::map< NODE, adevs::Bag<NODE> > GRAPH;
      GRAPH lC_graph;
      digraph.getGraph(lC_graph);

      // copy adevs graph to stl graph
      typedef std::map< NODE, std::set<NODE> > StlGraph;
      StlGraph graph;		// graph to serialize
      for (GRAPH::iterator graph_iter = lC_graph.begin();
	   graph_iter != lC_graph.end(); graph_iter++) {

	for (adevs::Bag<NODE>::iterator node_iter = graph_iter->second.begin();
	     node_iter != graph_iter->second.end();
	     node_iter++) {
	  graph[graph_iter->first].insert(*node_iter);
	}
      }

      // save graph
      ar & BOOST_SERIALIZATION_NVP(graph);

    } // serialization save of the DIGRAPH class

    /**
     * Free function for serialization load of the DIGRAPH class.
     *
     * @param ar reference to archive type
     * @param digraph reference to DIGRAPH model
     * @param version version number
     */
    template<class Archive>
    void load(Archive & ar, efscape::impl::DIGRAPH& digraph,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::NETWORK>(digraph) );

      // 2) load component models
      typedef std::set<efscape::impl::DEVS*> ComponentStlSet;
      ComponentStlSet models;
      ar & BOOST_SERIALIZATION_NVP(models);

      // add component models to digraph
      for (ComponentStlSet::iterator iter = models.begin();
	   iter != models.end();
	   iter++)
	digraph.add(*iter);

      // 3) add model couplings
      typedef efscape::impl::DIGRAPH::node NODE;
      typedef std::map< NODE, std::set<NODE> > StlGraph;
      StlGraph graph;		// graph to serialize

      // save component models
      ar & BOOST_SERIALIZATION_NVP(graph);

      for (StlGraph::iterator graph_iter = graph.begin() ;
	   graph_iter != graph.end(); graph_iter++) {
	for (typename std::set<NODE>::iterator node_iter =
	       graph_iter->second.begin();
	     node_iter != graph_iter->second.end();
	     node_iter++) {

	  digraph.couple(graph_iter->first.model, graph_iter->first.port,
			 node_iter->model, node_iter->port);
	} // for (typename std::set<Node>
      }	// for (typename Graph::iterator = ...

    } // serialization load of DIGRAPH class

    /**
     * Free function for serialization save of the CELLSPACE class.
     *
     * @param ar reference to archive type
     * @param cellspace reference to CELLSPACE model
     * @param version version number
     */
    template<class Archive>
    void save(Archive & ar, const efscape::impl::CELLSPACE& cellspace,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::CellNetwork>(cellspace) );

      // 2) save the dimensions of the cell space
      long width = cellspace.getWidth();
      ar & BOOST_SERIALIZATION_NVP(width);

      long height = cellspace.getHeight();
      ar & BOOST_SERIALIZATION_NVP(height);

      long depth = cellspace.getDepth();
      ar & BOOST_SERIALIZATION_NVP(depth);

      //-------------------
      // 3) save sub models
      //-------------------
      // get cellspace components
      std::vector<const efscape::impl::CellDevs*> models;

      long ll_volume = width*height*depth;
      for (int x = 0; x < width; x++) {
	for (int y = 0; y < height; y++) {
	  for (int z = 0; z < depth; z++) {
	    models.push_back(cellspace.getModel(x,y,z));
	  }
	}
      }

      // save component models
      ar & BOOST_SERIALIZATION_NVP(models);

    }

    /**
     * Free function for serialization save of the CELLSPACE class.
     *
     * @param ar reference to archive type
     * @param cellspace reference to CELLSPACE model
     * @param version version number
     */
    template<class Archive>
    void load(Archive & ar, efscape::impl::CELLSPACE& cellspace,
	      const unsigned int version)
    {
      // 1) invoke serialization of the parent class
      ar & boost::serialization::make_nvp("NetworkModel",
					  boost::serialization::base_object<efscape::impl::CellNetwork>(cellspace) );

      // 2) load the dimensions of the cell space and initialize
      long width;
      ar & BOOST_SERIALIZATION_NVP(width);

      long height;
      ar & BOOST_SERIALIZATION_NVP(height);

      long depth;
      ar & BOOST_SERIALIZATION_NVP(depth);

      cellspace.createSpace(width,height,depth);

      // 3) load component models
      std::vector<efscape::impl::CellDevs*> models;
      ar & BOOST_SERIALIZATION_NVP(models);

      long ll_volume = width*height*depth;
      long index = 0;
      for (int x = 0; x < width; x++) {
	for (int y = 0; y < height; y++) {
	  for (int z = 0; z < depth; z++) {
	    if (index < models.size())
	      cellspace.add( models[index++], x, y, z );
	  }
	}
      }
    }

  } // namespace serialization

}   // namespace boost

#endif	// ifndef EFSCAPE_IMPL_ADEVS_SERIALIZATION_HH
