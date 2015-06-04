EXTRA_DIST = atlas_0_7_test.cpp

RECHECK_LOGS =

# have to include both here to get it to build, but we're including
# distinct headers so we're okay
AM_CXXFLAGS = -O2 -Wall @ATLAS_0_7_CFLAGS@

if HAVE_ATLAS_0_7

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wfmath

ATLAS_0_7_TESTS = atlas_0_7_test

atlas_0_7_test_SOURCES = atlas_0_7_test.cpp
atlas_0_7_test_LDADD = ../wfmath/libwfmath-1.0.la @ATLAS_0_7_LIBS@

else
ATLAS_0_7_TESTS =
endif

check_PROGRAMS = $(ATLAS_0_7_TESTS)
TESTS = $(check_PROGRAMS)
