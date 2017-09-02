// __COPYRIGHT_START__
// Package Name : efscape
// File Name : SimRunner.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_SIMRUNNER_HPP
#define EFSCAPE_IMPL_SIMRUNNER_HPP

// cereal serialization definitions
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

// boost serialization definitions
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/shared_ptr.hpp>

// parent class definition
#include <efscape/impl/adevs_config.hpp>

// data member definitions
#include <efscape/impl/ClockI.hpp>
#include <efscape/utils/type.hpp>

#include <json/json.h>
#include <adevs/adevs_exception.h>

namespace efscape {

  namespace impl {

    /**
     * Implements an adevs-based model wrapper that encapsulates a simulation
     * model session. It replaces the efscape::impl::AdevsModel
     *
     * @author Jon C. Cline <clinej@stanfordalumni.org>
     * @version 0.2.0 created 27 Apr 2017, revised 11 Aug 2017
     */
    class SimRunner : public ModelWrapperBase
    {
    public:

      /** constructor */
      SimRunner();

      /**
       * constructor*
       *
       * @param aC_parameters embedded in a JSON string
       */
      SimRunner(Json::Value aC_parameters);

      /** destructor */
      ~SimRunner();

      //-------------------------
      // accessor/mutator methods
      //-------------------------

      /** @returns model type name */
      std::string getModelTypeName() const {
	return mC_modelTypeName;
      }

      /** @returns model name */
      std::string getModelName() const{
	return mC_modelName;
      }

      //
      // clock
      //

      /** @returns reference to clock */
      ClockIPtr& getClockIPtr();

      /** @returns reference to clock */
      const ClockIPtr& getClockIPtr() const;

      /**
       * Sets the handle to a clock.
       *
       * @param aCr_clock handle to clock
       */
      void setClockIPtr(const ClockIPtr& aCr_clock);

      /** @returns reference to clock */
      const ClockI& getClock() const;

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

      std::string mC_modelTypeName;
      std::string mC_modelName;

      /** simulation clock (implementation) */
      ClockIPtr mCp_ClockI;

    private:
      friend class cereal::access;

      template<class Archive>
      void serialize(Archive & ar)
      {
	// invoke serialization
	ar( cereal::make_nvp("adevs::ModelWrapper", // parent class
			     cereal::base_class<efscape::impl::ModelWrapperBase>(this) ),
	    cereal::make_nvp("modelTypeName", mC_modelTypeName),
	    cereal::make_nvp("modelName", mC_modelName),
	    cereal::make_nvp("clock", mCp_ClockI) );
      }
    };				// class SimRunner

  } // namespace impl

} // namespace efscape

namespace boost {

  namespace seralization {

    template<class Archive>
    void save(Archive & ar, const efscape::impl::SimRunner& simRunner,
	      const unsigned int version)
    {
      // first retrieve the wrapped model
      const efscape::impl::DEVSPtr lCp_wrappedModel =
	simRunner.getWrappedModel();

      efscape::impl::ClockIPtr lCp_clock =
	simRunner.getClockIPtr();

      // save parent class data
      ar & boost::serialization::make_nvp("ModelWrapper",
					  boost::serialization::base_object<efscape::impl::ModelWrapperBase>(simRunner) );

      // save clock
      ar  & boost::serialization::make_nvp("Clock", lCp_clock);

      // save the wrapped model
      ar & boost::serialization::make_nvp("WrappedModel", lCp_wrappedModel );
    }

    template<class Archive>
    void load(Archive & ar, efscape::impl::SimRunner& simRunner,
	      const unsigned int version)
    {
      efscape::impl::DEVSPtr lCp_wrappedModel;
      efscape::impl::ClockIPtr lCp_clock;

      // load parent class data
      ar & boost::serialization::make_nvp("ModelWrapper",
					  boost::serialization::base_object<efscape::impl::ModelWrapperBase>(simRunner) );

      // load clock
      ar  & boost::serialization::make_nvp("Clock", lCp_clock);

      // save the wrapped model
      ar & boost::serialization::make_nvp("WrappedModel", lCp_wrappedModel );

      simRunner.setWrappedModel( lCp_wrappedModel );
      simRunner.setClockIPtr(lCp_clock);
    }

  } // namespace serialization
} // namespace boost

#endif	// #ifndef EFSCAPE_IMPL_SIMRUNNER_HPP
