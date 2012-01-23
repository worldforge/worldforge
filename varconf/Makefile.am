## Process this file with automake to generate Makefile.in

INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES = libvarconf-1.0.la

libvarconf_1_0_la_LDFLAGS = -version-info @VARCONF_VERSION_INFO@ $(SIGC_LIBS)

libvarconf_1_0_la_SOURCES = \
    config.cpp parse_error.cpp variable.cpp \
    dynbase.cpp dyntypes.cpp dyncmp.cpp

libvarconfincludedir = $(includedir)/varconf-1.0/varconf
libvarconfinclude_HEADERS = \
    config.h parse_error.h varconf.h variable.h \
    dynbase.h dyntypes.h dyncmp.h dynvar.h varconf_defs.h
