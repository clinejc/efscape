// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GeoDataSource.hpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEODATASOURCE_HPP
#define EFSCAPE_GIS_GEODATASOURCE_HPP

#include <efscape/gis/Geogrid_impl.hh>
#include <boost/serialization/nvp.hpp>
#include <string>

namespace efscape {

  namespace gis {

    // forward declaration
    class GeoDataSource;

    /**
     * A base interface for accessing georeferenced grid dataset.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.3 created 04 Aug 2006, revised 05 Oct 2014
     *
     * ChangeLog:
     *  - 2014-10-05 name changed back to GeoDataset
     *  - 2006-11-24 name changed from GeoDataset to GeoRaster
     *  - 2006-08-04 created class GeoDataSet
     */
    class GeoDataSource
    {
      friend class boost::serialization::access;

    public:

      //
      // constructor/destructor
      //
      GeoDataSource();

      virtual ~GeoDataSource();

      //
      // methods
      //
      virtual bool is_open() const { return false; }

      virtual Geogrid* create_geogrid(const char* acp_name) const = 0;

      /** @returns number of records */
      virtual long num_recs() const {
	return 1;
      }

      // read
      virtual int read(Geogrid* aCp_grid, long al_index) const
	throw (std::out_of_range)
      {
	return 1;
      }

      // write
      virtual int write(const Geogrid* aCp_grid, long al_index)
	throw (std::out_of_range)
      {
	return 1;
      }

