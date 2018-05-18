## Source directory for efscape

idl_sources = efscape.ice

hh_idl_sources = efscape.h

cc_idl_sources = efscape.cpp

impl_sources = efscapeI.h efscapeI.cpp

AM_CPPFLAGS = -DICE_CPP11_MAPPING
AM_CPPFLAGS += -I$(top_srcdir)/src/slice
AM_CPPFLAGS += $(DEPS_CFLAGS)

noinst_LTLIBRARIES = libefscape-idl.la
libefscape_idl_la_SOURCES= $(hh_idl_sources) $(cc_idl_sources)

%.h %.cpp: %.ice
	slice2cpp -I$(top_srcdir)/src/slice --impl-c++11 --include-dir efscape $<

clean-source:
	rm -f $(impl_sources)

clean-generic:
	make clean-source
	rm -f $(hh_idl_sources) $(cc_idl_sources)

copyright:
	cp $(top_srcdir)/Copyright.doc $(top_srcdir)/Makefile.cr $(top_srcdir)/Sed.cr .
	make -f Makefile.cr NAME="${PACKAGE}" FILES="${idl_sources}"
	rm -f Copyright.doc Makefile.cr Sed.cr

