// __COPYRIGHT_START__
// Package Name : efscape
// File Name : DataFrameTie.hh
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
#ifndef EFSCAPE_SERVER_DATAFRAMEI_HH
#define EFSCAPE_SERVER_DATAFRAMEI_HH

// parent class definition
#include <efscape/efscape.h>

#include <IceUtil/Handle.h>
#include <efscape/impl/DataFrameI.hh>
#include <boost/shared_ptr.hpp>
#include <map>

namespace efscape {

  namespace server {

    // forward declarations
    class DataFrameTie;

    // typedefs
    typedef IceUtil::Handle<DataFrameTie> DataFrameTiePtr;
    typedef std::map<std::string, DataFrameTiePtr> DataFrameTiePtrMap;

    /**
     * Defines the DataFrameTie class, which provides a simple data frame
     * interface to a netCDF dataset. To be valid, the underlying dataset
     * should have a single dimension (all variables with more than one
     * dimension, the first dimension, will be ignored).
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.04 created 04 May 2007, revised 11 Mar 2009
     *
     * ChangeLog:
     *   - 2009.03.11 class name changed to DataFrameTie (created)
     *   - 2007.10.14 Class name changed to 'DataFrameI'
     *   - 2007.07.23 public method made virtual
     *     - void open(const char*, unsigned long)
     *   - 2007.05.04 Created class DataFrame.
     */
    class DataFrameTie : public ::efscape::data::DataFrame
    {
    public:

      DataFrameTie();
      DataFrameTie(const efscape::impl::DataFrameIPtr& aCp_DataFrame);
      virtual ~DataFrameTie();

      //
      // ICE interface efscape::DataFrame
      //
      virtual
      std::string getName(const Ice::Current&);

      virtual
      ::efscape::data::StringSequence variableNames(const Ice::Current&);

      virtual
      ::Ice::Long numRows(const Ice::Current& current);

      virtual
      ::efscape::data::DataFramePtr getRowSubset(::Ice::Long,
						 ::Ice::Long,
						 const Ice::Current&);

      virtual
      ::efscape::data::EfValueMatrix getRowDataSubset(::Ice::Long,
						      ::Ice::Long,
						      const Ice::Current&);
      //-------------------------
      // accessor/mutator methods
      //-------------------------
      void setDataFrame(const efscape::impl::DataFrameIPtr& aCp_DataFrame);
      const char* name() const;

    protected:

      /** smart handle to DataFrameI object*/
      efscape::impl::DataFrameIPtr mCp_DataFrame;

    };				// class DataFrameTie definition

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_DATAFRAMEI_HH
