// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Singleton.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__

#ifndef EFSCAPE_UTILS_SINGLETON_HPP
#define EFSCAPE_UTILS_SINGLETON_HPP

#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>

namespace efscape {
  namespace utils {

    /**
     * Defines a Singleton with double-checked locking pattern
     * (see
     * http://www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.singleton).
     *
     * @author Jon Cline <clinej@stanfordalumni.org>
     * @version 1.0.0 created 16 Nov 2015, revised 16 Nov 2015
     */
    template <typename T>
    class Singleton {
    public:
      static T & instance()
      {
	T * tmp = instance_.load(boost::memory_order_consume);
	if (!tmp) {
	  boost::mutex::scoped_lock guard(instantiation_mutex);
	  tmp = instance_.load(boost::memory_order_consume);
	  if (!tmp) {
	    tmp = new T;
	    instance_.store(tmp, boost::memory_order_release);
	  }
	}
	return *tmp;
      }
      
    private:
      static boost::atomic<T *> instance_;
      static boost::mutex instantiation_mutex;
    };
   
    template <typename T>
    boost::atomic<T *> Singleton<T>::instance_(0);

    template <typename T>
    boost::mutex Singleton<T>::instantiation_mutex;

  } // namespace utils
}   // namespace efscape

#endif	// #ifndef EFSCAPE_UTILS_SINGLETON_HPP
