
AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlasCodecs-0.7.la

libAtlasCodecs_0_7_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasCodecs_0_7_la_LIBADD = $(top_builddir)/Atlas/libAtlas-0.7.la

libAtlasCodecs_0_7_la_SOURCES = \
	Utility.cpp XML.cpp Packed.cpp Bach.cpp

libAtlasCodecsincludedir = $(includedir)/Atlas-C++-0.7/Atlas/Codecs

libAtlasCodecsinclude_HEADERS = Utility.h XML.h Packed.h Bach.h
