
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

lib_LTLIBRARIES = libAtlasFilters-0.7.la

libAtlasFilters_0_7_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasFilters_0_7_la_LIBADD = $(top_builddir)/Atlas/libAtlas-0.7.la \
	$(OTHER_LIBS)

libAtlasFilters_0_7_la_SOURCES = \
	Bzip2.cpp Bzip2.h \
	Gzip.cpp Gzip.h
