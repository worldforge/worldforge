SUBDIRS = . Objects Message Codecs Filters Net Utilities Funky

lib_LTLIBRARIES = libAtlas.la

libAtlas_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasincludedir = $(includedir)/Atlas

libAtlas_la_SOURCES = \
	Bridge.cpp Codec.cpp Filter.cpp

libAtlasinclude_HEADERS = \
	Bridge.h Codec.h Filter.h Negotiate.h EncoderBase.h \
	Task.h Codec_impl.h Exception.h Atlas.h
