AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)

EXTRA_DIST = bc_sloppy_mag_3

RECHECK_LOGS =

headerdir = $(includedir)/wfmath-1.0/wfmath
header_HEADERS = error.h const.h int_to_string.h \
	probability.h timestamp.h \
	MersenneTwister.h randgen.h shuffle.h \
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
	line.h line_funcs.h \
	intersect_decls.h intersect.h \
	wfmath.h stream.h atlasconv.h zero.h

lib_LTLIBRARIES = libwfmath-1.0.la
libwfmath_1_0_la_SOURCES = basis.h const.cpp stream.cpp int_to_string.cpp \
	probability.cpp timestamp.cpp randgen.cpp rotmatrix.cpp vector.cpp \
	point.cpp quaternion.cpp polygon.cpp intersect.cpp \
	axisbox.cpp polygon_intersect.cpp ball.cpp rotbox.cpp segment.cpp \
	line.cpp
libwfmath_1_0_la_LDFLAGS = -lm -version-info @WFMATH_VERSION_INFO@

check_PROGRAMS = vector_test rotmatrix_test point_test \
	quaternion_test shape_test polygon_test probability_test \
	const_test intstring_test \
	time_test ball_test line_test randgen_test
TESTS = $(check_PROGRAMS)

vector_test_SOURCES = vector_test.cpp general_test.h
vector_test_LDFLAGS = -lm
vector_test_LDADD = libwfmath-1.0.la

rotmatrix_test_SOURCES = rotmatrix_test.cpp general_test.h
rotmatrix_test_LDFLAGS = -lm
rotmatrix_test_LDADD = libwfmath-1.0.la

point_test_SOURCES = point_test.cpp general_test.h shape_test.h
point_test_LDFLAGS = -lm
point_test_LDADD = libwfmath-1.0.la

quaternion_test_SOURCES = quaternion_test.cpp general_test.h
quaternion_test_LDFLAGS = -lm
quaternion_test_LDADD = libwfmath-1.0.la

shape_test_SOURCES = shape_test.cpp general_test.h shape_test.h
shape_test_LDFLAGS = -lm
shape_test_LDADD = libwfmath-1.0.la

polygon_test_SOURCES = polygon_test.cpp general_test.h shape_test.h
polygon_test_LDFLAGS = -lm
polygon_test_LDADD = libwfmath-1.0.la

probability_test_SOURCES = probability_test.cpp
probability_test_LDFLAGS = -lm
probability_test_LDADD = libwfmath-1.0.la

const_test_SOURCES = const_test.cpp
const_test_LDFLAGS = -lm
const_test_LDADD = libwfmath-1.0.la

intstring_test_SOURCES = intstring_test.cpp
intstring_test_LDFLAGS = -lm
intstring_test_LDADD = libwfmath-1.0.la

time_test_SOURCES = timestamp_test.cpp
time_test_LDFLAGS = -lm
time_test_LDADD = libwfmath-1.0.la

ball_test_SOURCES = ball_test.cpp
ball_test_LDFLAGS = -lm
ball_test_LDADD = libwfmath-1.0.la

line_test_SOURCES = line_test.cpp
line_test_LDFLAGS = -lm
line_test_LDADD = libwfmath-1.0.la

randgen_test_SOURCES = randgen_test.cpp
randgen_test_LDFLAGS = -lm
randgen_test_LDADD = libwfmath-1.0.la
