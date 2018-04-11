// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_json.hpp
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
#ifndef EFSCAPE_IMPL_ADEVS_JSON_HPP
#define EFSCAPE_IMPL_ADEVS_JSON_HPP

// efscape definitions
#include <efscape/impl/adevs_config.hpp>
#include <json/json.h>
#include <string>

namespace efscape {

  namespace impl {

    /**
     * This utility function parses JSON data in a JSON value and attempts
     * to build the specified adevs model.
     *
     * @param aC_config JSON value containing model configuration
     * @returns handle to model 
     */   
    DEVS* buildModelFromJSON(Json::Value lC_config);
    
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

