
AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlasMessage-0.6.la

libAtlasMessage_0_6_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasMessage_0_6_la_LIBADD = $(top_builddir)/Atlas/libAtlas-0.6.la

libAtlasMessage_0_6_la_SOURCES = \
	Element.cpp MEncoder.cpp DecoderBase.cpp QueuedDecoder.cpp

libAtlasMessageincludedir = $(includedir)/Atlas-C++-0.6/Atlas/Message

libAtlasMessageinclude_HEADERS = \
	Element.h MEncoder.h DecoderBase.h QueuedDecoder.h
