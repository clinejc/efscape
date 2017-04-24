// __COPYRIGHT_START__
// Package Name : efscape
// File Name : xerces_utils.hpp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef XERCES_UTILS_HPP_INCLUDED
#define XERCES_UTILS_HPP_INCLUDED

#include <xercesc/util/PlatformUtils.hpp>
#include <efscape/utils/xerces_strings.hpp>

namespace efscape {
  namespace utils {

    // RAII utility that initializes the parser and frees resources
    // when it goes out of scope
    class XercesInitializer {
    public:
      XercesInitializer( ) { xercesc::XMLPlatformUtils::Initialize( ); }
      ~XercesInitializer( ) { xercesc::XMLPlatformUtils::Terminate( ); }
    private:
      // Prohibit copying and assignment
      XercesInitializer(const XercesInitializer&);
      XercesInitializer& operator=(const XercesInitializer&);
    };

    // RAII utility that releases a resource when it goes out of scope.
    template<typename T>
    class DOMPtr {
    public:
      DOMPtr(T* t) : t_(t) { }
      ~DOMPtr( ) { t_->release( ); }
      T* operator->( ) const { return t_; }
    private:
      // prohibit copying and assigning
      DOMPtr(const DOMPtr&);
      DOMPtr& operator=(const DOMPtr&);
      T* t_;
    };

    // Reports errors encountered while parsing using a DOMBuilder.
    class EfscapeErrorHandler : public xercesc::DOMErrorHandler {
    public:
      bool handleError(const xercesc::DOMError& e)
      {
        std::cout << toNative(e.getMessage( )) << "\n";
        return false;
      }
    };

  } // namespace utils
}   // namespace efscape

#endif // #ifndef XERCES_UTILS_HPP_INCLUDED
