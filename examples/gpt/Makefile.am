# source directory for gpt example

libdir = $(prefix)/lib/efscape

hpp_sources = genr.h
hpp_sources += job.h
hpp_sources += proc.h
hpp_sources += transd.h
hpp_sources += transd.h

hpp_sources += genr.hpp
hpp_sources += job.hpp
hpp_sources += proc.hpp
hpp_sources += transd.hpp
hpp_sources += gpt_config.hpp
hpp_sources += CreateGpt.hpp

cpp_sources = genr.cpp
cpp_sources += proc.cpp
cpp_sources += transd.cpp
cpp_sources += export_libgpt.cpp
cpp_sources += CreateGpt.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/examples
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += $(DEPS_CFLAGS)

# libgpt: shared library for gpt
lib_LTLIBRARIES = libgpt.la
libgpt_la_SOURCES = $(cpp_sources)
libgpt_la_LIBADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la

# runGpt: a simple command-line program for running the gpt simulation
bin_PROGRAMS = runGpt createGpt

runGpt_SOURCES = main.cpp
runGpt_LDADD = libgpt.la
runGpt_LDADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
runGpt_LDADD += $(DEPS_LIBS)
runGpt_LDADD += $(BOOST_MPI_LIBS)

createGpt_SOURCES = $(top_srcdir)/src/efscape/impl/driver.cpp
createGpt_LDADD = libgpt.la
createGpt_LDADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
createGpt_LDADD += $(DEPS_LIBS)
createGpt_LDADD += $(BOOST_MPI_LIBS)
