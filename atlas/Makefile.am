AM_CPPFLAGS = -O2 -Wall -I.. -I../wfmath @ATLAS_CFLAGS@

if HAVE_ATLAS

headerdir = $(includedir)/wfmath
header_HEADERS = atlasconv.h

lib_LTLIBRARIES = libwfmathatlas.la
libwfmathatlas_la_SOURCES = atlasconv.cpp

check_PROGRAMS = atlas_test
TESTS = $(check_PROGRAMS)

atlas_test_SOURCES = atlas_test.cpp
atlas_test_LDADD = libwfmathatlas.la ../wfmath/libwfmath.la @ATLAS_LIBS@

endif
