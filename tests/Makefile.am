TESTS = mercatortest testTerrain

INCLUDES = -I$(top_srcdir)

check_PROGRAMS = mercatortest testTerrain

mercatortest_SOURCES = main.cpp
mercatortest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTerrain_SOURCES = testTerrain.cpp
testTerrain_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2
