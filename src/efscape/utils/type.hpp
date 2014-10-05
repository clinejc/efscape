// __COPYRIGHT_START__
// Package Name : efscape
// File Name : type.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
// 2014-08-31 from http://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname

#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <typeinfo>

namespace efscape {
  namespace utils {
    std::string demangle(const char* name);

    template <class T>
    std::string type(const T& t) {

      return demangle(typeid(t).name());
    }

  }
}

#endif
