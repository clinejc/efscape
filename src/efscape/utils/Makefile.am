## source directory for the efscape utils library classes

hh_sources = CommandOpt.hpp
hh_sources += DynamicLibrary.hpp
hh_sources += Factory.hpp
hh_sources += Singleton.hpp
hh_sources += type.hpp
hh_sources += boost_utils.hpp
hh_sources += boost_utils.ipp

cc_sources = CommandOpt.cpp
cc_sources += DynamicLibrary.cpp
cc_sources += type.cp
cc_sources += boost_utils.cpp

library_includedir = $(includedir)/$(PACKAGE)/utils
library_include_HEADERS = $(hh_sources)

lib_LTLIBRARIES = libefscape-utils.la 
AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += $(DEPS_CFLAGS)
AM_CPPFLAGS += $(BOOST_CPPFLAGS)
libefscape_utils_la_LIBADD =  $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
libefscape_utils_la_LIBADD +=  $(BOOST_PROGRAM_OPTIONS_LIBS)
libefscape_utils_la_LIBADD +=  $(BOOST_REGEX_LIBS)
libefscape_utils_la_SOURCES=  $(hh_sources) $(cc_sources)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	rm -f Copyright.doc Makefile.cr Sed.cr
