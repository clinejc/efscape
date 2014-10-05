// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GeoNetCDF.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_GIS_GEONETCDF_HH
#define EFSCAPE_GIS_GEONETCDF_HH

#include <netcdfcpp.h>
#include <geos/geom/Envelope.h>

#include <efscape/gis/GeoDataSource.hpp>
#include <efscape/gis/Geogrid_impl.hh>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <stdexcept>
#include <string>
#include <map>
#include <sstream>
#include <vector>

namespace efscape {

  namespace gis {

    // utility method for creating Geogrid based on NcType data type
    Geogrid* CreateGeogrid(NcType eType);

    /**
     * A base interface for accessing georeferenced netCDF data.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.00 created 30 Jul 2006, revised 05 Oct 2014
     *
     * ChangeLog:
     *  - 2014-10-05 revised due to changes to parent class GeoDataSource
     *  - 2009-07-24 revised
     *  - 2006-07-30 Create class GeoNetCDF
     */
    class GeoNetCDF : public GeoDataSource
    {
       friend class boost::serialization::access;

    public:
      GeoNetCDF();
      GeoNetCDF(const char* acp_filename, NcFile::FileMode ae_filemode);
      GeoNetCDF(const char* acp_name,
		int ai_xsize,
		int ai_ysize,
		int ai_zsize,
		const geos::geom::Envelope& aCr_envelope,
		const char* acp_projection,
		bool ab_isa_timeseries);
      ~GeoNetCDF();

      //
      // methods
      //

      void open(const char* acp_filename, bool ab_is_mutable);
      void open(const char* acp_filename, NcFile::FileMode ae_filemode);
      bool close();

      /** @returns whether this datasource is mutable */
      bool is_mutable() const {
	return mb_is_mutable;
      }

      /** @returns name of dataset */
      const char* name() const { return mC_name.c_str(); }

      // methods for adding and accessing variables
      bool add_geogrid(const char* acp_name, NcType ae_type);

      /** @returns whether the netCDF dataset is open */
      bool is_open() const {
	if (mCp_dataset.get() == 0)
	  return false;

	return mCp_dataset->is_valid();
      }

      /** @returns handle to GDALDataset */
      NcFile* dataset() { return mCp_dataset.get(); }
      const NcFile* dataset() const { return mCp_dataset.get(); }

      bool isa_timeseries() const;
      bool is_georeferenced() const;
      bool is_georeferenced(const char* acp_name) const;
      long num_rows() const;
      long num_cols() const;
      long num_bands() const;
      geos::geom::Envelope envelope() const;
      const char* spatial_ref() const;

      Geogrid* create_geogrid(const char* acp_name) const;

      // read
      int read(Geogrid* aCp_grid,long al_index) const
	throw (std::out_of_range);

      template<class Type>
      int read(Geogrid_impl<Type>& aCr_grid, long al_index) const
	throw (std::out_of_range);

      // write
      int write(const Geogrid* aCp_grid,long al_index)
	throw (std::out_of_range);

      template<class Type>
      int write(const Geogrid_impl<Type>& aCr_grid, long al_index)
	throw (std::out_of_range);

      // attributes
      void time_units(double ad_units,
		      const char* acp_unit,
		      boost::posix_time::ptime& aCr_base_date);
      boost::posix_time::time_duration time_unit() const;

      boost::posix_time::ptime base_date() const;
      double delta_t() const;
      boost::posix_time::ptime date(long al_index) const
	throw (std::out_of_range);

      /**
       * Sets the time for the specified record.
       *
       * @param ad_time time
       * @param al_index record index
       */
      void time(double ad_time, long al_index) {
	if (isa_timeseries()) {
	  mCp_dataset->get_var("time")->put_rec(&ad_time, al_index);
	}
      }

      /** @returns number of records */
      long num_recs() const {
	if (isa_timeseries()) {
	  long ll_size = mCp_dataset->rec_dim()->size();
	  if (ll_size > 0)
	    return ll_size;
	}
	return 1;
      }

    protected:

      void time_units(const char* acp_units);

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
	// parent class
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GeoDataSource);
      }

      /** handle to netCDF dataset */
      boost::scoped_ptr<NcFile> mCp_dataset;

      /** dataset name (path) */
      std::string mC_name;

      /** whether the dataset is writeable */
      bool mb_is_mutable;

      /** record index */
      long ml_index;

      /** name of current variable */
      std::string mC_cur_var_name;

      /** base date (from global attribute <time_units>) */
      boost::posix_time::ptime mC_base_date;

      /** time unit (from global attribute <time_units>) */
      boost::posix_time::time_duration mC_time_unit;

      /** time delta (from global attribute <delta_t>) */
      double md_delta_t;

    };				// class GeoNetCDF<> definition

    // typedefs
    typedef boost::shared_ptr<GeoNetCDF> GeoRasterNcPtr; // deprecated
    typedef boost::shared_ptr<GeoNetCDF> GeoNetCDFPtr;

    // utility function for creating a GeoNetCDF dataset from a geogrid
    GeoNetCDF* createGeoNetCDF(const char* acp_filename,
			       const Geogrid& aCr_geogrid);
    NcType getNcType(const Geogrid* aCp_geogrid);

  } // namespace gis

} // namespace efscape

#endif	// #ifndef EFSCAPE_GIS_GEONETCDF_HH
