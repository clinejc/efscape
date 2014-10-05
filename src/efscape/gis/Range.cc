// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Range.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/Range.hh>

#include <iostream>

/**
 * function that overrides the "<<" operator
 *
 * @param aCr_stream output stream
 * @param aCr_range range
 * @returns updated output stream
 */
std::ostream& operator <<( std::ostream& aCr_stream,
			   const efscape::gis::Range& aCr_range) {
  aCr_stream << "( min: " << aCr_range.min()
	     << ", max: " << aCr_range.max() << " )";
  return aCr_stream;
}
