if HAVE_ATLAS

AM_CPPFLAGS = -O2 -Wall -I.. -I../wfmath @ATLAS_CFLAGS@

check_PROGRAMS = atlas_test
TESTS = $(check_PROGRAMS)

atlas_test_SOURCES = atlas_test.cpp
atlas_test_LDADD = ../wfmath/libwfmath.la @ATLAS_LIBS@

endif
