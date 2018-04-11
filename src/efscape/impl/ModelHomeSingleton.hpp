// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeSingleton.hpp
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
#ifndef EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP
#define EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP

namespace efscape {

  namespace impl {

    /**
     * External singleton interface (see loki/test).
     */
    template<class T>
    class Singleton
    {
    public:
      static T& Instance();
    };


  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOME_SINGLETON_HPP
