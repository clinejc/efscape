// __COPYRIGHT_START__
// Package Name : efscape
// File Name : DynamicLibrary.cpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/utils/DynamicLibrary.hpp>
#include <ltdl.h>
#include <iostream>

namespace efscape {

  namespace utils {

    //--------------------------------------------------
    // code borrowed from the llvm library (20 Jan 2006)
    //--------------------------------------------------
    static bool did_initialize_ltdl = false;

    static inline void check_ltdl_initialization() {
      if (!did_initialize_ltdl) {
	if (0 != lt_dlinit())
	  throw std::string(lt_dlerror());
	did_initialize_ltdl = true;
      }
    }

    static std::vector<lt_dlhandle> OpenedHandles;


    /** default constructor */
    DynamicLibrary::DynamicLibrary() : mp_handle(0) {
      check_ltdl_initialization();

      lt_dlhandle a_handle = lt_dlopen(0);

      if (a_handle == 0)
	throw std::string("Can't open program as dynamic library");

      mp_handle = a_handle;
      OpenedHandles.push_back(a_handle);

    } // end of DynamicLibrary::DynamicLibrary()

    /**
     * constructor 
     *
     * @param acp_filename library name
     */
    DynamicLibrary::DynamicLibrary(const char* acp_filename) : mp_handle(0) {
      check_ltdl_initialization();

      lt_dlhandle a_handle = lt_dlopen(acp_filename);

      if (a_handle == 0)
	a_handle = lt_dlopenext(acp_filename);

      if (a_handle == 0)
	throw std::string("Can't open :") + acp_filename + ": " + lt_dlerror();

      mp_handle = a_handle;
      OpenedHandles.push_back(a_handle);

    } // end of DynamicLibrary::DynamicLibrary(...)

    /** destructor */
    DynamicLibrary::~DynamicLibrary() {
      lt_dlhandle a_handle = (lt_dlhandle) mp_handle;
      if (a_handle) {
	lt_dlclose(a_handle);

	for (std::vector<lt_dlhandle>::iterator I = OpenedHandles.begin(),
	       E = OpenedHandles.end(); I != E; ++I) {
	  if (*I == a_handle) {
	    // Note: don't use the swap/pop_back trick here. Order is important.
	    OpenedHandles.erase(I);
	  }
	}
      }
    } // end of DynamicLibrary::~DynamicLibrary()

  } // end of namespace impl

} // end of namespace efscape
