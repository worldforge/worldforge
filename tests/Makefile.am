TESTS = mercatortest testTerrain testShader testTerrainMod

INCLUDES = -I$(top_srcdir)

check_PROGRAMS = mercatortest testTerrain testShader testTerrainMod

mercatortest_SOURCES = main.cpp
mercatortest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTerrain_SOURCES = testTerrain.cpp
testTerrain_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testShader_SOURCES = testShader.cpp
testShader_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTerrainMod_SOURCES = testTerrainMod.cpp
testTerrainMod_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2
