// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RelogoWrapper.hpp
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
#ifndef EFSCAPE_IMPL_RELOGOWRAPPER_HPP
#define EFSCAPE_IMPL_RELOGOWRAPPER_HPP

// boost serialization definitions
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>

#include <efscape/impl/efscapelib.hpp>
#include <relogo/SimulationRunnerPlus.h>
#include <json/json.h>

namespace efscape
{
namespace impl
{

/**
     * Provides an ADEVS wrapper template class for Repast HPC models.
     * <br><br>
     * Assumes that the wrapped model has implemented the following functions:
     *  -# const repast::Properties& getProperties()
     *  -# void setup(repast::Properties&)
     *  -# Json::Value outputFunction()
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.1 created 28 Jan 2019, updated 29 Jan 2019
     */
template <typename ObserverType, typename PatchType>
class RelogoWrapper : public ATOMIC
{
  friend class boost::serialization::access;

public:
  RelogoWrapper();
  RelogoWrapper(Json::Value aC_modelProps);
  virtual ~RelogoWrapper();

  //-------------------
  // devs model methods
  //-------------------

  /// Internal transition function.
  void delta_int();

  /// External transition function.
  void delta_ext(double e, const adevs::Bag<IO_Type> &xb);

  /// Confluent transition function.
  void delta_conf(const adevs::Bag<IO_Type> &xb);

  /// Output function.
  void output_func(adevs::Bag<IO_Type> &yb);

  /// Time advance function.
  double ta();

  /// Output value garbage collection.
  void gc_output(adevs::Bag<IO_Type> &g);

  //-----------------
  // devs model ports
  //-----------------
  static const efscape::impl::PortType setup_in;

private:
  void setup(std::string aC_propsFile);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) const
  {
    // save parent class data
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ATOMIC);
  }

  /** handle to Repast properties in JSON format */
  Json::Value mC_modelProps;

  /** handle to Repast model */
  std::unique_ptr< repast::relogo::SimulationRunnerPlus<ObserverType, PatchType> >
  mCp_model;

}; // template class<> RelogoWrapper

} // namespace impl
} // namespace efscape

#endif // #ifndef EFSCAPE_IMPL_RELOGOWRAPPER_HPP
