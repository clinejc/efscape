// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockI.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_CLOCK_I_HH
#define EFSCAPE_IMPL_CLOCK_I_HH

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

namespace efscape {

  namespace impl {

    // forward declarations
    class ClockI;

    // typedefs
    typedef boost::shared_ptr<ClockI> ClockIPtr;

    /**
     * Defines the Clock class, which holds time-related information for
     * a Efscape simulation. The attributes are loosely based on the CDC netCDF
     * Conventions for Gridded Data.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 3.1.0 revised 14 Sep 2009
     */
    class ClockI
    {
      friend class boost::serialization::access;

    public:

      ClockI();
      ClockI(const ClockI& aCr_clock);

      virtual ~ClockI();

      /** @returns current time */
      double time() const { return md_TimeCurrent; }

      /** @returns current time */
      double getTime() { return md_TimeCurrent; }

      /** @returns reference to current time */
      double& time() { return md_TimeCurrent; }

      /** @returns time step */
      double timeDelta() const { return md_TimeDelta; }

      /** @returns reference to time step */
      double& timeDelta() { return md_TimeDelta; }

      /** @returns end time */
      double timeMax() const { return md_TimeMax; }

      /** @returns end time */
      double& timeMax() { return md_TimeMax; }

      boost::posix_time::ptime date_time( double ad_time ) const;
      void base_date(const boost::posix_time::ptime& aCr_datetime);
 
      /** @returns time units */
      boost::posix_time::time_duration units() const {
	return mC_units;
      }
      void units(const boost::posix_time::time_duration& aCr_units);

      std::string timeUnits() const;

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
	ar & boost::serialization::make_nvp("base_date", mC_base_date);
	ar & boost::serialization::make_nvp("time_units", mC_units);
	ar & boost::serialization::make_nvp("time",md_TimeCurrent);
	ar & boost::serialization::make_nvp("time_delta",md_TimeDelta);
	ar & boost::serialization::make_nvp("time_max",md_TimeMax);
      }

      /** current time */
      double md_TimeCurrent;

      /** time step */
      double md_TimeDelta;

      /** end time of the model simulation */
      double md_TimeMax;

      /** base date */
      boost::posix_time::ptime mC_base_date;

      /** time units */
      boost::posix_time::time_duration mC_units;

    };				// class ClockI definition

  } // namespace impl

} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_CLOCK_I_HH

