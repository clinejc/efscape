// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockI.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
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
    typedef std::shared_ptr<ClockI> ClockIPtr;

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
      void timeUnits(const char* acp_units);

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

#include <cereal/cereal.hpp>

// cereal serialization
namespace cereal {
  //
  // cereal serialization for ClockI
  //
  template<class Archive>
  void load(Archive & ar, efscape::impl::ClockI& aCr_clock)
  {
    double ld_time;
    double ld_timeDelta;
    double ld_timeMax;
    std::string lC_timeUnits;
    
    ar( cereal::make_nvp("time", ld_time),
	cereal::make_nvp("time_delta", ld_timeDelta),
	cereal::make_nvp("time_max", ld_timeMax),
	cereal::make_nvp("time_units", lC_timeUnits) );
    
    aCr_clock.time() = ld_time;
    aCr_clock.timeDelta() = ld_timeDelta;
    aCr_clock.timeMax() = ld_timeMax;
    aCr_clock.timeUnits(lC_timeUnits.c_str() );
  }

  template<class Archive>
  void save(Archive & ar, const efscape::impl::ClockI& aCr_clock)
  {
    double ld_time = aCr_clock.time();
    double ld_timeDelta = aCr_clock.timeDelta();
    double ld_timeMax = aCr_clock.timeMax();
    std::string lC_timeUnits = aCr_clock.timeUnits();
    
    ar( cereal::make_nvp("time", ld_time),
	cereal::make_nvp("time_delta", ld_timeDelta),
	cereal::make_nvp("time_max", ld_timeMax),
	cereal::make_nvp("time_units", lC_timeUnits) );
  }
  
} // namespace cereal

#endif	// #ifndef EFSCAPE_IMPL_CLOCK_I_HH

