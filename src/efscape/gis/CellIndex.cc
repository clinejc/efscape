// __COPYRIGHT_START__
// Package Name : efscape
// File Name : CellIndex.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/CellIndex.hh>

#include <efscape/gis/Geogrid.hh>

namespace efscape {

  namespace gis {

    /**
     * Returns the associated grid cell indices via references
     *
     * @param aCp_grid handle to grid
     * @param ai_row reference to row index
     * @param ai_col reference to column index
     */
    void CellIndex::operator()(const Geogrid* aCp_grid,
			       size_t& ai_row, size_t& ai_col) const
    {
    }

    /**
     * Returns the associated grid cell indices via refences.
     *
     * @param aCp_grid handle to grid
     * @param ai_row reference to row index
     * @param ai_col reference to column index
     */
    void Torus::operator()(const Geogrid* aCp_grid,
			   size_t& ai_row, size_t& ai_col) const
    {
      if (aCp_grid == NULL)
	return;

      int li_nrows = aCp_grid->num_rows();
      int li_ncols = aCp_grid->num_cols();

      ai_row = (ai_row+li_nrows)%li_nrows;
      ai_col = (ai_col+li_ncols)%li_ncols;
    }

  } // namespace gis

} // namespace efscape
