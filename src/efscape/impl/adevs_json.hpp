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

#include <picojson_serializer.h>
#include <picojson_set_serializer.h>
#include <picojson_vector_serializer.h>

#include <string>

namespace efscape {

  namespace impl {

   /**
     * This utility function parses JSON data in a property tree and attempts
     * to build the specified adevs model.
     *
     * @param aC_pt property tree containing the model configuration
     * @returns handle to model 
     */
    DEVS* createModelFromJSON(const boost::property_tree::ptree& aC_pt);

    /**
     * This utility function loads information from a JSON file.
     * 
     * @param aC_path relative path of the JSON file
     * @returns JSON property tree
     */
    boost::property_tree::ptree loadInfoFromJSON(std::string aC_path);

    //
    // Some utility classes
    //
    /**
     * Contains information about the simulation time dimension
     */
    struct SimTime {
      /** time step */
      double delta_t;

      /** stopping time */
      double stopAt;

      /** time units */
      std::string units;

      friend class picojson::convert::access;
      template<class Archive>
      void json(Archive & ar) const
      {
	ar & picojson::convert::member("delta_t", delta_t);
	ar & picojson::convert::member("stopAt", stopAt);
	ar & picojson::convert::member("units", units);
      }
      template<class Archive>
      void json(Archive & ar)
      {
	ar & picojson::convert::member("delta_t", delta_t);
	ar & picojson::convert::member("stopAt", stopAt);
	ar & picojson::convert::member("units", units);
      }
    };

    /**
     * Digraph coupling node structure
     */
    struct DigraphNode {
      std::string model;
      std::string port;

      DigraphNode() :
	model(""),
	port("") {}
      
      DigraphNode(std::string aModel, std::string aPort) :
	model(aModel),
	port(aPort) {}

      DigraphNode(const DigraphNode& aDigraphNode) :
	model(aDigraphNode.model),
	port(aDigraphNode.port) {}

      friend class picojson::convert::access;
      template<class Archive>
      void json(Archive & ar) const
      {
	ar & picojson::convert::member("model", model);
	ar & picojson::convert::member("port", port);
      }
      template<class Archive>
      void json(Archive & ar)
      {
	ar & picojson::convert::member("model", model);
	ar & picojson::convert::member("port", port);
      }
    };

    /**
     * Digraph coupling structure
     */
    struct DigraphCoupling {
      DigraphNode from;
      DigraphNode to;

      DigraphCoupling() :
	from("",""),
	to("","") {}
      
      DigraphCoupling(DigraphNode fromNode, DigraphNode toNode) :
	from(fromNode),
	to(toNode) {}

      DigraphCoupling(std::string modelFrom, std::string portFrom,
		      std::string modelTo, std::string portTo) :
	from(modelFrom, portFrom),
	to(modelTo, portTo) {}

      DigraphCoupling(const DigraphCoupling& aDigraphCoupling) :
	from(aDigraphCoupling.from),
	to(aDigraphCoupling.to) {}

      friend class picojson::convert::access;
      template<class Archive>
      void json(Archive & ar) const
      {
	ar & picojson::convert::member("from", from);
	ar & picojson::convert::member("to", to);
      }
      template<class Archive>
      void json(Archive & ar)
      {
	ar & picojson::convert::member("from", from);
	ar & picojson::convert::member("to", to);
      }

    };
    
  } // namespace impl

} // namespace efscape

namespace picojson {
  namespace convert {

    template <class Archive>
    void json(Archive &ar, efscape::impl::ClockI const &clock) {

      double delta_t = clock.timeDelta();
      ar &picojson::convert::member("delta_t", delta_t);

      double stopAt = clock.timeMax();
      ar &picojson::convert::member("stopAt", stopAt);

      std::string units = clock.timeUnits();
      ar &picojson::convert::member("units", units);
    }

    template <class Archive>
    void json(Archive &ar, efscape::impl::ClockI &clock) {

      ar &picojson::convert::member("delta_t", clock.timeDelta());

      ar &picojson::convert::member("stopAt", clock.timeMax());

      std::string units;
      ar &picojson::convert::member("units", units);
      clock.timeUnits(units.c_str());
    }

  }
}

#endif	// #ifndef EFSCAPE_IMPL_ADEVS_JSON_HPP

