## Source directory for efscape/gis

SUBDIRS = . TestCode

hh_sources = Index.hh
hh_sources += Range.hh
hh_sources += CellIndex.hh
hh_sources += Geogrid.hh
hh_sources += Geogrid_impl.hh
hh_sources += Geogrid_impl.ipp
hh_sources += GeoDataSource.hpp
hh_sources += GDALWrapper.hh
hh_sources += GeoNetCDF.hh
hh_sources += GeoNetCDF.ipp
hh_sources += gdal_utils.hpp
hh_sources += gdal_utils.ipp
hh_sources += LandscapeI.hpp

cc_sources = Index.cc
cc_sources += CellIndex.cc
cc_sources += Range.cc
cc_sources += Geogrid.cc
cc_sources += Geogrid_impl.cc
cc_sources += GeoDataSource.cpp
cc_sources += GDALWrapper.cc
cc_sources += GeoNetCDF.cc
cc_sources += gdal_utils.cpp
cc_sources += LandscapeI.cpp

library_includedir = $(includedir)/efscape/gis
library_include_HEADERS = $(hh_sources)

lib_LTLIBRARIES = libefscape-gis.la

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/efscape/utils
AM_CPPFLAGS += $(GDAL_CFLAGS)
AM_CPPFLAGS += $(GEOS_CFLAGS)
AM_CPPFLAGS += $(NETCDF_CXX_CFLAGS)

LIBS = $(GDAL_LIBS)
LIBS += $(GEOS_CCLIBS)
LIBS += $(NETCDF_CXX_LIBS)
LIBS += $(BOOST_DATE_TIME_LIB)
LIBS += $(BOOST_FILESYSTEM_LIB)
LIBS += $(BOOST_SYSTEM_LIB)

libefscape_gis_la_SOURCES= $(hh_sources) $(cc_sources)
#libefscape_gis_la_LDFLAGS= -release $(GENERIC_RELEASE)
libefscape_gis_la_LIBADD = $(GDAL_LIBS)
libefscape_gis_la_LIBADD += $(GEOS_CCLIBS)
libefscape_gis_la_LIBADD += $(NETCDF_CXX_LIBS)
libefscape_gis_la_LIBADD += $(BOOST_DATE_TIME_LIB)
libefscape_gis_la_LIBADD += $(BOOST_FILESYSTEM_LIB)
libefscape_gis_la_LIBADD += $(BOOST_SYSTEM_LIB)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	rm -f Copyright.doc Makefile.cr Sed.cr
