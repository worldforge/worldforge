## Process this file with automake to generate Makefile.in

INCLUDES = -I$(top_srcdir)

if VARCONF_SIGC_SUFFIX
lib_LTLIBRARIES = libvarconf-sigc1.2.la
else
lib_LTLIBRARIES = libvarconf.la
endif

libvarconf_sigc1_2_la_LDFLAGS = @VARCONF_VERSION_INFO@ $(SIGC_LIBS)

libvarconf_sigc1_2_la_SOURCES = \
    config.cpp parse_error.cpp variable.cpp \
    dynbase.cpp dyntypes.cpp dyncmp.cpp

libvarconf_la_LDFLAGS = @VARCONF_VERSION_INFO@ $(SIGC_LIBS)

libvarconf_la_SOURCES = \
    config.cpp parse_error.cpp variable.cpp \
    dynbase.cpp dyntypes.cpp dyncmp.cpp

libvarconfincludedir = $(includedir)/varconf-0.5/varconf
libvarconfinclude_HEADERS = \
    Config.h parse_error.h varconf.h variable.h \
    dynbase.h dyntypes.h dyncmp.h dynvar.h
