EXTRA_DIST = atlas_0_4_test.cpp atlas_0_6_test.cpp

# have to include both here to get it to build, but we're including
# distinct headers so we're okay
CXXFLAGS = -O2 -Wall @ATLAS_0_4_CFLAGS@ @ATLAS_0_6_CFLAGS@

if HAVE_ATLAS_0_4

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/wfmath

ATLAS_0_4_TESTS = atlas_0_4_test

atlas_0_4_test_SOURCES = atlas_0_4_test.cpp
atlas_0_4_test_LDADD = ../wfmath/libwfmath-0.2.la @ATLAS_0_4_LIBS@

else
ATLAS_0_4_TESTS =
endif

if HAVE_ATLAS_0_6

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/wfmath

ATLAS_0_6_TESTS = atlas_0_6_test

atlas_0_6_test_SOURCES = atlas_0_6_test.cpp
atlas_0_6_test_LDADD = ../wfmath/libwfmath-0.2.la @ATLAS_0_6_LIBS@

else
ATLAS_0_6_TESTS =
endif

check_PROGRAMS = $(ATLAS_0_4_TESTS) $(ATLAS_0_6_TESTS)
TESTS = $(check_PROGRAMS)
