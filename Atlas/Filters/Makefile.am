
INCLUDES = -I$(top_srcdir) -I$(top_builddir)

lib_LTLIBRARIES = libAtlasFilters-0.6.la

libAtlasFilters_0_6_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasFilters_0_6_la_LIBADD = ../libAtlas-0.6.la -lz -lbz2

libAtlasFilters_0_6_la_SOURCES = \
        Bzip2.cpp Bzip2.h \
	Gzip.cpp Gzip.h
