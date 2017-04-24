// __COPYRIGHT_START__
// Package Name : efscape
// File Name : DynamicLibrary.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_UTILS_DYNAMICLIBRARY_HH
#define EFSCAPE_UTILS_DYNAMICLIBRARY_HH

#include <string>
#include <vector>


namespace efscape {

  namespace utils {

    /**
     * This class provides an interface to dynamic libraries.
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.0.0 created 29 Jan 2006, revised 10 Jul 2014
     */
    class DynamicLibrary {

    public:

      DynamicLibrary();
      DynamicLibrary(const char* acp_filename);

      /** destructor */
      virtual ~DynamicLibrary();

      //-----------------
      // accessor methods
      //-----------------
      /** @returns handle to library */
      void* handle() {
	return mp_handle;
      }

    protected:

      /** handle to library */
      void* mp_handle;
    };

  } // end of namespace utils

} // end of namespace efscape

#endif
