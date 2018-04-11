// __COPYRIGHT_START__
// Package Name : efscape
// File Name : boost_utils.hpp
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
