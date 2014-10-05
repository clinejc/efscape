// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Geogrid_impl.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/Geogrid_impl.ipp>

namespace efscape {
  namespace gis {

    template class Geogrid_impl<GByte>;
    template class Geogrid_impl<char>;
    template class Geogrid_impl<short>;
    template class Geogrid_impl<int>;
    template class Geogrid_impl<long>;
    template class Geogrid_impl<float>;
    template class Geogrid_impl<double>;

  }
}
