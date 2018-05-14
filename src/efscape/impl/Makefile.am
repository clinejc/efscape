## source directory for the DEVS simulator implementation classes

hh_sources = adevs_config.hpp
hh_sources += adevs_json.hpp
hh_sources += adevs_decorator.h
hh_sources += adevs_decorator_serialization.hpp
hh_sources += efscape_cereal.hpp
hh_sources += ClockI.hpp
hh_sources += ModelHomeI.hpp
hh_sources += ModelHomeSingleton.hpp
hh_sources += ModelType.hpp
hh_sources += ModelType.ipp
hh_sources += BuildSim.hpp
hh_sources += RunSim.hpp
hh_sources += RepastModelWrapper.hpp
hh_sources += RepastModelWrapper.ipp
hh_sources += SimRunner.hpp

cc_sources = adevs_config.cpp
cc_sources += adevs_json.cpp
cc_sources += ClockI.cpp
cc_sources += efscape_cereal.cpp
cc_sources += efscape_serialization.cpp
cc_sources += ModelHomeI.cpp
cc_sources += ModelHomeSingleton.cpp
cc_sources += ModelType.cpp
cc_sources += BuildSim.cpp
cc_sources += RunSim.cpp
cc_sources += SimRunner.cpp
cc_sources += export.cpp

# library
lib_LTLIBRARIES = libefscape-impl.la

AM_CPPFLAGS = -DICE_CPP11_MAPPING # needed for ZeroC Ice c++ mappings
AM_CPPFLAGS += -I$(top_srcdir)/src
AM_CPPFLAGS += $(DEPS_CFLAGS)
AM_CPPFLAGS += $(BOOST_CPPFLAGS)

libefscape_impl_la_LDFLAGS = $(BOOST_SERIALIZATION_LDFLAGS)

libefscape_impl_la_LIBADD = $(top_srcdir)/src/efscape/utils/libefscape-utils.la
libefscape_impl_la_LIBADD += $(BOOST_SERIALIZATION_LIBS)
libefscape_impl_la_LIBADD += $(BOOST_MPI_LIBS)
libefscape_impl_la_LIBADD += $(BOOST_FILESYSTEM_LIBS)
libefscape_impl_la_LIBADD += $(BOOST_SYSTEM_LIBS)
libefscape_impl_la_LIBADD += $(BOOST_PROGRAM_OPTIONS_LIBS)
libefscape_impl_la_LIBADD += $(BOOST_DATE_TIME_LIBS)
libefscape_impl_la_LIBADD += $(NETCDF_CXX_LIBS)
libefscape_impl_la_LIBADD += $(DEPS_LIBS)
libefscape_impl_la_LIBADD += -lltdl

libefscape_impl_la_SOURCES=  $(hh_sources) $(cc_sources)

library_includedir = $(includedir)/efscape/impl
library_include_HEADERS = $(hh_sources) driver.cpp

# programs
bin_PROGRAMS = efdriver efbuilder

# efdriver: program for running a model
efdriver_SOURCES = driver.cpp

efdriver_LDFLAGS = $(BOOST_MPI_LDFLAGS)

#efdriver_LDADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
efdriver_LDADD = libefscape-impl.la
efdriver_LDADD += $(BOOST_MPI_LIBS)
efdriver_LDADD += $(DEPS_LIBS)

# efbuilder: program for running a model
efbuilder_SOURCES = driver.cpp

efbuilder_LDFLAGS = $(BOOST_MPI_LDFLAGS)

efbuilder_LDADD = libefscape-impl.la
efbuilder_LDADD += $(BOOST_MPI_LIBS)
efbuilder_LDADD += $(DEPS_LIBS)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="driver.cpp"
	rm -f Copyright.doc Makefile.cr Sed.cr
