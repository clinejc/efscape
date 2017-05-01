// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelTie.hh
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_SERVER_MODELTIE_HPP
#define EFSCAPE_SERVER_MODELTIE_HPP

// parent class definitions
#include <Ice/Ice.h>
#include <efscape/efscape.h>

#include <efscape/impl/adevs_config.hpp>
#include <efscape/impl/ClockI.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace efscape {

  namespace server {

    /**
     * Implements the ICE interface ::efscape::Model and provides an abstract
     * interface for server-side models.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.0.0 created 18 Oct 2008, revised 22 Apr 2017
     */
    class ModelTie : virtual public ::efscape::Model,
		     public adevs::EventListener<efscape::impl::IO_Type>
    {
    public:

      ModelTie();
      ModelTie(adevs::Devs<efscape::impl::IO_Type>* aCp_model);
      ModelTie(adevs::Devs<efscape::impl::IO_Type>* aCp_model,
	       const char* acp_name);
      ~ModelTie();

      //
      // ICE interface efscape::Entity
      //
      virtual ::std::string getName(const Ice::Current&) const;

      //
      // ICE interface efscape::Model
      //
      virtual bool initialize(const Ice::Current&);
      virtual ::Ice::Double timeAdvance(const Ice::Current&);
      virtual bool internalTransition(const Ice::Current&);
      virtual bool externalTransition(::Ice::Double,
				      const ::efscape::Message&,
				      const Ice::Current&);
      virtual bool confluentTransition(const ::efscape::Message&,
				       const Ice::Current&);
      virtual ::efscape::Message outputFunction(const Ice::Current&);

      virtual ::std::string getType(const Ice::Current&) const;
      virtual ::std::string getBaseType(const Ice::Current&) const;
      virtual void setName(const std::string&,
			   const Ice::Current&);

      virtual ::efscape::PortDescriptions getInPorts(const Ice::Current&) const;
      virtual ::efscape::PortDescriptions getOutPorts(const Ice::Current&) const;

      virtual ::std::wstring saveXML(const Ice::Current&);

      virtual void destroy(const Ice::Current&);

      //
      // local (server-side) interfaces
      //

      //---------------------------
      // adevs EventListener method
      //---------------------------
      void outputEvent(adevs::Event<efscape::impl::IO_Type> x, double t);

      /**
       * Sets the associated model.
       *
       * @param aCp_model handle to adevs::devs object
       */
      void setWrappedModel(adevs::Devs<efscape::impl::IO_Type>* aCp_model) {
	mCp_WrappedModel.reset(aCp_model);
      }

      /** @returns handle to associated model */
      adevs::Devs<efscape::impl::IO_Type>* getWrappedModel() {
	return mCp_WrappedModel.get();
      }

    protected:

      virtual
      void translateOutput(const Ice::Current& aCr_current,
			   ::efscape::Message& aCr_external_output);
      virtual
      void translateInput(const Ice::Current& aCr_current,
			  const ::efscape::Message& aCr_external_input,
			  adevs::Bag<adevs::Event<efscape::impl::IO_Type> >&
			  aCr_internal_input);

      /** handle to simulator */
      boost::scoped_ptr< adevs::Simulator<efscape::impl::IO_Type> >
      mCp_simulator;

      /** handle to model */
      boost::scoped_ptr< adevs::Devs<efscape::impl::IO_Type> >
      mCp_WrappedModel;

      /** output buffer */
      adevs::Bag< adevs::Event<efscape::impl::IO_Type> > mCC_OutputBuffer;

      /** simulation clock (implementation) */
      ::efscape::impl::ClockIPtr mCp_clock;

    private:

      /** model name */
      std::string mC_name;

    };				// class ModelTie

  } // namespace server

} // namespace efscape

#endif	// #ifndef EFSCAPE_SERVER_MODELTIE_HPP
