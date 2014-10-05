// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CellIndex.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_CELLINDEX_HH
#define EFSCAPE_GIS_CELLINDEX_HH

#include <efscape/gis/Index.hh>

namespace efscape {

  namespace gis {

    // forward declarations
    class Geogrid;

    /**
     * Provides a base interface for locating cells in a grid. Based on
     * a similar class in the SimApp-derived EcoApp library package.
     *
     * ChangeLog
     *   - 2009.03.19 Changed to function object.
     *   - 2008.09.07
     */
    class CellIndex
    {
    public:

      virtual void operator()(const Geogrid* aCp_grid,
			      size_t& ai_row,
			      size_t& ai_col) const;

    };				// class CellIndex

    /**
     * Provides a interface for locating cells in a grid torus. Based on
     * a similar class in the SimApp-derived EcoApp library package.
     *
     * ChangeLog
     *   - 2009.03.19 Changed to function object.
     *   - 2008.09.07
     */
    class Torus : public CellIndex
    {
    public:

      void operator()(const Geogrid* aCp_grid,
		      size_t& ai_row,
		      size_t& ai_col) const;

    };				// class Torus

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_CELLINDEX_HH
