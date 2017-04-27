## source directory for the ICE DEVS simulator implementation

hh_sources = ModelHomeTie.hpp
hh_sources += ModelTie.hpp
hh_sources += SimulatorI.hpp
hh_sources += RunServer.hpp

cc_sources = ModelHomeTie.cpp
cc_sources += SimulatorI.cpp
cc_sources += ModelTie.cpp
cc_sources += RunServer.cpp

# library
lib_LTLIBRARIES = libefscape-ice.la

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += $(BOOST_CPPFLAGS)
AM_CPPFLAGS += $(DEPS_CFLAGS)
#AM_CPPFLAGS += $(ICE_CFLAGS)
#libefscape_ice_la_LIBADD = $(ICE_LIBS)
libefscape_ice_la_LIBADD = $(DEPS_LIBS)
libefscape_ice_la_LIBADD += $(top_srcdir)/src/efscape/libefscape-idl.la
libefscape_ice_la_LIBADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
libefscape_ice_la_SOURCES=  $(hh_sources) $(cc_sources)

#library_includedir = $(includedir)/$(GENERIC_LIBRARY_NAME)/server
library_includedir = $(includedir)/efscape/server
library_include_HEADERS = $(hh_sources)

# programs
bin_PROGRAMS = efserver
bin_PROGRAMS += efclient

# efserver: ICE server for efscape simulator
efserver_SOURCES = $(top_srcdir)/src/efscape/impl/driver.cc
efserver_LDADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
efserver_LDADD += $(top_srcdir)/src/efscape/server/libefscape-ice.la
efserver_LDADD += $(BOOST_MPI_LIBS)
efserver_LDADD += $(BOOST_REGEX_LIBS)
efserver_LDADD += $(DEPS_LIBS)
efserver_LDADD += $(NETCDF_CXX_LIBS)

# efclient: ICE client for efscape simulator
efclient_SOURCES = Client.cpp
efclient_LDADD = $(top_srcdir)/src/efscape/server/libefscape-ice.la
efclient_LDADD += $(DEPS_LIBS)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="Client.cpp"
	rm -f Copyright.doc Makefile.cr Sed.cr
