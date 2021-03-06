// __COPYRIGHT_START__
// Package Name : efscape
// File Name : boost_utils.cpp
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

#include <efscape/utils/boost_utils.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <string>
#include <sstream>

namespace efscape {
  namespace utils {

    //----------------------------------------------------------------
    // helper functions for extracting values from a boost::any object
    //  - implementation
    //----------------------------------------------------------------
    bool is_empty(const boost::any & operand)
    {
      return operand.empty();
    }

    bool is_int(const boost::any & operand)
    {
      return operand.type() == typeid(int);
    }

    bool is_double(const boost::any & operand)
    {
      return operand.type() == typeid(double);
    }

    bool is_char_ptr(const boost::any & operand)
    {
      try {
	boost::any_cast<const char *>(operand);
	  return true;
      }
      catch(const boost::bad_any_cast &) {
	return false;
      }
    }

    bool is_string(const boost::any & operand)
    {
      try {
	boost::any_cast<std::string>(&operand);
	return true;
      }
      catch(const boost::bad_any_cast &) {
	return false;
      }
    }

    /**
     * Converts a boost::property_tree:ptree to JSON format.
     * <br><br>
     * Note: removes double quotes from all integers and doubles
     *
     * @param aCr_pt reference to ptree
     * @return JSON string
     */
    std::string ptree_to_json(const boost::property_tree::ptree& aCr_pt)
    {
      // regex to extract a number in double quote
      boost::regex rgx("\"(-{0,1}[0-9]+\\.{0,1}[0-9]*)\"");

      std::ostringstream lC_buffer;
      boost::property_tree::write_json(lC_buffer,
				       aCr_pt,
				       false);
      std::string fmt_str("$1");
      return  boost::regex_replace(lC_buffer.str(), rgx, fmt_str);
    }

  } // namespace utils
}   // namespace efscape
