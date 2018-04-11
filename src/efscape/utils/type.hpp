// __COPYRIGHT_START__
// Package Name : efscape
// File Name : type.hpp
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
