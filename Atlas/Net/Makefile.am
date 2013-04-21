
AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlasNet-0.7.la

libAtlasNet_0_7_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasNet_0_7_la_LIBADD = $(top_builddir)/Atlas/Codecs/libAtlasCodecs-0.7.la \
    $(top_builddir)/Atlas/libAtlas-0.7.la

libAtlasNetincludedir = $(includedir)/Atlas-C++-0.7/Atlas/Net

libAtlasNet_0_7_la_SOURCES = \
	Stream.cpp Loopback.cpp

libAtlasNetinclude_HEADERS = \
	Stream.h Loopback.h