      //----------
      // iterators
      //----------
      /**
       * GeoDataSource iterator.
       */
      class Iterator :
	public std::iterator<std::random_access_iterator_tag, Geogrid*>
      {
      public:
	/**
	 * Constructs an uninitialized iterator.
	 *
	 * Assign a value before using it.
	 */
	Iterator() :
	  mCp_dataset(0),
	  mC_name(""),
	  ml_index(0),
	  mCp_grid(0)
	{}

	/**
	 * Constructs an iterator for a Geodataset.
	 *
	 * @param aCp_geodataset handle to geodataset
	 * @param acp_name name of geogrid
	 * @param al_index data index
	 */
	Iterator(GeoDataSource* aCp_dataset, const char* acp_name, long al_index) :
	  mCp_dataset(aCp_dataset),
	  mC_name(acp_name),
	  ml_index(al_index),
	  mCp_grid(0)
	{
	  if (ml_index >= 0 && ml_index < mCp_dataset->num_recs()) {
	    mCp_grid = mCp_dataset->create_geogrid(acp_name);
	    if (mCp_grid != 0)
	      mCp_dataset->read(mCp_grid,ml_index);
	  }
	}

	/**
	 * Constructs an iterator copy.
	 *
	 * @param aCr_source iterator to copy
	 */
	Iterator(const Iterator& aCr_source) {
	  this->mCp_dataset = aCr_source.mCp_dataset;
	  this->mC_name = aCr_source.mC_name;
	  this->ml_index = aCr_source.ml_index;

	  if (aCr_source.mCp_grid != 0)
	    this->mCp_grid = aCr_source.mCp_grid->clone();
	  else
	    this->mCp_grid = 0;
	}

	/** destructor */
	~Iterator() {
	  if (mCp_grid != 0)
	    delete mCp_grid;
	}

	/**
	 * Copies the source iterator to this iterator.
	 *
	 * @param aCr_source iterator
	 * @param reference to this iterator
	 */
	Iterator& operator=(const Iterator& aCr_source) {
	  this->mCp_dataset = aCr_source.mCp_dataset;
	  this->ml_index = aCr_source.ml_index;
	  if (mCp_grid != 0)
	    delete mCp_grid;

	  this->mCp_grid = aCr_source.mCp_grid->clone();

	  return (*this);
	}

	/** @returns grid name */
	const char* name() const { return mC_name.c_str(); }

	/** @returns data index */
	long index() const { return ml_index; }

	/** @returns whether the iterator is valid */
	bool is_valid() {
	  return (mCp_grid != 0);
	}

	/**
	 * Writes geogrid data to the geodataset.
	 *
	 * @returns exit status of write operation
	 */
	int write() {
	  return mCp_dataset->write(mCp_grid,ml_index);
	}

	/**
	 * Returns the current item.
	 *
	 * @returns reference to current item
	 */
	Geogrid* operator*() {
	  return( mCp_grid );
	}

	/**
	 * Returns a pointer to the current item
	 *
	 * @returns pointer to the current item
	 */
	Geogrid** operator->() {
	  return( &mCp_grid );
	}

	/**
	 * Increment iterator to refer to the next element and return this
	 * iterator.
	 *
	 * @returns  iterator pointing at new current item
	 */
	Iterator& operator++() {
	  ++ml_index;

	  mCp_dataset->read(mCp_grid,ml_index);

	  return(*this);
	}

	/**
	 * Increment iterator to refer to the next element and return
	 * an iterator to the previous current item.
	 *
	 * @returns iterator pointing at previous current item
	 */
	Iterator operator++(int) {
	  Iterator tmp(*this);
	  ++(*this);

	  return(tmp);
	}

	/**
	 * Decrement iterator to refer to the previous element and return this
	 * iterator.
	 *
	 * @returns iterator pointing at new current item
	 */
	Iterator& operator--() {
	  --ml_index;

	  mCp_dataset->read(mCp_grid,ml_index);

	  return(*this);
	}

	/**
	 * Decrement iterator to refer to the previous element and return an
	 * iterator to the previous current item.
	 *
	 * @returns iterator pointing at previous current item
	 */
	Iterator operator--(int) {
	  Iterator tmp(*this);
	  --(*this);

	  return(tmp);
	}

	/**
	 * Returns true if the iterators point to the same item.
	 *
	 * @param aCr_other other iterator
	 * @returns true if (*this) == (*other)
	 */
	bool operator==(const Iterator& aCr_other) {
	  return ( mCp_dataset == aCr_other.mCp_dataset &&
		   mC_name == aCr_other.mC_name &&
		   ml_index == aCr_other.ml_index );
	}

	/**
	 * Returns true if the iterators are point to different items.
	 *
	 * @param aCr_other other iterator
	 * @returns true if (*this) != (*other)
	 */
	bool operator!=(const Iterator& aCr_other) {
	  return ( mCp_dataset != aCr_other.mCp_dataset ||
		   mC_name == aCr_other.mC_name ||
		   ml_index != aCr_other.ml_index );
	}

	/**
	 * Returns true if the item pointed to by this iterator is before the
	 * item pointed to by the other iterator.
	 *
	 * @param aCr_other other iterator
	 * @returns true if position of (*this) < position of (*other)
	 */
	bool operator<(const Iterator& aCr_other) {
	  return ( mCp_dataset == aCr_other.mCp_dataset &&
		   mC_name == aCr_other.mC_name &&
		   std::string(mCp_grid->name()) == aCr_other.mCp_grid->name() &&
		   ml_index < aCr_other.ml_index );
	}

	/**
	 * Returns true if the item pointed to by this iterator either is
	 * before the item pointed to by the other iterator or are the same.
	 *
	 * @param aCr_other other iterator
	 * @returns true if position of (*this) is <= position of (*other)
	 */
	bool operator<=(const Iterator& aCr_other) {
	  return ( mCp_dataset == aCr_other.mCp_dataset &&
		   mC_name == aCr_other.mC_name &&
		   ml_index <= aCr_other.ml_index );
	}

	/**
	 * Returns true if the item pointed to by this iterator is after the
	 * item pointed to by the other iterator.
	 *
	 * @param aCr_other other iterator
	 * @returns true if position of (*this) is > position of (*other)
	 */
	bool operator>(const Iterator& aCr_other) {
	  return ( mCp_dataset == aCr_other.mCp_dataset &&
		   mC_name == aCr_other.mC_name &&
		   ml_index > aCr_other.ml_index );
	}

	/**
	 * Returns true if the item pointed to by this iterator either is
	 * after the item pointed to by the other iterator or are the same.
	 *
	 * @param aCr_other other iterator
	 * @returns true if position of (*this) is before position of (*other)
	 */
	bool operator>=(const Iterator& aCr_other) {
	  return ( mCp_dataset == aCr_other.mCp_dataset &&
		   mC_name == aCr_other.mC_name &&
		   ml_index >= aCr_other.ml_index );
	}

	/**
	 * Advance by n locations and return a reference to this iterator.
	 *
	 * @param n number of locations to advance
	 * @return iterator
	 */
	Iterator& operator+=(size_t n) {
	  ml_index+=(long)n;

	  if ( ml_index >= mCp_dataset->num_recs() )
	    ml_index = mCp_dataset->num_recs();
	  else
	    mCp_dataset->read(mCp_grid,ml_index);

	  return (*this);
	}

	/**
	 * Retreat by n locations and return a reference to this iterator.
	 *
	 * @param n number of locations to retreat
	 * @return iterator
	 */
	Iterator& operator-=(size_t n) {
	  ml_index-=(long)n;

	  if ( ml_index < 0)
	    ml_index = -1;
	  else
	    mCp_dataset->read(mCp_grid,ml_index);

	  return (*this);
	}

	/**
	 * Return an iterator that is positioned n elements ahead of this
	 * iterator.
	 *
	 * @param n number of elements
	 * @return iterator positioned n elements ahead of this iterator
	 */
	Iterator operator+(size_t n) {
	  long ll_index = ml_index + (long)n;
	  if ( ml_index >= mCp_dataset->num_recs() )
	    ml_index = mCp_dataset->num_recs();
	
	  return ( Iterator(mCp_dataset, mC_name.c_str(), ll_index) );
	}

	/**
	 * Return an iterator that is positioned n elements behind this
	 * iterator.
	 *
	 * @param n number of elements
	 * @return iterator positioned n elements behind this iterator
	 */
	Iterator operator-(size_t n) {
	  long ll_index = ml_index - (long)n;
	  if ( ll_index < 0)
	    ll_index = -1;

	  return ( Iterator(mCp_dataset, mC_name.c_str(), ll_index) );
	}

	/**
	 * Return an iterator that is positioned n elements ahead of this
	 * iterator.
	 *
	 * @param n number of elements
	 * @return iterator positioned n elements ahead of this iterator
	 */
	Iterator operator[](size_t n) {
	  long ll_index = ml_index + (long)n;
	  if ( ml_index >= mCp_dataset->num_recs() )
	    ml_index = mCp_dataset->num_recs();

	  return ( Iterator(mCp_dataset, mC_name.c_str(), ll_index) );
	}

	/**
	 * Return the distance between two iterators.
	 *
	 * @param other iterator
	 * @returns distance between two iterators
	 */
	size_t operator-(const Iterator& aCr_other) {
	  return (ml_index - aCr_other.ml_index);
	}

      private:

	/** handle to associated GeoDataSource */
	GeoDataSource* mCp_dataset;

	/** geogrid name */
	std::string mC_name;

	/** data index */
	long ml_index;

	/** handle to geogrid */
	Geogrid* mCp_grid;

      };			// class GeoDataSource::Iterator

      //-----------------------
      // typedefs for iterators
      //-----------------------
      typedef Iterator iterator;
//       typedef ConstIterator const_iterator;

      //--------------------
      // container interface
      //--------------------
      /**
       * Returns an iterator to the first data record.
       *
       * @param acp_name grid name
       * @returns an iterator pointing to the first data item
       */
      iterator begin(const char* acp_name) {
	return ( iterator(this, acp_name, 0) );
      }

      /**
       * Returns an iterator pointing immediately after the last data record.
       *
       * @param acp_name grid name
       * @returns an iterator pointing immediately after the last item
       */
      iterator end(const char* acp_name) {
	return ( iterator(this, acp_name, num_recs() ) );
      }

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
	// parent class
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GeoDataSource);
      }

    };				// class GeoDataSource

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GEODATASOURCE_HPP

