// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockI.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/ClockI.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <boost/algorithm/string.hpp>

// using namespace repast;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace efscape {

  namespace impl {

    /** constructor */
    ClockI::ClockI() :
      md_TimeCurrent(0.),
      md_TimeDelta(1.),
      md_TimeMax(0.)
    {
    }

    /** copy constructor */
    ClockI::ClockI(const ClockI& aCr_clock)
    {
      md_TimeCurrent = aCr_clock.md_TimeCurrent;
      md_TimeDelta = aCr_clock.md_TimeDelta;
      md_TimeMax = aCr_clock.md_TimeMax;
      mC_base_date = aCr_clock.mC_base_date;
      mC_units = aCr_clock.mC_units;
    }

    /** destructor */
    ClockI::~ClockI() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Deleting ClockI...");
    }

    /**
     * Returns the date time that corresponds with simulation time.
     *
     * @param ad_time simulation time
     * @returns simulation date time
     */
    boost::posix_time::ptime ClockI::date_time( double ad_time ) const {
      if (mC_base_date.is_not_a_date_time() || mC_units.is_not_a_date_time() ) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Not a date time");
	return mC_base_date;
      }

      time_duration lC_duration = mC_units*((int)ad_time);

      return (mC_base_date+lC_duration);
    }

    /**
     * Sets the base date/time of the simulation.
     * 
     * @param aCr_datetime
     */
    void ClockI::base_date(const ptime& aCr_datetime) {
      mC_base_date = aCr_datetime;
    }

    /**
     * Sets the time units of the simulation
     *
     * @param aCr_units time units
     */
    void ClockI::units(const boost::posix_time::time_duration& aCr_units) {
      mC_units = aCr_units;
    }

    /**
     * Returns the time units in a string.\n\n
     *
     * "time unit since reference time"
     *
     * @returns time units
     */
    std::string ClockI::timeUnits() const {
      if (date_time(0).is_not_a_date_time()) {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Not a date time");
	return "";
      }

      double ld_seconds_per_year = 365.242*60*60*24;
      double ld_seconds_per_month = ld_seconds_per_year/12.0;

      std::string lC_units_string;
      if (mC_units == seconds(1))
	lC_units_string = "seconds";
      else if (mC_units == minutes(1))
	lC_units_string = "minutes";
      else if (mC_units == hours(1))
	lC_units_string = "hours";
      else if (mC_units == hours(24))
	lC_units_string = "days";
      else if (mC_units == hours(24)*7)
	lC_units_string = "weeks";
      else if (mC_units == seconds((long)ld_seconds_per_month))
	lC_units_string = "months";
      else if (mC_units == seconds((long)ld_seconds_per_year))
	lC_units_string = "years";
      else {
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "time units <"
		      << mC_units << "> not recognized");
	return "";
      }

      // std::ostringstream lC_buffer;
      // lC_buffer << md_TimeDelta;

      lC_units_string = /*lC_buffer.str() + " " +*/ lC_units_string + " since "
	+ to_iso_extended_string(mC_base_date) + " in standard";

      return lC_units_string;
    }

    /**
     * Sets the time units from a string.
     *
     * "time unit since reference time"
     *
     * @param acp_units time units
     */
    void ClockI::timeUnits(const char* acp_units)
    {
      std::string lC_units = acp_units;
      std::vector<std::string> lC1_fields;

      // split the units string into fields
      boost::split(lC1_fields, lC_units, boost::algorithm::is_space());
      if (lC1_fields.size() >= 3) {

	// first field should be time unit (e.g. days)
	double ld_seconds_per_year = 365.242*60*60*24;
	double ld_seconds_per_month = ld_seconds_per_year/12.0;

	if (lC1_fields[0] == "seconds")
	  mC_units = seconds(1);
	else if (lC1_fields[0] == "minutes")
	  mC_units = minutes(1);
	else if (lC1_fields[0] == "hours")
	  mC_units = hours(1);
	else if (lC1_fields[0] == "days")
	  mC_units = hours(24);
	else if (lC1_fields[0] == "weeks")
	  mC_units = hours(24)*7;
	else if (lC1_fields[0] == "months")
	  mC_units = seconds((long)ld_seconds_per_month);
	else if (lC1_fields[0] == "year")
	  mC_units = seconds((long)ld_seconds_per_year);

	// second field should equal 'since' (ignore)
	// third field should be the base date (iso extended format)
	std::string lC_base_date = lC1_fields[2];
	std::string::size_type i =
	  lC_base_date.find('T'); // replace 'T' date time separator with ' '
	if (i != std::string::npos)
	  lC_base_date.replace(i,1," ");

	mC_base_date = time_from_string(lC_base_date.c_str());
      }
    }

  } // namespace impl

} // namespace efscape
