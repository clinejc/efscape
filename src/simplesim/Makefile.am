# source directory for the efscape::impl::TestCode programs

libdir = $(prefix)/lib/efscape

hh_sources = BasicModel.hh
hh_sources += SimpleGenerator.hh
hh_sources += SimpleObserver.hh
hh_sources += CreateSimpleSim.hh

cc_sources = BasicModel.cc
cc_sources += SimpleGenerator.cc
cc_sources += SimpleObserver.cc
cc_sources += CreateSimpleSim.cc

bin_PROGRAMS = createSimpleSim

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += -I$(top_srcdir)/src 
AM_CPPFLAGS += $(DEPS_CFLAGS)

# libefsimplesim: shared library for SimplesimModel
lib_LTLIBRARIES = libefsimplesim.la
libefsimplesim_la_SOURCES = $(cc_sources)
libefsimplesim_la_LIBADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
#libefsimplesim_la_LIBADD = $(DEPS_LIBS)

# eftestdriver: sample driver for running the simple model
createSimpleSim_SOURCES = $(top_srcdir)/src/efscape/impl/driver.cpp
createSimpleSim_LDADD = $(top_srcdir)/src/simplesim/libefsimplesim.la
createSimpleSim_LDADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
createSimpleSim_LDADD += $(DEPS_LIBS)
createSimpleSim_LDADD += $(BOOST_MPI_LIBS)
