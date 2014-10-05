// __COPYRIGHT_START__
// Package Name : efscape
// File Name : EfscapeBuilder.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/impl/EfscapeBuilder.hh>

#include <efscape/utils/xerces_strings.hpp>

namespace efscape {

  namespace impl {

    /** constructor */
    EfscapeBuilder::EfscapeBuilder() : ModelBuilder() {}

    /** destructor */
    EfscapeBuilder::~EfscapeBuilder() {}

    /**
     * Creates a model specified by the configuration in a configuration file
     *
     * @param acp_filename name of configuration file
     * @returns handle to model object
     */
    adevs::Devs<IO_Type>* EfscapeBuilder::parse(const char* acp_filename) {

      std::string lC_message = "EfscapeBuilder::parse("
	+ std::string(acp_filename) + "):";

      return efscape::impl::loadAdevs(acp_filename);

    } // EfscapeBuilder::parse(const char*)

    //---------------
    // register class
    //---------------
    /**
     * Creates a new EfscapeBuilder.
     *
     * @returns a handle to a new EfscapeBuilder object
     */
    ModelBuilder* create_EfscapeBuilder() {
      return (ModelBuilder*)new EfscapeBuilder;
    }

    const bool registered =
      TheBuilderFactory::Instance().
      Register(EfscapeBuilder::name(),
	       create_EfscapeBuilder);

  } // namespace impl

}   // namespace efscape
