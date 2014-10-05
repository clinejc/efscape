// __COPYRIGHT_START__
// Package Name : efscape
// File Name : DataFrameTie.cc
// Copyright (C) 2006-2009 by Jon C. Cline
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Bugs, comments, and questions can be sent to clinej@stanfordalumni.org
// __COPYRIGHT_END__
/*
 * ChangeLog:
 *   - 2007.05.04 Created class DataFrame implementation.
 */

#include <efscape/server/DataFrameTie.hh>

#include <sstream>
#include <fstream>

#include <boost/filesystem/convenience.hpp>

namespace efscape {

  namespace server {

    /**
     * Returns the data table name.
     *
     * @param current method invocation
     * @returns name
     */
    ::std::string
    DataFrameTie::getName(const Ice::Current& current)
    {
      return name();
    }

    /**
     * Returns the vector of variable names.
     *
     * @param current method invocation
     * @returns vector of variable names
     */
    ::efscape::data::StringSequence
    DataFrameTie::variableNames(const Ice::Current& current)
    {
      ::efscape::data::StringSequence lC1_names;
      mCp_DataFrame->getColumnNames(lC1_names);
      return lC1_names;
    }


    /**
     * Returns the number of table rows.
     *
     * @param current method invocation
     * @returns number of rows
     */
    ::Ice::Long
    DataFrameTie::numRows(const Ice::Current& current)
    {
      return mCp_DataFrame->num_recs();
    }

    ::efscape::data::DataFramePtr
    DataFrameTie::getRowSubset(::Ice::Long startRow,
			       ::Ice::Long endRow,
			       const Ice::Current& current)
    {
      return 0;
    }

    ::efscape::data::EfValueMatrix
    DataFrameTie::getRowDataSubset(::Ice::Long startRow,
				   ::Ice::Long endRow,
				   const Ice::Current& current)
    {
      return ::efscape::data::EfValueMatrix();
    }

    //
    // server-side methods
    //

    /** default constructor */
    DataFrameTie::DataFrameTie() {}

    /**
     * constructor
     *
     * @param aCp_DataFrame smart handle to wrapped data frame
     */
    DataFrameTie::DataFrameTie(const efscape::impl::DataFrameIPtr&
			       aCp_DataFrame) :
      mCp_DataFrame(aCp_DataFrame)
    {
    }

    /** destructor */
    DataFrameTie::~DataFrameTie() {}


    /**
     * Sets wrapped data frame handle
     *
     * @param aCp_DataFrame smart handle to wrapped data frame
     */
    void DataFrameTie::setDataFrame(const efscape::impl::DataFrameIPtr&
				    aCp_DataFrame) {
      mCp_DataFrame = aCp_DataFrame;
    }

    /** @returns name of data frame */
    const char* DataFrameTie::name() const {
      if (mCp_DataFrame.get())
	return mCp_DataFrame->name();

      return "";
    }

  } // namespace server

} // namespace efscape
