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

#include <string>

namespace efscape {

  namespace impl {

    /**
     * Creates a model from a model configuration stored in a JSON
     * string.
     *
     * @param aCr_JSONstring model configuration embedded in a JSON string
     * @returns handle to model
     * @throws std::logic_error
     */
    adevs::Devs<IO_Type>*
    createModelFromJSON(const std::string& aCr_JSONstring)
      throw(std::logic_error);

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

