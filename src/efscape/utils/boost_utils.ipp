// __COPYRIGHT_START__
// Package Name : efscape
// File Name : boost_utils.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/utils/boost_utils.hpp>

namespace efscape {
  namespace utils {

    //----------------------------------------------------------------
    // helper functions for extracting values from a boost::any object
    //  - implementation
    //----------------------------------------------------------------
    template <class DataType>
    bool is_type(const boost::any & operand)
    {
      try {
	boost::any_cast<DataType>(&operand);
	return true;
      }
      catch(const boost::bad_any_cast &) {
	return false;
      }
    }

    //-------------------------------------------------------------
    // helper functions for loading data types into a property tree
    //  - implementation
    //-------------------------------------------------------------
    template <class DataType>
    boost::property_tree::ptree
    vector_to_ptree(const char* acp_name,
		    const std::vector<DataType>& aC1r_vector)
    {
      boost::property_tree::ptree parent;
      boost::property_tree::ptree children;
      for (int i = 0; i < aC1r_vector.size(); i++) {
	boost::property_tree::ptree child;
	child.put("", aC1r_vector[i]);
	children.push_back( std::make_pair("", child) );
      }
      parent.add_child(acp_name, children);

      return parent;
    }

    template <class DataType>
    boost::property_tree::ptree
    matrix_to_ptree(const char* acp_name,
		    const std::vector< std::vector<DataType> >& aC2r_matrix)
    {
      boost::property_tree::ptree parent;
      boost::property_tree::ptree children;
      for (int i = 0; i < aC2r_matrix.size(); i++) {
	boost::property_tree::ptree row;
	for (int j = 0; j < aC2r_matrix[i].size(); j++) {
	  boost::property_tree::ptree child;
	  child.put("", aC2r_matrix[i][j]);
	  row.push_back( std::make_pair("", child) );
	}
	children.push_back( std::make_pair("", row) );
      }
      parent.add_child(acp_name, children);

      return parent;
    }

    template <class KeyType, class ValueType>
    boost::property_tree::ptree
    map_to_ptree(const char* acp_name,
		 const std::map< KeyType, ValueType >& aCr_map)
    {
      boost::property_tree::ptree parent;
      boost::property_tree::ptree children;
      typename std::map<KeyType,ValueType>::const_iterator iter;
      iter = aCr_map.begin();
      for ( ; iter != aCr_map.end(); iter++) {
	children.put( iter->first, iter->second );
      }
      parent.add_child(acp_name, children);

      return parent;
    }


  } // namespace utils
}   // namespace efscape
