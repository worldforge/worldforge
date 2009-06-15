TESTS = mercatortest testTerrain testShader testTerrainMod testIntersect \
        testPhys timeSeg testPlant testForest testQRNG testArea \
        testTileShader testShaderFactory testWFMath testBuffer \
        testSegment testAreaShader

INCLUDES = -I$(top_srcdir)

DISTCLEANFILES = test1.pgm test2.pgm

check_PROGRAMS = $(TESTS)

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

testPlant_SOURCES = testPlant.cpp
testPlant_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testForest_SOURCES = testForest.cpp
testForest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testQRNG_SOURCES = testQRNG.cpp

testArea_SOURCES = testArea.cpp
testArea_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testTileShader_SOURCES = testTileShader.cpp
testTileShader_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testShaderFactory_SOURCES = testShaderFactory.cpp
testShaderFactory_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testWFMath_SOURCES = testWFMath.cpp

testBuffer_SOURCES = testBuffer.cpp
testBuffer_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testSegment_SOURCES = testSegment.cpp
testSegment_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testAreaShader_SOURCES = testAreaShader.cpp
testAreaShader_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2
