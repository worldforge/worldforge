AM_CXXFLAGS = -O2 -Wall -DWFMATH_DEPRECATE_OLD_INTERSECT
INCLUDES = -I$(top_srcdir)
EXTRA_DIST = bc_sloppy_mag_3

headerdir = $(includedir)/wfmath
header_HEADERS = error.h const.h \
	probability.h timestamp.h randgen.h shuffle.h \
	vector.h vector_funcs.h \
	rotmatrix.h rotmatrix_funcs.h \
	point.h point_funcs.h \
	quaternion.h \
	axisbox.h axisbox_funcs.h \
	ball.h ball_funcs.h \
	wrapped_array.h miniball.h miniball_funcs.h \
	segment.h segment_funcs.h \
	rotbox.h rotbox_funcs.h \
	polygon.h polygon_funcs.h polygon_intersect.h \
	intersect_decls.h intersect.h \
	wfmath.h stream.h atlasconv.h

lib_LTLIBRARIES = libwfmath.la
libwfmath_la_SOURCES = basis.h const.cpp stream.cpp probability.cpp \
	timestamp.cpp randgen.cpp rotmatrix.cpp vector.cpp point.cpp \
	quaternion.cpp polygon.cpp intersect.cpp polygon_intersect.cpp
libwfmath_la_LDFLAGS = -lm -version-info @WFMATH_VERSION_INFO@

check_PROGRAMS = vector_test rotmatrix_test point_test \
	quaternion_test shape_test polygon_test probability_test const_test
TESTS = $(check_PROGRAMS)

vector_test_SOURCES = vector_test.cpp general_test.h
vector_test_LDFLAGS = -lm
vector_test_LDADD = libwfmath.la

rotmatrix_test_SOURCES = rotmatrix_test.cpp general_test.h
rotmatrix_test_LDFLAGS = -lm
rotmatrix_test_LDADD = libwfmath.la

point_test_SOURCES = point_test.cpp general_test.h shape_test.h
point_test_LDFLAGS = -lm
point_test_LDADD = libwfmath.la

quaternion_test_SOURCES = quaternion_test.cpp general_test.h
quaternion_test_LDFLAGS = -lm
quaternion_test_LDADD = libwfmath.la

shape_test_SOURCES = shape_test.cpp general_test.h shape_test.h
shape_test_LDFLAGS = -lm
shape_test_LDADD = libwfmath.la

polygon_test_SOURCES = polygon_test.cpp general_test.h shape_test.h
polygon_test_LDFLAGS = -lm
polygon_test_LDADD = libwfmath.la

probability_test_SOURCES = probability_test.cpp
probability_test_LDFLAGS = -lm
probability_test_LDADD = libwfmath.la

const_test_SOURCES = const_test.cpp
const_test_LDFLAGS = -lm
const_test_LDADD = libwfmath.la
