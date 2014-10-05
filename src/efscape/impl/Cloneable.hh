// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Cloneable.hh
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_CLONEABLE_HH
#define EFSCAPE_IMPL_CLONEABLE_HH

namespace efscape {
  namespace impl {

    // forward declaration
    class Cloneable;

    /**
     * An interface to provides a clone() method to any derived class.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 0.01 created 14 Feb 2009
     */
    class Cloneable {
    public:

      /** Default constructor */
      Cloneable() {}

      /** Virtual destructor */
      virtual ~Cloneable() {}

      /**
       * Returns clone of this object (to be implemented by derived classes).
       *
       * @returns handle to new object copy
       */
      virtual Cloneable* clone() const { return 0; }

    };				// class Cloneable

  } // namespace impl
}   // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_CLONEABLE_HH
