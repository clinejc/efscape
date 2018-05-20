## Source directory for efscape

idl_sources = common.ice
idl_sources += Model.ice
idl_sources += Simulator.ice
idl_sources += ModelHome.ice

hh_idl_sources = common.h
hh_idl_sources += Model.h
hh_idl_sources += Simulator.h
hh_idl_sources += ModelHome.h

cc_idl_sources = common.cpp
cc_idl_sources += Model.cpp
cc_idl_sources += Simulator.cpp
cc_idl_sources += ModelHome.cpp

impl_sources = commonI.h commonI.cpp
impl_sources += ModelI.h ModelI.cpp
impl_sources += SimulatorI.h SimulatorI.cpp
impl_sources += ModelHomeI.h ModelHomeI.cpp

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

