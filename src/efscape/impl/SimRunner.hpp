// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_SIMRUNNER_HPP
#define EFSCAPE_IMPL_SIMRUNNER_HPP

// boost serialization definitions
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>

// parent class definition
#include <efscape/impl/adevs_config.hpp>

// data member definitions
#include <efscape/impl/ClockI.hpp>
#include <boost/property_tree/ptree.hpp>

#include <adevs/adevs_exception.h>

namespace efscape {

  namespace impl {

    /**
     * Implements an adevs-based model wrapper that encapsulates a simulation
     * model session. It replace the efscape::impl::AdevsModel
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.1.1 created 27 Apr 2017, revised 09 June 2017
     */
    class SimRunner : public ModelWrapperBase
    {
      friend class boost::serialization::access;

    public:

      /** constructor */
      SimRunner();

      /** destructor */
      ~SimRunner();

      //-------------------------
      // accessor/mutator methods
      //-------------------------

      /**
       * Class method that returns information about this model class.
       *
       * @returns info about this model class
       */
      static boost::property_tree::ptree get_info();
 
      ///
      /// model configuration/properties
      ///

      /** @returns model configuration in JSON */
      std::string convert_to_json() const;

      /**
       * Sets the model properties
       *
       * @param aC_modelJson model JSON configuration string
       * @returns number of properties found
       */
      virtual unsigned int
      convert_from_json(const std::string& aC_modelJson);

      //
      // clock
      //
      /** @returns reference to clock */
      ClockIPtr& getClockIPtr();

      /**
       * Sets the handle to a clock.
       *
       * @param aCr_clock handle to clock
       */
      void setClockIPtr(const ClockIPtr& aCr_clock);

      /** @returns handle to clock */
      const ClockI* getClock() const;

      //---------------------------
      // adevs ModelWrapper methods
      //---------------------------

      /// Overriding parent methods

      /**
       * Atomic internal transition function.
       */
      void delta_int();

      /**
       * Atomic external transition function.
       *
       * @param e time
       * @param xb bag of events
       */
      void delta_ext(double e, const adevs::Bag<IO_Type>& xb);

      /**
       * Atomic confluence transition function.
       *
       * @param xb bag of events
       */
      void delta_conf(const adevs::Bag<IO_Type>& xb);

      /**
       * Atomic time advance function
       *
       * @returns time advance
       */
      double ta();
      
      /// implementing abtract methods
      
      /**
       * This method is used to translate incoming input objects into input
       * objects that the wrapped model can process.
       *
       * @param external_input bag of external inputs for the ModelWrapper
       * @param internal_input bag of internal inputs for the internal model
       */
      void translateInput(const adevs::Bag<IO_Type>& external_input,
			  adevs::Bag<adevs::Event<IO_Type> >& internal_input);

      /**
       * This method is used to translate outgoing output objects into objects
       * that the ModelWrapper can produce.
       *
       * @param internal_output bag of output from the internal model
       * @param external_output bag of output from the ModelWrapper
       */     
      void translateOutput(const
			   adevs::Bag<adevs::Event<IO_Type> >&
			   internal_output,
			   adevs::Bag<IO_Type>& external_output);

      /**
       * This is the garbage collection method for internal input events.
       *
       * @param g bag of events
       */
      void gc_input(adevs::Bag<adevs::Event<IO_Type> >& g);

      /**
       * Output value garbage collection.
       *
       * @param g bag of events
       */
      void gc_output(adevs::Bag<IO_Type>& g);

      //-----------------
      // devs model ports
      //-----------------
      /** clock input port */
      static const efscape::impl::PortType clock_in;

      /** properties input port */
      static const efscape::impl::PortType properties_in;

    protected:
      
      /**
       * Create the wrapped model (to be overridden by derived classes)
       *
       * @throws adevs::exception
       */
      virtual void createModel()
	throw(adevs::exception);

      /** model configuration as a JSON string */
      std::string mC_modelJson;
      
      /** simulation clock (implementation) */
      ClockIPtr mCp_ClockI;

    private:

      template<class Archive>
      void save(Archive & ar, const unsigned int version) const
      {
	// save parent class data
	ar & boost::serialization::make_nvp("ModelWrapper",
					    boost::serialization::base_object<ModelWrapperBase>(*this) );

	// save model configuration
	ar  & boost::serialization::make_nvp("modelJson",mC_modelJson);
	
	// save clock
	ar  & boost::serialization::make_nvp("Clock",mCp_ClockI);

	// save the wrapped model
	const DEVS* lCp_wrappedModel =
	  this->ModelWrapperBase::getWrappedModel();
	ar & boost::serialization::make_nvp("WrappedModel", lCp_wrappedModel );
      }

      template<class Archive>
      void load(Archive & ar, const unsigned int version)
      {
	// load parent class data
	ar & boost::serialization::make_nvp("ModelWrapper",
					    boost::serialization::base_object<ModelWrapperBase>(*this) );

	// save model configuration
	ar  & boost::serialization::make_nvp("modelJson",mC_modelJson);

	// load clock
	ar  & boost::serialization::make_nvp("Clock",mCp_ClockI);

	// save the wrapped model
	DEVS* lCp_wrappedModel = NULL;
	ar & boost::serialization::make_nvp("WrappedModel", lCp_wrappedModel );
	this->ModelWrapperBase::setWrappedModel( lCp_wrappedModel );
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER();

    };

  } // namespace impl

} // namespace efscape

BOOST_CLASS_VERSION(efscape::impl::SimRunner,5)

#endif	// #ifndef EFSCAPE_IMPL_SIMRUNNER_HPP
