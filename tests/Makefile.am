TESTS = mercatortest testTerrain testShader testTerrainMod testIntersect testPhys timeSeg

INCLUDES = -I$(top_srcdir)

check_PROGRAMS = mercatortest testTerrain testShader testTerrainMod testIntersect testPhys timeSeg

mercatortest_SOURCES = main.cpp
mercatortest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTerrain_SOURCES = testTerrain.cpp
testTerrain_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testShader_SOURCES = testShader.cpp
testShader_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTerrainMod_SOURCES = testTerrainMod.cpp
testTerrainMod_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testIntersect_SOURCES = testIntersect.cpp
testIntersect_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testPhys_SOURCES = testPhys.cpp
testPhys_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

timeSeg_SOURCES = timeSeg.cpp util_timer.h util_timer.cpp
timeSeg_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2
