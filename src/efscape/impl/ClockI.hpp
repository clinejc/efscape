// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ClockI.hpp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_CLOCK_I_HPP
#define EFSCAPE_IMPL_CLOCK_I_HPP

#include <boost/date_time/posix_time/posix_time.hpp> // date_time definitions

// boost serialiation definitions
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

#endif	// #ifndef EFSCAPE_IMPL_CLOCK_I_HPP
