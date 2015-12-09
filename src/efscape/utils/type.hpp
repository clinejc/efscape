// __COPYRIGHT_START__
// Package Name : efscape
// File Name : type.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
// 2015-11-08 from http://www.gilgil.net/communities/9180 (How to demangel a class name with Boost)
// 2014-08-31 from http://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname

#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <boost/units/detail/utility.hpp>
#include <typeinfo>

namespace efscape {
  namespace utils {
    std::string demangle(const char* name);

    template <class T>
    std::string type(const T& t) {
      // return demangle(typeid(t).name());
      return boost::units::detail::demangle(typeid(t).name());
    }

    template <class T>
    std::string type() {
      T t;
      return boost::units::detail::demangle(typeid(t).name());
    }

  }
}

#endif
