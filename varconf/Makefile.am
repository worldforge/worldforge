## Process this file with automake to generate Makefile.in

INCLUDES = -I$(top_builddir)

lib_LTLIBRARIES = libvarconf.la
libvarconf_la_LDFLAGS = \
    -version-info @VARCONF_CURRENT@:@VARCONF_REVISION@:@VARCONF_AGE@
libvarconf_la_SOURCES = \
    config.cpp parse_error.cpp variable.cpp

libvarconfincludedir = $(includedir)/varconf
libvarconfinclude_HEADERS = \
    Config.h parse_error.h varconf.h variable.h 
