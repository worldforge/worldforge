## Process this file with automake to generate Makefile.in

EXTRA_DIST = conf.cfg 

INCLUDES = -I$(top_srcdir)
LDADD = $(top_builddir)/varconf/libvarconf.la -lsigc

noinst_PROGRAMS = conftest
conftest_SOURCES = conftest.cpp

