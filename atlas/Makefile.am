AM_CPPFLAGS = -O2 -Wall -I.. -I../wfmath @ATLAS_CFLAGS@

if HAVE_ATLAS

headerdir = $(includedir)/wfmath
header_HEADERS = atlas_funcs.h

lib_LTLIBRARIES = libwfmathatlas.la
libwfmathatlas_la_SOURCES = atlas_funcs.cpp

check_PROGRAMS = atlas_test
TESTS = $(check_PROGRAMS)

atlas_test_SOURCES = atlas_test.cpp atlas_funcs.cpp
atlas_test_LDADD = ../wfmath/libwfmath.la @ATLAS_LIBS@

endif
