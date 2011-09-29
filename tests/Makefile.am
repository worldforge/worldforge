## Process this file with automake to generate Makefile.in

check_PROGRAMS = conftest

TESTS = $(check_PROGRAMS)

EXTRA_DIST = conf.cfg 

DISTCLEANFILES = conf2.cfg

INCLUDES = -I$(top_srcdir) -DSRCDIR=\"$(srcdir)\"
LDADD = $(top_builddir)/varconf/libvarconf-1.0.la

conftest_SOURCES = conftest.cpp

