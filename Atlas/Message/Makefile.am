
AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlasMessage-0.7.la

libAtlasMessage_0_7_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasMessage_0_7_la_LIBADD = $(top_builddir)/Atlas/libAtlas-0.7.la

libAtlasMessage_0_7_la_SOURCES = \
	Element.cpp MEncoder.cpp DecoderBase.cpp QueuedDecoder.cpp

libAtlasMessageincludedir = $(includedir)/Atlas-C++-0.7/Atlas/Message

libAtlasMessageinclude_HEADERS = \
	Element.h MEncoder.h DecoderBase.h QueuedDecoder.h
