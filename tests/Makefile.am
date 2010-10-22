TESTS = mercatortest Terraintest Shadertest TerrainModtest Intersecttest \
        testPhys timeSeg Planttest Foresttest testQRNG Areatest \
        TileShadertest ShaderFactorytest testWFMath Buffertest \
        Segmenttest AreaShadertest BasePointtest DepthShadertest \
        FillShadertest GrassShadertest ThresholdShadertest Matrixtest \
        TerrainaddAreatest

INCLUDES = -I$(top_srcdir)

LDADD = $(top_builddir)/Mercator/libmercator-0.2.la

DISTCLEANFILES = test1.pgm test2.pgm

RECHECK_LOGS =

check_PROGRAMS = $(TESTS)

mercatortest_SOURCES = main.cpp

Terraintest_SOURCES = Terraintest.cpp

Shadertest_SOURCES = Shadertest.cpp

TerrainModtest_SOURCES = TerrainModtest.cpp

Intersecttest_SOURCES = Intersecttest.cpp

testPhys_SOURCES = testPhys.cpp

timeSeg_SOURCES = timeSeg.cpp util_timer.h util_timer.cpp

Planttest_SOURCES = Planttest.cpp
Planttest_LDADD = $(top_builddir)/Mercator/Plant.lo

Foresttest_SOURCES = Foresttest.cpp

testQRNG_SOURCES = testQRNG.cpp

Areatest_SOURCES = Areatest.cpp

TileShadertest_SOURCES = TileShadertest.cpp

ShaderFactorytest_SOURCES = ShaderFactorytest.cpp

testWFMath_SOURCES = testWFMath.cpp

Buffertest_SOURCES = Buffertest.cpp
Buffertest_LDADD = $(top_builddir)/Mercator/Buffer.lo

Segmenttest_SOURCES = Segmenttest.cpp
Segmenttest_LDADD = $(top_builddir)/Mercator/Segment.lo

AreaShadertest_SOURCES = AreaShadertest.cpp

BasePointtest_SOURCES = BasePointtest.cpp
BasePointtest_LDADD = $(top_builddir)/Mercator/BasePoint.lo

DepthShadertest_SOURCES = DepthShadertest.cpp

FillShadertest_SOURCES = FillShadertest.cpp

GrassShadertest_SOURCES = GrassShadertest.cpp

ThresholdShadertest_SOURCES = ThresholdShadertest.cpp

Matrixtest_SOURCES = Matrixtest.cpp
Matrixtest_LDADD = $(top_builddir)/Mercator/Matrix.lo

TerrainaddAreatest_SOURCES = TerrainaddAreatest.cpp
TerrainaddAreatest_LDADD = $(top_builddir)/Mercator/Terrain.lo \
                           $(top_builddir)/Mercator/Segment.lo \
                           $(top_builddir)/Mercator/Area.lo
