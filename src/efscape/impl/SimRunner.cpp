// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.cpp
// Copyright (C) 2006-2019 Jon C. Cline
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

// class definition
#include <efscape/impl/SimRunner.hpp>

#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>
#include <efscape/impl/ModelType.hpp>

#include <sstream>

namespace efscape
{

namespace impl
{

SimRunner::SimRunner() : SimRunner(Json::nullValue)
{
}

SimRunner::SimRunner(Json::Value aC_modelProps) : ModelWrapperBase(),
                                                  mC_modelProps(aC_modelProps)
{
  // initialize the clock
  //
  mCp_ClockI.reset(new ClockI);
  mCp_ClockI->timeMax() = DBL_MAX;

  return;

  // load and set wrapped model
  //
  std::string lC_ClassName = "gpt::GPT"; // default root model is of class efscape::impl::AdevsModel
  LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                "Retrieving the model from the factory");
  DEVSPtr lCp_model(Singleton<ModelHomeI>::Instance()
                        .getModelFactory()
                        .createObject(lC_ClassName));

  if (lCp_model == nullptr)
  {
    LOG4CXX_ERROR(ModelHomeI::getLogger(),
                  "Unable to create wrapped model <" << lC_ClassName << ">");
    return;
  }
  else
  {
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                  "Set wrapped model <" << lC_ClassName << ">");
  }

  setWrappedModel(lCp_model);

} // SimRunner::SimRunner(Json::Value)

SimRunner::~SimRunner()
{
}

void SimRunner::delta_int()
{
  // advance clock
  if (getTime() < adevs_inf<double>())
    getClockIPtr()->time() = getTime() + ta();

  ModelWrapperBase::delta_int();
}

void SimRunner::delta_ext(double e, const adevs::Bag<IO_Type> &xb)
{
  // advance clock
  if (e >= 0)
    getClockIPtr()->time() += e;

  ModelWrapperBase::delta_ext(e, xb);
}

void SimRunner::delta_conf(const adevs::Bag<IO_Type> &xb)
{
  // advance clock
  if (getTime() < adevs_inf<double>())
    getClockIPtr()->time() = getTime();

  ModelWrapperBase::delta_conf(xb);
}

double SimRunner::ta()
{
  double ld_delta_t = ModelWrapperBase::ta();
  if (getTime() + ld_delta_t > getClockIPtr()->timeMax())
    return adevs_inf<double>();

  return ModelWrapperBase::ta();
}

void SimRunner::translateInput(const adevs::Bag<IO_Type> &
                                   external_input,
                               adevs::Bag<adevs::Event<IO_Type>> &
                                   internal_input)
{
  for (auto i: external_input)
  {
    LOG4CXX_DEBUG(ModelHomeI::getLogger(),
                  "passing on port <" << i.port << ">");
    internal_input.insert(adevs::Event<IO_Type>(getWrappedModel().get(),
                                                i));
  }
}

void SimRunner::translateOutput(const adevs::Bag<adevs::Event<IO_Type>> &
                                    internal_output,
                                adevs::Bag<IO_Type> &external_output)
{
  adevs::Bag<adevs::Event<IO_Type>>::iterator iter;
  for (auto i: internal_output)
  {
    external_output.insert(IO_Type(i.value));
  }
}

void SimRunner::gc_input(adevs::Bag<adevs::Event<IO_Type>> &g)
{
}

void SimRunner::gc_output(adevs::Bag<efscape::impl::IO_Type> &g)
{
}

//-------------------------
// accessor/mutator methods
//-------------------------

///
/// clock
///
ClockIPtr &SimRunner::getClockIPtr() { return mCp_ClockI; }
const ClockIPtr &SimRunner::getClockIPtr() const { return mCp_ClockI; }

void SimRunner::setClockIPtr(const ClockIPtr &aCp_clock)
{
  mCp_ClockI = aCp_clock;
}

const ClockI &SimRunner::getClock() const { return *mCp_ClockI; }

} // namespace impl

} // namespace efscape
