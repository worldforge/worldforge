if HAVE_ATLAS

AM_CXXFLAGS = -O2 -Wall @ATLAS_CFLAGS@
INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/wfmath

check_PROGRAMS = atlas_test
TESTS = $(check_PROGRAMS)

atlas_test_SOURCES = atlas_test.cpp
atlas_test_LDADD = ../wfmath/libwfmath.la @ATLAS_LIBS@

endif
