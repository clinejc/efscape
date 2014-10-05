// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CoverageI.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef OGIS_IMPL_COVERAGE_I_IPP
#define OGIS_IMPL_COVERAGE_I_IPP

#include <ogis/impl/CoverageI.hpp>

namespace ogis {

  namespace impl {

    /**
     * Return the value vector for a given point in the coverage.
     *
     * @param tparam value type
     * @param aCr_point Point at which to find the grid values
     * @returns value vector for a given point in the coverage
     */
    template <typename Type>
    std::vector<Type> CoverageI::evaluate(const ::pt::PtCoordinatePoint&
					  aCr_point)
    {
      return std::vector<Type>();
    }
    

  } // namespace impl

} // namespace ogis

#endif	// #ifndef OGIS_IMPL_COVERAGE_I_IPP
