// __COPYRIGHT_START__
// Package Name : efscape
// File Name : RepastModelWrapper.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_REPASTMODELWRAPPER_HPP
#define EFSCAPE_IMPL_REPASTMODELWRAPPER_HPP

// boost serialization definitions
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>

#include <efscape/impl/adevs_config.hpp>
#include <repast_hpc/Properties.h>
#include <boost/scoped_ptr.hpp>

namespace efscape {
  namespace impl {

    /**
     * Provides an ADEVS wrapper template class for Repast HPC models.
     * <br><br>
     * Assumes that the wrapped model has implemented a single function:
     *  -# void setup(repast::Properties&)
     *  -# const repast::Properties& getProperties()
     *  -# std::map<std::string,boost::any> getOutput()
     *    - Should return only output available for recording
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.3 created 09 Aug 2014, updated 05 Nov 2014
     *
     * ChangeLog:
     *  - 2014-11-06 implemented output_func(...)
     *    - added output of properties as a property_tree
     *    - added support for mCp_model->getOutput()
     *  - 2014-09-13 created version that loads the model.props file
     *  - 2014-08-09 created template class RepastModelWrapper<T>
     */
    template <class ModelType>
    class RepastModelWrapper : public ATOMIC
    {
      friend class boost::serialization::access;

    public:

      RepastModelWrapper();
      virtual ~RepastModelWrapper();

      //-------------------
      // devs model methods
      //-------------------

      /// Internal transition function.
      void delta_int();

      /// External transition function.
      void delta_ext(double e, const adevs::Bag<IO_Type>& xb);

      /// Confluent transition function.
      void delta_conf(const adevs::Bag<IO_Type>& xb);
 
      /// Output function.  
      void output_func(adevs::Bag<IO_Type>& yb);

      /// Time advance function.
      double ta();

      /// Output value garbage collection.
      void gc_output(adevs::Bag<IO_Type>& g);

      //-----------------
      // devs model ports
      //-----------------
      static const efscape::impl::PortType properties_in;

    private:

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version) const
      {
	// save parent class data
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ATOMIC);
      }

      /** handle to Repast model */
      boost::scoped_ptr<ModelType> mCp_model;

    };				// template class<> RepastModelWrapper

  } // namespace impl
}   // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_REPASTMODELWRAPPER_HPP
