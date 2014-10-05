// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Index.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/Index.hh>

#include <iostream>
#include <sstream>

/**
 * function that overrides the "<<" operator
 *
 * @param aCr_stream output stream
 * @param aCr_index index
 * @returns updated output stream
 */
std::ostream& operator <<( std::ostream& aCr_stream,
			   const efscape::gis::Index& aCr_index) {
  aCr_stream << "("
	     << aCr_index.z() << ","
	     << aCr_index.y() << ","
	     << aCr_index.x() << ")";
  return aCr_stream;
}

/**
 * function that overrides the ">>" operator
 *
 * @param aCr_stream input stream
 * @param aCr_index index
 * @returns updated input stream
 */
std::istream& operator >>( std::istream& aCr_stream,
			   efscape::gis::Index& aCr_index) {

  // first, attempt to read in '(')
  char ch;
  aCr_stream >> ch;
  if (ch != '(') {
    aCr_stream.setstate(std::ios::failbit);
    return aCr_stream;
  }

  // next, read in z coordinate and ','
  aCr_stream >> aCr_index.z();
  aCr_stream >> ch;
  if (ch != ',') {
    aCr_stream.setstate(std::ios::failbit);
    return aCr_stream;
  }

  // next, read in y coordinate and ','
  aCr_stream >> aCr_index.y();
  aCr_stream >> ch;
  if (ch != ',') {
    aCr_stream.setstate(std::ios::failbit);
    return aCr_stream;
  }

  // finally, read in x coordinate and closing ')'
  aCr_stream >> aCr_index.x();
  aCr_stream >> ch;
  if (ch != ')') {
    aCr_stream.setstate(std::ios::failbit);
    return aCr_stream;
  }

  return aCr_stream;
}

namespace efscape {
  namespace gis {

    /** @returns location in a string */
    std::string Index::toString() const {
      std::ostringstream lC_buffer;
      lC_buffer << *this;
      return lC_buffer.str();
    }

  }
}
