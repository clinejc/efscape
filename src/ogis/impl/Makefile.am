## Source directory for ogis/impl

hh_sources = ogis_utils.hpp
hh_sources += SampleDimensionI.hpp
hh_sources += CoverageI.hpp
hh_sources += CoverageI.ipp
hh_sources += GridCoverageI.hpp
hh_sources += GridCoverageExchangeI.hpp
#hh_sources += FeatureI.hpp
#hh_sources += FeatureTypeI.hpp
#hh_sources += FeatureCollectionI.hpp
#hh_sources += GeometryI.hpp
#hh_sources += CoordinateSystemI.hpp

cc_sources = ogis_utils.hpp
cc_sources += SampleDimensionI.cpp
cc_sources += CoverageI.cpp
cc_sources += GridCoverageI.cpp
cc_sources += GridCoverageExchangeI.cpp
#cc_sources += FeatureI.cpp
#cc_sources += FeatureTypeI.cpp
#cc_sources += FeatureCollectionI.cpp
#cc_sources += GeometryI.cpp
#cc_sources += CoordinateSystemI.cpp

library_includedir = $(includedir)/ogis/impl
library_include_HEADERS = $(hh_sources)

lib_LTLIBRARIES = libogis-ice.la

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/ogis
AM_CPPFLAGS += $(DEPS_CFLAGS) $(GDAL_CFLAGS)

libogis_ice_la_LIBADD = $(top_srcdir)/src/ogis/libogis-idl.la
libogis_ice_la_SOURCES= $(hh_sources) $(cc_sources)
#libogis_ice_la_LDFLAGS= -release $(GENERIC_RELEASE)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${cc_sources}"
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${hh_sources}"
	rm -f Copyright.doc Makefile.cr Sed.cr
