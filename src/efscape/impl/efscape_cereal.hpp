// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscape_cereal.hpp
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
#ifndef EFSCAPE_IMPL_EFSCAPE_CEREAL_HPP
#define EFSCAPE_IMPL_EFSCAPE_CEREAL_HPP

#include <cereal/cereal.hpp>

#include <efscape/impl/ClockI.hpp>

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

#endif // #ifndef EFSCAPE_IMPL_EFSCAPE_CEREAL_HPP
