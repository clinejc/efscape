// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelI.hpp
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
#ifndef EFSCAPE_SERVER_MODELTIE_HPP
#define EFSCAPE_SERVER_MODELTIE_HPP

// parent class definitions
#include <Ice/Ice.h>
#include <efscape/Model.h>

#include <efscape/impl/efscapelib.hpp>
#include <json/json.h>

/**
 * Implements the ICE interface efscape::Model and provides an abstract
 * interface for server-side models.
 *
 * @author Jon Cline <clinej@stanfordalumni.org>
 * @version 1.2.0 created 18 Oct 2008, revised 24 Jan 2019
 */
class ModelI : virtual public efscape::Model,
		 public adevs::EventListener<efscape::impl::IO_Type>
{
public:

  ModelI();
  ModelI(const efscape::impl::DEVSPtr& aCp_model,
	 Json::Value aC_info = Json::Value());
  ModelI(const efscape::impl::DEVSPtr& aCp_model,
	   const char* acp_name);
  ~ModelI();

  //
  // ICE interface efscape::Entity
  //
  virtual std::string getName(const Ice::Current&) const override;

  //
  // ICE interface efscape::Model
  //
  virtual bool initialize(const Ice::Current&) override;
  virtual double timeAdvance(const Ice::Current&) override;
  virtual bool internalTransition(const Ice::Current&) override;
  virtual bool externalTransition(double,
				  efscape::Message,
				  const Ice::Current&) override;
  virtual bool confluentTransition(efscape::Message,
				   const Ice::Current&) override;

  virtual efscape::Message outputFunction(const Ice::Current&) override;

  virtual std::string getType(const Ice::Current&) const override;
  virtual void setName(std::string,
		       const Ice::Current&) override;

  virtual std::string saveJSON(const Ice::Current&) override;

  virtual void destroy(const Ice::Current&) override;

  //
  // local (server-side) interfaces
  //

  //---------------------------
  // adevs EventListener method
  //---------------------------
  virtual void outputEvent(adevs::Event<efscape::impl::IO_Type> x, double t) override;

  /**
   * Sets the associated model.
   *
   * @param aCp_model handle to adevs::devs object
   */
  void setWrappedModel(const efscape::impl::DEVSPtr& aCp_model) {
    mCp_WrappedModel = aCp_model;
  }

  /** @returns handle to associated model */
  efscape::impl::DEVSPtr getWrappedModel() {
    return mCp_WrappedModel;
  }

protected:

  virtual
  void translateOutput(const Ice::Current& aCr_current,
		       efscape::Message& aCr_external_output);
  virtual
  void translateInput(const Ice::Current& aCr_current,
		      const efscape::Message& aCr_external_input,
		      adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
		      aCr_internal_input);

  /** handle to simulator */
  std::unique_ptr< adevs::Simulator<efscape::impl::IO_Type> >
  mCp_simulator;

  /** handle to model */
  efscape::impl::DEVSPtr mCp_WrappedModel;

  /** output buffer */
  adevs::Bag< adevs::Event<efscape::impl::IO_Type> > mCC_OutputBuffer;

  /** model metadata (may include scenario/session-specific info) */
  Json::Value mC_info;

private:

  /** model name */
  std::string mC_name;

};				// class ModelI

#endif	// #ifndef EFSCAPE_SERVER_MODELTIE_HPP
