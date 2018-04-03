# source directory for repast hpc v2.x relogo zombie example

libdir = $(prefix)/lib/efscape

hpp_sources = AgentPackage.h
hpp_sources += Human.h
hpp_sources += InfectionSum.h
hpp_sources += Zombie.h
hpp_sources += ZombieObserver.h

cpp_sources = Human.cpp
cpp_sources += InfectionSum.cpp
cpp_sources += Zombie.cpp
cpp_sources += ZombieObserver.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/examples
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += $(DEPS_CFLAGS)

# libgpt: shared library for gpt
lib_LTLIBRARIES = libzombie.la
libzombie_la_SOURCES = $(cpp_sources)
libzombie_la_LIBADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la

# zombie_model: executable for running the zombie model
bin_PROGRAMS = zombie_model

zombie_model_SOURCES = main.cpp
zombie_model_LDADD = libzombie.la
zombie_model_LDADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
zombie_model_LDADD += $(DEPS_LIBS)
zombie_model_LDADD += $(BOOST_MPI_LIBS)
