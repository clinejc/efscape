// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelFactory.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#include <efscape/impl/ModelFactory.hpp>

#include <efscape/impl/ModelFactory.ipp>

namespace efscape {

  namespace impl {

    //
    // class ModelFactory implementation
    //
    /** constructor */
    ModelFactory::ModelFactory() {
      if ( ModelHomeI::getLogger() != NULL)
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Creating ModelFactory...");
    }

    /** destructor */
    ModelFactory::~ModelFactory() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Shutting down ModelFactory...");
    }

    //
    // class CellModelFactory implementation
    //
    /** constructor */
    CellModelFactory::CellModelFactory() {
      if ( ModelHomeI::getLogger() != NULL)
	LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		      "Creating CellModelFactory...");
    }

    /** destructor */
    CellModelFactory::~CellModelFactory() {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Shutting down CellModelFactory...");
    }

  } // namespace impl

} // namespace efscape
