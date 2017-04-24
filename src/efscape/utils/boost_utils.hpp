// __COPYRIGHT_START__
// Package Name : efscape
// File Name : boost_utils.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#ifndef EFSCAPE_UTILS_BOOST_UTILS_HPP
#define EFSCAPE_UTILS_BOOST_UTILS_HPP

#include <boost/any.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/function.hpp>
#include <vector>
#include <map>
#include <string>

namespace efscape {
  namespace utils {

    //----------------------------------------------------------------
    // helper functions for extracting values from a boost::any object
    //----------------------------------------------------------------
    bool is_empty(const boost::any & operand);
    bool is_int(const boost::any & operand);
    bool is_double(const boost::any & operand);
    bool is_char_ptr(const boost::any & operand);
    bool is_string(const boost::any & operand);

    template <class DataType>
    bool is_type(const boost::any & operand);


    //-------------------------------------------------------------
    // helper functions for loading data types into a property tree
    //-------------------------------------------------------------
    template <class DataType>
    boost::property_tree::ptree
    vector_to_ptree(const char* acp_name,
		    const std::vector<DataType>& aC1r_vector);

    template <class DataType>
    boost::property_tree::ptree
    matrix_to_ptree(const char* acp_name,
		    const std::vector< std::vector<DataType> >& aC2r_matrix);

    template <class KeyType, class ValueType>
    boost::property_tree::ptree
    map_to_ptree(const char* acp_name,
		 const std::map< KeyType, ValueType >& aCr_map);
   
    // helper function for converting a ptree to JSON
    std::string ptree_to_json(const boost::property_tree::ptree& aCr_pt);

  }
}

#endif	// #ifndef EFSCAPE_UTILS_BOOST_UTILS_HPP
