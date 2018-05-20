// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeTie.hpp
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
#ifndef EFSCAPE_SERVER_MODELHOMETIE_HPP
#define EFSCAPE_SERVER_MODELHOMETIE_HPP

// parent class definition
#include <Ice/Ice.h>
#include <efscape/ModelHome.h>

/**
 * Implements the ICE efscape::ModelHome interface, a factory for simulation
 * models. It replaces the ModelRepository interface.
 *
 * @author Jon Cline <clinej@stanfordalumni.org>
 * @version 3.1.1 created 24 Dec 2006, revised 18 May 2018
 */
class ModelHomeTie : virtual public efscape::ModelHome
{
public:

  //
  // ICE interface ::efscape::ModelHome
  //
  virtual
  std::shared_ptr<efscape::ModelPrx>
  create(std::string,
	 const Ice::Current&) override;

  virtual
  std::shared_ptr<efscape::ModelPrx>
  createFromXML(std::wstring,
		const Ice::Current&) override;

  virtual
  std::shared_ptr<efscape::ModelPrx>
  createFromJSON(::std::string,
		 const Ice::Current&) override;

  virtual
  std::shared_ptr<::efscape::ModelPrx>
  createFromParameters(std::string,
		       const Ice::Current&) override;

  virtual
  efscape::ModelNameList getModelList(const Ice::Current&) override;

  virtual std::string getModelInfo(std::string,
				   const Ice::Current&) override;

  virtual
  std::shared_ptr<efscape::SimulatorPrx>
  createSim(std::shared_ptr<efscape::ModelPrx>,
	    const Ice::Current&) override;
  //
  // local (server-side) methods
  //

  ModelHomeTie();
  virtual ~ModelHomeTie();

};				// class ModelHomeTie

#endif	// #ifndef EFSCAPE_SERVER_MODELHOMETIE_HPP
