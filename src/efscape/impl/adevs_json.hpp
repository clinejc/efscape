// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_ADEVS_JSON_HPP
#define EFSCAPE_IMPL_ADEVS_JSON_HPP

// efscape definitions
#include <efscape/impl/adevs_config.hpp>
#include <efscape/impl/ClockI.hpp>

#include <boost/property_tree/ptree.hpp>
#include <json/json.h>

#include <cereal/cereal.hpp>

#include <string>

// cereal serialization
namespace cereal {
  //
  // cereal serialization for ClockI
  //
  template<class Archive>
  void load(Archive & ar, efscape::impl::ClockI& aCr_clock)
  {
    double ld_time;
    double ld_timeDelta;
    double ld_timeMax;
    std::string lC_timeUnits;
    
    ar( cereal::make_nvp("time", ld_time),
	cereal::make_nvp("time_delta", ld_timeDelta),
	cereal::make_nvp("time_max", ld_timeMax),
	cereal::make_nvp("time_units", lC_timeUnits) );
    
    aCr_clock.time() = ld_time;
    aCr_clock.timeDelta() = ld_timeDelta;
    aCr_clock.timeMax() = ld_timeMax;
    aCr_clock.timeUnits(lC_timeUnits.c_str() );
  }

  template<class Archive>
  void save(Archive & ar, const efscape::impl::ClockI& aCr_clock)
  {
    double ld_time = aCr_clock.time();
    double ld_timeDelta = aCr_clock.timeDelta();
    double ld_timeMax = aCr_clock.timeMax();
    std::string lC_timeUnits = aCr_clock.timeUnits();
    
    ar( cereal::make_nvp("time", ld_time),
	cereal::make_nvp("time_delta", ld_timeDelta),
	cereal::make_nvp("time_max", ld_timeMax),
	cereal::make_nvp("time_units", lC_timeUnits) );
  }
  
} // namespace cereal

namespace efscape {

  namespace impl {

    /**
     * This utility function parses JSON data in a JSON value and attempts
     * to build the specified adevs model.
     *
     * @param aC_config JSON value containing model configuration
     * @returns handle to model 
     */   
    DEVS* createModelFromJSON(const Json::Value& lC_config);
    
    /**
     * This utility function loads information from a JSON file.
     * 
     * @param aC_path relative path of the JSON file
     * @returns JSON property tree
     */
    boost::property_tree::ptree loadInfoFromJSON(std::string aC_path);

    //
    // Some utility functions and classes for working with model JSON data
    //

    /**
     * Parses clock parameters from a JSON object.
     *
     * @param aCr_value json value
     * @param aCr_clock reference to clock
     */
    void convert_from_json(const Json::Value& aCr_value, ClockI& aCr_clock);

    /**
     * Copies clock parameters to a JSON object
     *
     * @param aCr_clock reference to clock
     * @returns JSON object with time info
     */
    Json::Value convert_to_json(const ClockI& aCr_clock);

    /**
     * A simple class that provides scaffolding for building a Digraph
     * from a JSON configuration data.
     */
    class DigraphBuilder {
    public:
      /** digraph node */
      struct node {
	/** default constructor */
	node() :
	  model(""),
	  port("") {}

	/**
	 * constructor
	 *
	 * @param aModel name of model
	 * @param aPort name of port
	 */
	node(std::string aModel, std::string aPort) :
	  model(aModel),
	  port(aPort) {}

	/**
	 * constructor
	 *
	 * @param aNode node to be copied
	 */
	node(const node& aNode) :
	  model(aNode.model),
	  port(aNode.port) {}

	/** model name */
	std::string model;

	/** port name */
	std::string port;

	/**
	 * Converts the node from JSON format
	 *
	 * @param aCr_value JSON object containing node parameters
	 */
	void convert_from_json(const Json::Value& aCr_value) {
	  this->model = aCr_value["model"].asString();
	  this->port = aCr_value["port"].asString();
	}

	/**
	 * Returns parameters for this node in JSON format.
	 *
	 * @returns JSON object containing node parameters
	 */
	Json::Value convert_to_json() const {
	  Json::Value lC_value;

	  lC_value["model"] = this->model;
	  lC_value["port"] = this->port;

	  return lC_value;
	}
      };

      /** digraph node coupling */
      struct edge {
	/** source node */
	node from;

	/** destination node */
	node to;

	/** default constructor */
	edge() :
	  from("",""),
	  to("","") {}

	/**
	 * constructor
	 *
	 * @param source node
	 * @param destination node
	 */
	edge(node fromNode, node toNode) :
	  from(fromNode),
	  to(toNode) {}

	/*
	 * constructor
	 *
	 * @param modelFrom name of source model
	 * @param portFrom source port
	 * @param modelTo name of destination model
	 * @param portTo destination port
	 */
	edge(std::string modelFrom, std::string portFrom,
		 std::string modelTo, std::string portTo) :
	  from(modelFrom, portFrom),
	  to(modelTo, portTo) {}

	/**
	 * copy constructor
	 *
	 * @param aEdge edge to be copied
	 */
	edge(const edge& aEdge) :
	  from(aEdge.from),
	  to(aEdge.to) {}

	/**
	 * Loads the coupling parameters from a JSON object.
	 *
	 * @param aCr_value JSON object containing coupling parameters
	 */
	void convert_from_json(const Json::Value& aCr_value) {
	  if (aCr_value["from"].isObject())
	    this->from.convert_from_json(aCr_value["from"]);
	  if (aCr_value["to"].isObject())
	    this->to.convert_from_json(aCr_value["to"]);
	}

	/**
	 * Returns the coupling parameters in a JSON object
	 *
	 * @returns JSON object containing coupling parameters
	 */
	Json::Value convert_to_json() const {
	  Json::Value lC_value;

	  lC_value["from"] = this->from.convert_to_json();
	  lC_value["to"] = this->to.convert_to_json();

	  return lC_value;
	}
      };

      /** constructor */
      DigraphBuilder() {}

      /**
       * Adds a model to the DigraphBuilder.
       *
       * @param name name of the model
       * @param modelValue JSON object containing the model configuration
       */
      void add(std::string name, Json::Value modelValue)
      {
	mCC_models[name] = modelValue;
      }
      
      /**
       * Add a coupling between two models.
       *
       * @param modelFrom name of source model
       * @param portFrom source port
       * @param modelTo name of destination model
       * @param portTo destination port
       */     
      void coupling(std::string modelFrom, std::string portFrom,
		    std::string modelTo, std::string portTo)
      {
	struct edge lC_edge(modelFrom, portFrom,
			    modelTo, portTo);
	mC1_couplings.push_back(lC_edge);
      }

      /**
       * Converts the DigraphBuilder to JSON.
       *
       * @returns JSON value containing Digraph configuration
       */
      Json::Value convert_to_json() const;

      /**
       * Builds a digraph from a JSON object
       *
       * @param aCr_value JSON value
       * @param aCp_digraph pointer to digraph
       * @returns pointer to DEVS model if successfull
       */
      static DEVS* build_digraph_from_json(const Json::Value& aCr_value,
					   DIGRAPH* aCp_digraph);

    private:
      /** map of models */
      std::map<std::string, Json::Value> mCC_models;

      /** array of digraph couplings */
      std::vector<edge> mC1_couplings;
	
    };				// DigraphBuilder
    
  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_ADEVS_JSON_HPP

