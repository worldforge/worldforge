
SUBDIRS = . Message Codecs Filters Net Utilities Objects Funky

lib_LTLIBRARIES = libAtlas.la

libAtlas_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlas_la_SOURCES = \
	Bridge.cpp Codec.cpp Filter.cpp

libAtlasincludedir = $(includedir)/Atlas

libAtlasinclude_HEADERS = \
	Bridge.h Codec.h Filter.h Negotiate.h EncoderBase.h \
	Task.h Codec_impl.h Exception.h Atlas.h
