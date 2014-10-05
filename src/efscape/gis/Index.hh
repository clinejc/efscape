// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Index.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_INDEX_HH
#define EFSCAPE_GIS_INDEX_HH

#include <iosfwd>

namespace efscape {

  namespace gis {

    /**
     * A simple grid index class
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.3 created 31 Jul 2006, revised 01 Jun 2010
     */
    class Index {

      /** band index */
      long ml_band;

      /** row index */
      long ml_row;

      /** column index */
      long ml_col;

    public:

      /** default constructor */
      Index() : ml_band(0), ml_row(0), ml_col(0) {}

      /** copy constructor */
      Index(const Index& aCr_orig) {
	ml_band = aCr_orig.ml_band;
	ml_row = aCr_orig.ml_row;
	ml_col = aCr_orig.ml_col;
      }

      /**
       * constructor
       *
       * @param ai_row row
       * @param ai_col column
       */
      Index(long ai_row, long ai_col) :
	ml_band(0),
	ml_row(ai_row),
	ml_col(ai_col) {}

      /**
       * constructor
       *
       * @param ai_band band
       * @param ai_row row
       * @param ai_col column
       */
      Index(long ai_band, long ai_row, long ai_col) :
	ml_band(ai_band),
	ml_row(ai_row),
	ml_col(ai_col) {}

      /**
       * Resets the row/column indices
       *
       * @param ai_row row
       * @param ai_col column
       * @returns reference to this index
       */
      Index& reset(long ai_row, long ai_col) {
	ml_row = ai_row;
	ml_col = ai_col;
	return (*this);
      }
      

      // data access
      long band() const { return ml_band; }
      long& band() { return ml_band; }
      long z() const { return ml_band; }
      long& z() { return ml_band; }
      long row() const { return ml_row; }
      long& row() { return ml_row; }
      long y() const { return ml_row; }
      long& y() { return ml_row; }
      long col() const { return ml_col; }
      long& col() { return ml_col; }
      long x() const { return ml_col; }
      long& x() { return ml_col; }

      bool operator==(const Index& aCr_index2) const {
	return ( z() == aCr_index2.z() &&
		 y() == aCr_index2.y() &&
		 x() == aCr_index2.x() );
      }

      bool operator!=(const Index& aCr_index2) const {
	return ( ( z() == aCr_index2.z() &&
		   y() != aCr_index2.y() ) ||
		 x() != aCr_index2.x() );
      }

      Index operator+(const Index& aCr_index2) const {
	return ( Index( z() + aCr_index2.z(),
			y() + aCr_index2.y(),
			x() + aCr_index2.x() ) );
      }

      Index operator+=(const Index& aCr_index2) {
	ml_band += aCr_index2.ml_band;
	ml_row += aCr_index2.ml_row;
	ml_col += aCr_index2.ml_col;
	return ( *this );
      }

      Index operator-(const Index& aCr_index2) const {
	return ( Index( z() - aCr_index2.z(),
			y() - aCr_index2.y(),
			x() - aCr_index2.x() ) );
      }

      Index operator-=(const Index& aCr_index2) {
	ml_band -= aCr_index2.ml_band;
	ml_row -= aCr_index2.ml_row;
	ml_col -= aCr_index2.ml_col;
	return ( *this );
      }

      std::string toString() const;

    };				// class Index

  } // namespace gis

} // namespace efscape

// IO functions for class efscape::gis::Index - definition
std::ostream& operator <<( std::ostream& aCr_stream,
                           const efscape::gis::Index& aCr_index);
std::istream& operator >>( std::istream& aCr_stream,
                           efscape::gis::Index& aCr_index);

#endif	// #ifndef EFSCAPE_GIS_INDEX_HH
