lib_LTLIBRARIES = libmercator-0.2.la

libmercator_0_2_la_LDFLAGS = -version-info @MERCATOR_VERSION_INFO@

libmercator_0_2_la_SOURCES = \
	Mercator.cpp \
	Matrix.cpp \
	Terrain.cpp \
	Segment.cpp

libmercatordir = $(includedir)/Mercator-0.2/Mercator

libmercator_HEADERS = \
	Mercator.h \
	Matrix.h \
	Terrain.h \
	TerrainMod.h \
	BasePoint.h \
	Segment.h
	
bin_PROGRAMS = mercatortest

mercatortest_SOURCES = \
		 main.cpp

mercatortest_LDFLAGS = -lmercator-0.2
