TESTS = mercatortest Terraintest Shadertest TerrainModtest Intersecttest \
        testPhys timeSeg Planttest Foresttest testQRNG Areatest \
        TileShadertest ShaderFactorytest testWFMath Buffertest \
        Segmenttest AreaShadertest BasePointtest DepthShadertest \
        FillShadertest GrassShadertest ThresholdShadertest Matrixtest

INCLUDES = -I$(top_srcdir)

DISTCLEANFILES = test1.pgm test2.pgm

check_PROGRAMS = $(TESTS)

mercatortest_SOURCES = main.cpp
mercatortest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Terraintest_SOURCES = Terraintest.cpp
Terraintest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Shadertest_SOURCES = Shadertest.cpp
Shadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

TerrainModtest_SOURCES = TerrainModtest.cpp
TerrainModtest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Intersecttest_SOURCES = Intersecttest.cpp
Intersecttest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testPhys_SOURCES = testPhys.cpp
testPhys_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

timeSeg_SOURCES = timeSeg.cpp util_timer.h util_timer.cpp
timeSeg_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Planttest_SOURCES = Planttest.cpp
Planttest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Foresttest_SOURCES = Foresttest.cpp
Foresttest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testQRNG_SOURCES = testQRNG.cpp

Areatest_SOURCES = Areatest.cpp
Areatest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

TileShadertest_SOURCES = TileShadertest.cpp
TileShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

ShaderFactorytest_SOURCES = ShaderFactorytest.cpp
ShaderFactorytest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

testWFMath_SOURCES = testWFMath.cpp

Buffertest_SOURCES = Buffertest.cpp
Buffertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Segmenttest_SOURCES = Segmenttest.cpp
Segmenttest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

AreaShadertest_SOURCES = AreaShadertest.cpp
AreaShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

BasePointtest_SOURCES = BasePointtest.cpp
BasePointtest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

DepthShadertest_SOURCES = DepthShadertest.cpp
DepthShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

FillShadertest_SOURCES = FillShadertest.cpp
FillShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

GrassShadertest_SOURCES = GrassShadertest.cpp
GrassShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

ThresholdShadertest_SOURCES = ThresholdShadertest.cpp
ThresholdShadertest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2

Matrixtest_SOURCES = Matrixtest.cpp
Matrixtest_LDADD = -L$(top_builddir)/Mercator -lmercator-0.2
