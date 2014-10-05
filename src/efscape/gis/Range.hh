// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Range.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_RANGE_HH
#define EFSCAPE_GIS_RANGE_HH

#include <efscape/gis/Index.hh>
#include <iosfwd>

namespace efscape {

  namespace gis {

    /**
     * Defines a grid range
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.02 created 31 July 2006, revised 09 Jun 2009
     */
    class Range {

      /** minimum index */
      Index mC_min;

      /** maximum index */
      Index mC_max;

    public:

      /** default constructor */
      Range() {}

      /**
       * constructor
       *
       * @param aCr_index1 first index
       * @param aCr_index2 second index
       */
      Range(const Index& aCr_index1, const Index& aCr_index2) {
	init(aCr_index1, aCr_index2);
      }

      /**
       * Initializes the range.
       *
       * @param aCr_index1 first index
       * @param aCr_index2 second index
       */
      void init(const Index& aCr_index1, const Index& aCr_index2) {
	if (aCr_index1.row() <= aCr_index2.row()) {
	  mC_min.row() = aCr_index1.row();
	  mC_max.row() = aCr_index2.row();
	}
	else {
	  mC_min.row() = aCr_index2.row();
	  mC_max.row() = aCr_index1.row();
	}

	if (aCr_index1.col() <= aCr_index2.col()) {
	  mC_min.col() = aCr_index1.col();
	  mC_max.col() = aCr_index2.col();
	}
	else {
	  mC_min.col() = aCr_index2.col();
	}
      }

      Index& min() { return mC_min; }
      Index min() const { return mC_min; }
      Index& max() { return mC_max; }
      Index max() const { return mC_max; }

      bool operator==(const Range& aCr_range2) {
	return ( mC_min == aCr_range2.mC_min &&
		 mC_max == aCr_range2.mC_max );
      }
      bool operator!=(const Range& aCr_range2) {
	return ( mC_min != aCr_range2.mC_min ||
		 mC_max != aCr_range2.mC_max );
      }

      size_t num_rows() const {
	return ( mC_max.row() - mC_min.row() + 1 );
      }

      size_t num_cols() const {
	return ( mC_max.col() - mC_min.col() + 1 );
      }

      size_t size() const {
	return ((mC_max.row()-mC_min.row()+1)*(mC_max.col()-mC_min.col()+1));
      }

    };			// end of class Range

  } // end of namespace gis

} // end of namespace efscape

std::ostream& operator <<( std::ostream& aCr_stream,
			   const efscape::gis::Range& aCr_range);

#endif	// #ifndef EFSCAPE_GIS_RANGE_HH
