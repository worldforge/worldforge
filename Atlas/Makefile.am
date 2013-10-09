SUBDIRS = . Message Codecs Filters Net Objects Funky

AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlas-0.6.la

libAtlas_0_6_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlas_0_6_la_SOURCES = \
	Bridge.cpp Codec.cpp Negotiate.cpp EncoderBase.cpp Exception.cpp \
	Filter.cpp Formatter.cpp

libAtlasincludedir = $(includedir)/Atlas-C++-0.6/Atlas

libAtlasinclude_HEADERS = \
	Bridge.h Codec.h Filter.h Negotiate.h EncoderBase.h \
	Task.h Codec_impl.h Exception.h Debug.h Version.h Formatter.h
