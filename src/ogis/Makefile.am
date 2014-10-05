## Source directory for ogis

SUBDIRS= . impl

idl_sources = ogc_pt.ice
idl_sources += ogc_cs.ice
idl_sources += ogc_ct.ice
idl_sources += ogc_cv.ice
idl_sources += ogc_gc.ice
idl_sources += ogc_sf.ice

hh_sources = ogc_pt.h
hh_sources += ogc_cs.h
hh_sources += ogc_ct.h
hh_sources += ogc_cv.h
hh_sources += ogc_gc.h
hh_sources += ogc_sf.h

cc_sources = ogc_pt.cpp
cc_sources += ogc_cs.cpp
cc_sources += ogc_ct.cpp
cc_sources += ogc_cv.cpp
cc_sources += ogc_gc.cpp
cc_sources += ogc_sf.cpp

impl_sources = ogc_ptI.h ogc_ptI.cpp
impl_sources += ogc_csI.h ogc_csI.cpp
impl_sources += ogc_ctI.h ogc_ctI.cpp
impl_sources += ogc_cvI.h ogc_cvI.cpp
impl_sources += ogc_gcI.h ogc_gcI.cpp
impl_sources += ogc_sfI.h ogc_sfI.cpp

library_includedir = $(includedir)/ogis
library_include_HEADERS = $(idl_sources) $(hh_sources)

noinst_LTLIBRARIES = libogis-idl.la
AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(DEPS_CFLAGS)
libogis_idl_la_SOURCES= $(hh_sources) $(cc_sources)

%.h %.cpp: %.ice
	slice2cpp -I$(top_srcdir)/src --impl --include-dir ogis $<

clean-source:
	rm -f $(impl_sources)

clean-generic:
	make clean-source
	rm -f $(hh_sources) $(cc_sources)
