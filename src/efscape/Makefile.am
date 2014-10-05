## Source directory for efscape

SUBDIRS = utils gis impl . server

idl_sources = efscape.ice

hh_idl_sources = efscape.h

cc_idl_sources = efscape.cpp

impl_sources = efscapeI.h efscapeI.cpp

library_includedir = $(includedir)/$(PACKAGE)
library_include_HEADERS = $(idl_sources) $(hh_idl_sources) $(hh_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/$(PACKAGE)/utils
AM_CPPFLAGS += $(DEPS_CFLAGS)

noinst_LTLIBRARIES = libefscape-idl.la
libefscape_idl_la_SOURCES= $(hh_idl_sources) $(cc_idl_sources)

%.h %.cpp: %.ice
	slice2cpp -I$(top_srcdir)/src --impl --include-dir efscape $<

clean-source:
	rm -f $(impl_sources)

clean-generic:
	make clean-source
	rm -f $(hh_idl_sources) $(cc_idl_sources)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${idl_sources}"
	rm -f Copyright.doc Makefile.cr Sed.cr

