// __COPYRIGHT_START__
// Package Name : efscape
// File Name : GeoNetCDF.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/GeoNetCDF.ipp>

#include <ogr_spatialref.h>
#include <sstream>

#define TM                     "transverse_mercator"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace efscape {

  namespace gis {

    /** constructor */
    GeoNetCDF::GeoNetCDF() :
      mC_name(""),
      mb_is_mutable(false),
      ml_index(0),
      md_delta_t(0)
    {}

    /**
     * constructor
     *
     * Opens an existing dataset.
     *
     * @param acp_filename file name
     * @param ae_filemode file mode
     */
    GeoNetCDF::GeoNetCDF(const char* acp_filename,
			 NcFile::FileMode ae_filemode) :
      mC_name(acp_filename)
    {
      open(acp_filename,ae_filemode);
    } // GeoNetCDF::GeoNetCDF(const char*)

    /**
     * constructor
     *
     * @param acp_name grid name
     * @param ai_xsize x dimension size
     * @param ai_ysize y dimension size
     * @param ai_zsize z dimension size
     * @param aCr_envelope spatial extent
     * @param acp_projection spatial projection as WKT
     * @param ab_isa_timeseries whether this is a time series
     */
    GeoNetCDF::GeoNetCDF(const char* acp_name,
			 int ai_xsize,
			 int ai_ysize,
			 int ai_zsize,
			 const geos::geom::Envelope& aCr_envelope,
			 const char* acp_projection,
			 bool ab_isa_timeseries) :
      mCp_dataset(0),
      mC_name(acp_name),
      mb_is_mutable(true),
      ml_index(0),
      md_delta_t(0.)
    {
      mCp_dataset.reset(new NcFile(acp_name, NcFile::Replace));

      // add dimensions and coordinates
      NcDim* lCp_dim = NULL;
      NcVar* lCp_var = NULL;

      // time coordinate (optional)
      if (ab_isa_timeseries) {
	lCp_dim = mCp_dataset->add_dim("time");
	lCp_var = mCp_dataset->add_var("time", ncDouble, lCp_dim);
	lCp_var->add_att("long_name", "time");
      }

      // z coordinate (optional)
      if (ai_zsize > 1) {
	lCp_dim = mCp_dataset->add_dim("z",ai_zsize);
	lCp_var = mCp_dataset->add_var("z", ncDouble, lCp_dim);

	// default:"z" coordinate represents bands
	std::vector<double> ld1_z_values(ai_zsize);
	for (int iBand = 0; iBand < ld1_z_values.size(); iBand++)
	  ld1_z_values[iBand] = iBand;
	lCp_var->put(&ld1_z_values[0], ld1_z_values.size());
      }

      // y coordinate
      lCp_dim = mCp_dataset->add_dim("y", ai_ysize);
      lCp_var = mCp_dataset->add_var("y", ncDouble, lCp_dim);
      lCp_var->add_att("units", "m");
      lCp_var->add_att("long_name","y coordinate of projection");
      lCp_var->add_att("standard_name", "projection_y_coordinate");

      // x coordinate
      lCp_dim = mCp_dataset->add_dim("x",ai_xsize);
      lCp_var = mCp_dataset->add_var("x", ncDouble, lCp_dim);
      lCp_var->add_att("units", "m");
      lCp_var->add_att("long_name","x coordinate of projection");
      lCp_var->add_att("standard_name", "projection_x_coordinate");

      // netCDF CF-1 conventions should be observed
      mCp_dataset->add_att("Conventions","CF-1.0");
      mCp_dataset->add_att("AREA_OR_POINT", "Area");

      // add geospatial attributes
      lCp_var = mCp_dataset->add_var(TM, ncChar);

      lCp_var->add_att("Northernmost_Northing",aCr_envelope.getMaxY());
      lCp_var->add_att("Southernmost_Northing",aCr_envelope.getMinY());
      lCp_var->add_att("Easternmost_Easting",aCr_envelope.getMaxX());
      lCp_var->add_att("Westernmost_Easting",aCr_envelope.getMinX());

      lCp_var->add_att("spatial_ref", acp_projection);
      OGRSpatialReference lC_oSRS(acp_projection);

      double ld_width = aCr_envelope.getMaxX()-aCr_envelope.getMinX();
      double ld_height = aCr_envelope.getMaxY()-aCr_envelope.getMinY();
      geos::geom::Coordinate lC_res( ld_width/((double)num_cols()),
			      ld_height/((double)num_rows()) );      

      double ld1_GeoTransform[6] = {
	aCr_envelope.getMinX(),
	lC_res.x,
	0,
	aCr_envelope.getMaxY(),
	0,
	-lC_res.y
      };

      std::ostringstream lC_GeoTransform;
      for (int i = 0; i < 6; i++)
	lC_GeoTransform << ld1_GeoTransform[i] << " ";

      lCp_var->add_att("GeoTransform", lC_GeoTransform.str().c_str());

      lCp_var->add_att("grid_mapping_name", TM);

      lCp_var->add_att("latitude_of_projection_origin",
		       (float)lC_oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN));
      lCp_var->add_att("central_meridian",
		       (float)lC_oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN));
      lCp_var->add_att("scale_factor_at_projection_origin",
		       (float)lC_oSRS.GetProjParm(SRS_PP_SCALE_FACTOR));
      lCp_var->add_att("false_easting",
		       (float)lC_oSRS.GetProjParm(SRS_PP_FALSE_EASTING));
      lCp_var->add_att("false_northing",
		       (float)lC_oSRS.GetProjParm(SRS_PP_FALSE_NORTHING));

      // initialize coordinate vectors to comply with CF-1 convensions
      lCp_var = mCp_dataset->get_var("y");
      std::vector<double> ld1_coordinates(ai_ysize);
      for (int iRow = 0; iRow < ld1_coordinates.size(); iRow++)
	ld1_coordinates[iRow] = aCr_envelope.getMaxY()- (iRow+0.5)*lC_res.y;
      lCp_var->put(&ld1_coordinates[0], ld1_coordinates.size());

      lCp_var = mCp_dataset->get_var("x");
      ld1_coordinates.resize(ai_xsize);
      for (int iCol = 0; iCol < ld1_coordinates.size(); iCol++)
	ld1_coordinates[iCol] = aCr_envelope.getMinX()+ (iCol+0.5)*lC_res.x;
      lCp_var->put(&ld1_coordinates[0], ld1_coordinates.size());

    } // GeoNetCDF::GeoNetCDF(const char*,int,int,int,...)

    /** destructor */
    GeoNetCDF::~GeoNetCDF() {}

    /**
     * Open netCDF file <acp_filename>
     *
     * @param acp_filename file name
     * @param ab_is_mutable whether the dataset is writeable
     */
    void GeoNetCDF::open(const char* acp_filename,
			 bool ab_is_mutable)
    {
      mb_is_mutable = ab_is_mutable; // redundant -- just making sure

      NcFile::FileMode le_filemode = NcFile::ReadOnly;
      if (ab_is_mutable)
	le_filemode = NcFile::Write;

      open(acp_filename,le_filemode); // open the dataset

      if (isa_timeseries()) {
	NcVar* lCp_time_var = mCp_dataset->get_var("time");
	time_units( lCp_time_var->get_att("units")->as_string(0) );
	md_delta_t = lCp_time_var->get_att("delta_t")->as_double(0);
      }
    }

    /**
     * Open netCDF file <acp_filename>
     *
     * @param acp_filename file name
     * @param ae_filemode file mode
     */
    void GeoNetCDF::open(const char* acp_filename,
			 NcFile::FileMode ae_filemode)
    {
      mb_is_mutable = (ae_filemode == NcFile::ReadOnly);

      mCp_dataset.reset(new NcFile(acp_filename,ae_filemode) );

      if (isa_timeseries()) {
	NcVar* lCp_time_var = mCp_dataset->get_var("time");
	time_units( lCp_time_var->get_att("units")->as_string(0) );
	md_delta_t = lCp_time_var->get_att("delta_t")->as_double(0);
      }
    }

    /**
     * Closes the netCDF file
     *
     * @returns whether operation was successful
     */
    bool GeoNetCDF::close() {
      if (mCp_dataset.get() != 0)
	return (bool)mCp_dataset->close();

      return false;
    }

    /**
     * Adds a variable to the dataset.
     *
     * @param acp_name variable name
     * @param ae_type netCDF data type
     */
    bool GeoNetCDF::add_geogrid(const char* acp_name, NcType ae_type) {
      if (!is_georeferenced())
	return false;

      // create a vector of variable dimensions
      std::vector<NcDim*> lCp1_DimVector;

      for (int li_dim = 0; li_dim < mCp_dataset->num_dims(); li_dim++) {
	lCp1_DimVector.push_back(mCp_dataset->get_dim(li_dim));
      }

      // reverse the order of the dimensions
      const NcDim** lCp1_dims = new const NcDim* [lCp1_DimVector.size()];
      for (long i = 0; i < lCp1_DimVector.size(); i++)
	lCp1_dims[i] = lCp1_DimVector[i];

      // add variable
      NcVar* lCp_var = 0;

//       lCp_var = mCp_dataset->add_var(acp_name,
// 				     ae_type,
// 				     lCp1_DimVector.size(),
// 				     lCp1_dims);
      switch (lCp1_DimVector.size()) {
      case 1:
	lCp_var = mCp_dataset->add_var(acp_name,
				       ae_type,
				       lCp1_dims[0]);
	break;
      case 2:
	lCp_var = mCp_dataset->add_var(acp_name,
				       ae_type,
				       lCp1_dims[0],
				       lCp1_dims[1]);
	break;
      case 3:
	lCp_var = mCp_dataset->add_var(acp_name,
				       ae_type,
				       lCp1_dims[0],
				       lCp1_dims[1],
				       lCp1_dims[2]);
	break;
      case 4:
	lCp_var = mCp_dataset->add_var(acp_name,
				       ae_type,
				       lCp1_dims[0],
				       lCp1_dims[1],
				       lCp1_dims[2],
				       lCp1_dims[3]);
	break;
      default:			// otherwise invalid
	break;
      }

      delete lCp1_dims;

      if (lCp_var == 0)
	return false;

      lCp_var->add_att("grid_mapping", TM);

      return true;

    } // GeoNetCDF::add_var(const char*, NcType)

    /** @returns whether this dataset is a time series */
    bool GeoNetCDF::isa_timeseries() const {
      for (long i = 0; i < mCp_dataset->num_dims(); i++) {
	if (std::string(mCp_dataset->get_dim(i)->name()) == "time") {
	  return true;
	}
      }

      return false;
    }

    /** @returns whether this dataset is properly georeferenced */
    bool GeoNetCDF::is_georeferenced() const {
      if (mCp_dataset == 0)
	return false;

      //  2007-07-15 note: moved georeferencing to "transverse_mercator"
      NcVar* lCp_var = mCp_dataset->get_var(TM);
      if (lCp_var == 0)
	return false;

      // 2006-08-21 note: this test is not complete
      return ( mCp_dataset->get_dim("x") != 0 &&
	       mCp_dataset->get_dim("y") != 0 &&
	       lCp_var->get_att("Northernmost_Northing") != 0 &&
	       lCp_var->get_att("Southernmost_Northing") != 0 &&
	       lCp_var->get_att("Easternmost_Easting") != 0 &&
	       lCp_var->get_att("Westernmost_Easting") != 0 &&
	       lCp_var->get_att("spatial_ref") != 0 );
    }

    /**
     * Returns whether this variable is properly georeferenced.
     *
     * @param acp_name variable name
     * @returns whether this variable is properly georeferenced
     */
    bool GeoNetCDF::is_georeferenced(const char* acp_name) const {
      if (!is_georeferenced())
	return false;

      NcVar* lCp_var = mCp_dataset->get_var(acp_name);
      if (lCp_var == 0)
	return false;

      // check all variable dimensions
      int li_cnt = 0;
      for (int i = 0; i < lCp_var->num_dims(); i++) {
	if (std::string(lCp_var->get_dim(i)->name()) == "x" ||
	    std::string(lCp_var->get_dim(i)->name()) == "y")
	  ++li_cnt;
      }

      return (li_cnt == 2);
    }

    /** @returns number of rows */
    long GeoNetCDF::num_rows() const {
      if (!mCp_dataset->is_valid())
	return 0;

      NcDim* lCp_dim = mCp_dataset->get_dim("y");
      if (lCp_dim == 0)
	return 0;

      return lCp_dim->size();
    }

    /** @returns number of columns */
    long GeoNetCDF::num_cols() const {
      if (!mCp_dataset->is_valid())
	return 0;

      NcDim* lCp_dim = mCp_dataset->get_dim("x");
      if (lCp_dim == 0)
	return 0;

      return lCp_dim->size();
    }

    /** @returns number of bands */
    long GeoNetCDF::num_bands() const {
      if (!is_georeferenced())
	return 0;

      bool lb_found_dim = false;
      for (long i = 0; i < mCp_dataset->num_dims(); i++)
	if ( (lb_found_dim =
	      (std::string(mCp_dataset->get_dim(i)->name()) == "z")) )
	  break;

      if (!lb_found_dim)
	return 1;

      return mCp_dataset->get_dim("z")->size();
    }

    /** @returns envelope of netCDF dataset spatial extent */
    geos::geom::Envelope GeoNetCDF::envelope() const {
      geos::geom::Envelope lC_envelope;

      if (is_georeferenced()) {

	NcVar* lCp_var = mCp_dataset->get_var(TM);

	double ld_minX =
	  lCp_var->get_att("Westernmost_Easting")->as_double(0);
	double ld_maxX =
	  lCp_var->get_att("Easternmost_Easting")->as_double(0);
	double ld_minY =
	  lCp_var->get_att("Southernmost_Northing")->as_double(0);
	double ld_maxY =
	  lCp_var->get_att("Northernmost_Northing")->as_double(0);

	lC_envelope.init( ld_minX, ld_maxX,
			  ld_minY, ld_maxY );
      }

      return lC_envelope;
    }

    /** @returns projection reference */
    const char* GeoNetCDF::spatial_ref() const {
      if (is_georeferenced())
	return ( (const char*)mCp_dataset->get_var(TM)->get_att("spatial_ref")->as_string(0) );


      return std::string("").c_str();
    }

    /**
     * Returns a handle to a new Geogrid <acp_name>.
     *
     * @param acp_name grid name
     * @returns handle to new Geogrid
     */
    Geogrid* GeoNetCDF::create_geogrid(const char* acp_name) const {
      Geogrid* lCp_geogrid = 0;

      if (is_georeferenced()) {
	NcVar* lCp_var = mCp_dataset->get_var(acp_name);
	if (lCp_var != 0) {
	  lCp_geogrid = CreateGeogrid(lCp_var->type());
	  if (lCp_geogrid != 0) {
	    lCp_geogrid->name(acp_name);
	    lCp_geogrid->resize( num_cols(),
				 num_rows(),
				 num_bands(),
				 envelope(),
				 spatial_ref() );
// 	    if (mCp_dataset->rec_dim()->size() > 0)
// 	      read(lCp_geogrid,0); // initialize geogrid

	  } // if (lCp_geogrid ! = 0)
	}   // if (lCp_var != 0)
      }	    // if (is_georeferenced())

      return lCp_geogrid;
    }


    /**
     * Reads a dataset record to a Geogrid object.
     *
     * @param aCp_geogrid handle to Geogrid
     * @param al_index record index
     * @return error status
     * @throws std::out_of_range
     */
    int GeoNetCDF::read(Geogrid* aCp_geogrid,
			long al_index) const
      throw (std::out_of_range)
    {
      // read in geogrid data
      int li_status = 1;	// presume failure
      if (dynamic_cast<CharGeogrid*>(aCp_geogrid))
	li_status = this->read((CharGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<ShortGeogrid*>(aCp_geogrid))
	li_status = this->read((ShortGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<LongGeogrid*>(aCp_geogrid))
	li_status = this->read((LongGeogrid&)*aCp_geogrid,al_index);
      else if (dynamic_cast<FloatGeogrid*>(aCp_geogrid))
	li_status = this->read((FloatGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<DoubleGeogrid*>(aCp_geogrid))
	li_status = this->read((DoubleGeogrid&)*aCp_geogrid, al_index);

      return li_status;

    } // GeoNetCDF::read(Geogrid*,const char*) const

    /**
     * writes a dataset record from a Geogrid object.
     *
     * @param aCp_geogrid handle to Geogrid
     * @param al_index record index
     * @return error status
     * @throws std::out_of_range
     */
    int GeoNetCDF::write(const Geogrid* aCp_geogrid,
			 long al_index)
      throw (std::out_of_range)
    {
      // write geogrid data
      int li_status = 1;
      if (dynamic_cast<const CharGeogrid*>(aCp_geogrid))
	li_status = this->write((const CharGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<const ShortGeogrid*>(aCp_geogrid))
	li_status = this->write((const ShortGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<const LongGeogrid*>(aCp_geogrid))
	li_status = this->write((const LongGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<const FloatGeogrid*>(aCp_geogrid))
	li_status = this->write((const FloatGeogrid&)*aCp_geogrid, al_index);
      else if (dynamic_cast<const DoubleGeogrid*>(aCp_geogrid))
	li_status = this->write((const DoubleGeogrid&)*aCp_geogrid, al_index);

      return li_status;

    } // GeoNetCDF::write(const Geogrid*, const char*)

    /**
     * Sets the time units attribute of the dataset.
     *
     * @param acp_units time units as a string
     */
    void GeoNetCDF::time_units(const char* acp_units) {
      std::string lC_units = acp_units;

      char c = ' ';
      std::string::size_type i = 0;
      std::string::size_type j = lC_units.find(c);
      std::vector<std::string> lC1_fields;
      while (j != std::string::npos) {
	lC1_fields.push_back(lC_units.substr(i, j-i));
	i = ++j;
	j = lC_units.find(c, j);

	if (j == std::string::npos)
	  lC1_fields.push_back(lC_units.substr(i, lC_units.length( )));
      }

      if (lC1_fields.size() >= 4) {

	// first field should be time unit (e.g. days)
	double ld_seconds_per_year = 365.242*60*60*24;
	double ld_seconds_per_month = ld_seconds_per_year/12.0;

	if (lC1_fields[0] == "seconds")
	  mC_time_unit = seconds(1);
	else if (lC1_fields[0] == "minutes")
	  mC_time_unit = minutes(1);
	else if (lC1_fields[0] == "hours")
	  mC_time_unit = hours(1);
	else if (lC1_fields[0] == "days")
	  mC_time_unit = hours(24);
	else if (lC1_fields[0] == "weeks")
	  mC_time_unit = hours(24)*7;
	else if (lC1_fields[0] == "months")
	  mC_time_unit = seconds((long)ld_seconds_per_month);
	else if (lC1_fields[0] == "year")
	  mC_time_unit = seconds((long)ld_seconds_per_year);

	// second field should equal 'since' (ignore)
	// third field should be the base date (iso extended format)
	std::string lC_base_date = lC1_fields[2];

	i = lC_base_date.find('T'); // replace 'T' date time separator with ' '
	if (i != std::string::npos)
	  lC_base_date.replace(i,1," ");

	mC_base_date = time_from_string(lC_base_date.c_str());
      }

    } // GeoNetCDF::time_units(const char*)

    /**
     * Sets the time units for the dataset.
     *
     * @param ad_units delta_t in time units
     * @param acp_unit time unit
     * @param aCr_base_date base date
     */
    void GeoNetCDF::time_units(double ad_units,
			       const char* acp_unit,
			       ptime& aCr_base_date)
    {
      // set time unit
      double ld_seconds_per_year = 365.242*60*60*24;
      double ld_seconds_per_month = ld_seconds_per_year/12.0;

      std::string lC_unit = acp_unit;
      if (lC_unit == "seconds")
	  mC_time_unit = seconds(1);
      else if (lC_unit == "minutes")
	mC_time_unit = minutes(1);
      else if (lC_unit == "hours")
	mC_time_unit = hours(1);
      else if (lC_unit == "days")
	mC_time_unit = hours(24);
      else if (lC_unit == "weeks")
	mC_time_unit = hours(24)*7;
      else if (lC_unit == "months")
	mC_time_unit = seconds((long)ld_seconds_per_month);
      else if (lC_unit == "year")
	mC_time_unit = seconds((long)ld_seconds_per_year);

      std::string lC_time_units = lC_unit + " since ";

      // set base date
      mC_base_date = aCr_base_date;

      lC_time_units +=
	to_iso_extended_string(mC_base_date) + std::string(" in standard");

      md_delta_t = ad_units;

      if (isa_timeseries()) {
	NcVar* lCp_time_var = mCp_dataset->get_var("time");

	lCp_time_var->add_att("delta_t", md_delta_t);
	lCp_time_var->add_att("units", lC_time_units.c_str());
      }

    } // GeoNetCDF::time_units(double,const char*, ptime&)

    /** @returns <time_unit> */
    time_duration GeoNetCDF::time_unit() const {
      return mC_time_unit;
    }

    /** @returns <base_date> */
    ptime GeoNetCDF::base_date() const {
      return mC_base_date;
    }

    /** @returns delta_t attribute (time duration in units) */
    double GeoNetCDF::delta_t() const {
      return md_delta_t;
    }

    /**
     * Returns the date_time associated with the specified index.
     *
     * @param al_index
     * @throws std::out_of_range
     */
    ptime GeoNetCDF::date(long al_index) const
      throw (std::out_of_range)
    {
      if (!isa_timeseries())
	return not_a_date_time;

      long ll_max_index = num_recs()-1;

      if (al_index < 0 || al_index > ll_max_index) {
	std::ostringstream lC_error_msg;
	lC_error_msg << "index <" << al_index << "> is out of the valid range "
		     << "[0-" << ll_max_index << "]";
	throw (std::out_of_range(lC_error_msg.str()));
      }

      ptime lC_date = base_date();
      time_duration lC_time_step = time_unit(); // 1 time unit
      if (!lC_date.is_special()) {
	if (lC_time_step.total_milliseconds() != 0) { // fixed time step
	  lC_time_step *= (long)delta_t();
	  lC_time_step *= al_index;
	}
	else {				// retrieve time coordinate
	  NcValues* lCp_time;
	  lCp_time = mCp_dataset->get_var("time")->get_rec(al_index);
	  lC_time_step *= lCp_time->as_long(0);
	  delete lCp_time;
	}
	lC_date += lC_time_step; // date relative to base date
      }

      return lC_date;
    }


   /**
     * Utility method for creating Geogrid based on NcType data type.
     *
     * @param eType NcType data type
     * @returns handle to new Geogrid object
     */
    Geogrid* CreateGeogrid(NcType eType) {

      Geogrid* lCp_geogrid = NULL;

      switch (eType) {
      case ncByte:
      case ncChar:
	lCp_geogrid = new CharGeogrid;
	break;
      case ncShort:
	lCp_geogrid = new ShortGeogrid;
	break;
      case ncInt:
	lCp_geogrid = new LongGeogrid;
	break;
      case ncFloat:
	lCp_geogrid = new FloatGeogrid;
	break;
      case ncDouble:
      default:
	lCp_geogrid = new DoubleGeogrid;
      }

      return lCp_geogrid;

    } // CreateGeogrid(NcType)

    /**
     * Returns NcType that corresponds to geogrid specified.
     *
     * @param aCp_geogrid handle to geogrid
     * @returns NcType that corresponds to geogrid specified
     */
    NcType getNcType(const Geogrid* aCp_geogrid)
    {
      NcType eType;
      if (dynamic_cast<const ByteGeogrid*>(aCp_geogrid))
	eType = ncByte;
      else if (dynamic_cast<const CharGeogrid*>(aCp_geogrid))
	eType = ncByte;
      else if (dynamic_cast<const ShortGeogrid*>(aCp_geogrid))
	eType = ncShort;
      else if (dynamic_cast<const IntGeogrid*>(aCp_geogrid))
	eType = ncInt;
      else if (dynamic_cast<const LongGeogrid*>(aCp_geogrid))
	eType = ncLong;
      else if (dynamic_cast<const FloatGeogrid*>(aCp_geogrid))
	eType = ncFloat;
      else if (dynamic_cast<const DoubleGeogrid*>(aCp_geogrid))
	eType = ncDouble;

      return eType;
    }

    /**
     * Utility function for creating a GeoNetCDF dataset for writing from a
     * geogrid.
     *
     * @param acp_filename filename
     * @param aCp_geogrid handle to geogrid object
     */
    GeoNetCDF* createGeoNetCDF(const char* acp_filename,
			       const Geogrid& aCr_geogrid)
    {
      bool lb_success = false;

      // open new GeoNetCDF file
      GeoNetCDF* lCp_GeoNetCDF =
	new GeoNetCDF(acp_filename,
		      aCr_geogrid.num_cols(),
		      aCr_geogrid.num_rows(),
		      aCr_geogrid.num_bands(),
		      aCr_geogrid.envelope(),
		      aCr_geogrid.spatial_ref(),
		      false);
      lCp_GeoNetCDF->add_geogrid(aCr_geogrid.name(),
				 getNcType(&aCr_geogrid));
      lCp_GeoNetCDF->write(&aCr_geogrid, 0);
      lb_success = lCp_GeoNetCDF->is_open();

      if (!lb_success) {
	delete lCp_GeoNetCDF;
	return 0;
      }

      return lCp_GeoNetCDF;

    } // createGeoNetCDF(...)

  } // namespace gis

} // namespace efscape
