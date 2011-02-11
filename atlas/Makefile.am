EXTRA_DIST = atlas_0_6_test.cpp

# have to include both here to get it to build, but we're including
# distinct headers so we're okay
AM_CXXFLAGS = -O2 -Wall @ATLAS_0_6_CFLAGS@

if HAVE_ATLAS_0_6

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wfmath

ATLAS_0_6_TESTS = atlas_0_6_test

atlas_0_6_test_SOURCES = atlas_0_6_test.cpp
atlas_0_6_test_LDADD = ../wfmath/libwfmath-0.3.la @ATLAS_0_6_LIBS@

else
ATLAS_0_6_TESTS =
endif

check_PROGRAMS = $(ATLAS_0_6_TESTS)
TESTS = $(check_PROGRAMS)
