## source directory for the ICE DEVS simulator implementation

hh_sources = ModelHomeI.hpp
hh_sources += ModelI.hpp
hh_sources += SimulatorI.hpp
hh_sources += RunServer.hpp

cc_sources = ModelHomeI.cpp
cc_sources += SimulatorI.cpp
cc_sources += ModelI.cpp
cc_sources += RunServer.cpp

# cpp flags
AM_CPPFLAGS = -DICE_CPP11_MAPPING # needed for ZeroC Ice c++ mappings
AM_CPPFLAGS += -I.
AM_CPPFLAGS += -I$(top_srcdir)/src/slice
AM_CPPFLAGS += -I$(top_srcdir)/src
AM_CPPFLAGS += $(BOOST_CPPFLAGS)
AM_CPPFLAGS += $(DEPS_CFLAGS)

# library
lib_LTLIBRARIES = libefscape-ice.la

libefscape_ice_la_LIBADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
libefscape_ice_la_LIBADD += $(top_srcdir)/src/slice/efscape/libefscape-idl.la
#libefscape_ice_la_LIBADD += $(RHPC_LIBS)
libefscape_ice_la_LIBADD += $(ICE_LIBS)
libefscape_ice_la_LIBADD += $(DEPS_LIBS)

libefscape_ice_la_SOURCES=  $(hh_sources) $(cc_sources)

# programs
bin_PROGRAMS = efserver
bin_PROGRAMS += efclient

# efserver: ICE server for efscape simulator
efserver_SOURCES = $(top_srcdir)/src/efscape/impl/driver.cpp

efserver_LDADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
efserver_LDADD += libefscape-ice.la
efserver_LDADD += $(ICE_LIBS)
efserver_LDADD += $(BOOST_MPI_LIBS)
efserver_LDADD += $(BOOST_REGEX_LIBS)
efserver_LDADD += $(DEPS_LIBS)
efserver_LDADD += $(NETCDF_CXX_LIBS)

# efclient: ICE client for efscape simulator
efclient_SOURCES = Client.cpp
efclient_LDADD = $(top_srcdir)/src/slice/efscape/libefscape-idl.la
efclient_LDADD += $(ICE_LIBS)
efclient_LDADD += $(BOOST_FILESYSTEM_LIBS)
efclient_LDADD += $(BOOST_SYSTEM_LIBS)
efclient_LDADD += $(DEPS_LIBS)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="Client.cpp"
	rm -f Copyright.doc Makefile.cr Sed.cr
