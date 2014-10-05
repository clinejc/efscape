// __COPYRIGHT_START__
// Package Name : efscape
// File Name : type.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/utils/type.hpp>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace efscape {
  namespace utils {


    struct handle {
      char* p;
      handle(char* ptr) : p(ptr) { }
      ~handle() { std::free(p); }
    };

    std::string demangle(const char* name) {

      int status = -4; // some arbitrary value to eliminate the compiler warning

      handle result( abi::__cxa_demangle(name, NULL, NULL, &status) );

      return (status==0) ? result.p : name ;
    }
  }
}

#else
namespace efscape {
  namespace utils {

    // does nothing if not g++
    std::string demangle(const char* name) {
      return name;
    }
  }
}

#endif
