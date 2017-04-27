// __COPYRIGHT_START__
// Package Name : efscape
// File Name : InitObject.hh
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_INITOBJECT_HH
#define EFSCAPE_IMPL_INITOBJECT_HH

#include <stdexcept>

namespace efscape {
  namespace impl {

    // forward declaration
    class InitObject;

    /**
     * An interface to provides an initialize() method to any derived class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.0.3 created 14 Feb 2009, revised 13 Sep 2009
     */
    class InitObject {
    public:

      /** Default constructor */
      InitObject() {}

      /** Virtual destructor */
      virtual ~InitObject() {}

      /**
       * Initializes the object (to be implemented by derived classes).
       */
      virtual void initialize()
	throw(std::logic_error) {}

    };				// class InitObject

  } // namespace impl
}   // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_INITOBJECT_HH
