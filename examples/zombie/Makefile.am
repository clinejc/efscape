# source directory for repast hpc v2.x relogo zombie example

libdir = $(prefix)/lib/efscape

hpp_sources = AgentPackage.h
hpp_sources += Human.h
hpp_sources += InfectionSum.h
hpp_sources += Zombie.h
hpp_sources += ZombieObserver.h
hpp_sources += ZombieTest.hpp

cpp_sources = Human.cpp
cpp_sources += InfectionSum.cpp
cpp_sources += Zombie.cpp
cpp_sources += ZombieObserver.cpp
#cpp_sources += ZombieWrapper.cpp
cpp_sources += ZombieTest.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/examples
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += $(DEPS_CFLAGS)

# libzombie: convenience library for zombie model
noinst_LTLIBRARIES = libzombie.la
libzombie_la_SOURCES = $(cpp_sources)
libzombie_la_LIBADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
libzombie_la_LIBADD += $(DEPS_LIBS)
libzombie_la_LIBADD += $(BOOST_MPI_LIBS)

# libzombieserver: shared library for ZombieWrapper
lib_LTLIBRARIES = libzombieserver.la
libzombieserver_la_SOURCES = ZombieWrapper.cpp
libzombieserver_la_LIBADD = libzombie.la
libzombieserver_la_LIBADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la

# zombie_model: executable for running the zombie model
bin_PROGRAMS = zombie_model zombie_test

zombie_model_SOURCES = main.cpp
zombie_model_LDADD = libzombie.la
zombie_model_LDADD += $(top_srcdir)/src/efscape/impl/libefscape-impl.la
zombie_model_LDADD += $(DEPS_LIBS) -lnetcdf_c++
zombie_model_LDADD += $(BOOST_MPI_LIBS)

# zombie_test: program for testing the Zombie model
zombie_test_SOURCES = zombie_test.cpp #$(top_srcdir)/src/efscape/impl/driver.cpp
zombie_test_LDADD = $(top_srcdir)/src/efscape/impl/libefscape-impl.la
zombie_test_LDADD += $(DEPS_LIBS)
zombie_test_LDADD += $(BOOST_MPI_LIBS)