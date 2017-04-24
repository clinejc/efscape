// __COPYRIGHT_START__
// Package Name : efscape
// File Name : xerces_strings.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_UTILS_XERCES_STRINGS_HPP_INCLUDED
#define EFSCAPE_UTILS_XERCES_STRINGS_HPP_INCLUDED

#include <string>
#include <boost/scoped_array.hpp>
#include <xercesc/util/XMLString.hpp>

namespace efscape {
  namespace utils {

    typedef std::basic_string<XMLCh> XercesString;

    // Converts from a narrow-character string to a wide-character string.
    inline XercesString fromNative(const char* str)
    {
      boost::scoped_array<XMLCh> ptr(xercesc::XMLString::transcode(str));
      return XercesString(ptr.get( ));
    }

    // Converts from a narrow-character string to a wide-charactr string.
    inline XercesString fromNative(const std::string& str)
    {
      return fromNative(str.c_str( ));
    }

    // Converts from a wide-character string to a narrow-character string.
    inline std::string toNative(const XMLCh* str)
    {
      boost::scoped_array<char> ptr(xercesc::XMLString::transcode(str));
      return std::string(ptr.get( ));
    }

    // Converts from a wide-character string to a narrow-character string.
    inline std::string toNative(const XercesString& str)
    {
      return toNative(str.c_str( ));
    }

    // Converts from a Xerces String to a std::wstring
    inline std::wstring xercesToWstring(const XercesString& str)
    {
      return std::wstring(str.begin(), str.end());
    }

    // Converts from a std::wstring to a XercesString
    inline XercesString wstringToXerces(const std::wstring& str)
    {
      return XercesString(str.begin(), str.end());
    }

  } // namespace utils
}   // namespace efscape

#endif // #ifndef EFSCAPE_UTILS_XERCES_STRINGS_HPP_INCLUDED
