AM_CPPFLAGS = -O2 -Wall -I..
EXTRA_DIST = bc_sloppy_mag_3

headerdir = $(includedir)/wfmath
header_HEADERS = error.h const.h \
	vector.h vector_funcs.h \
	rotmatrix.h rotmatrix_funcs.h \
	point.h point_funcs.h \
	quaternion.h \
	axisbox.h axisbox_funcs.h \
	ball.h ball_funcs.h \
	miniball_config.h wrapped_array.h miniball.h miniball_funcs.h \
	segment.h segment_funcs.h \
	rotbox.h rotbox_funcs.h \
	polygon.h polygon_funcs.h \
	intersect_decls.h intersect.h \
	wfmath.h stream.h atlasconv.h

lib_LTLIBRARIES = libwfmath.la
libwfmath_la_SOURCES = basis.h const.cpp stream.cpp \
	rotmatrix.cpp vector.cpp point.cpp quaternion.cpp polygon.cpp intersect.cpp
libwfmath_la_LDFLAGS = -lm

check_PROGRAMS = vector_test rotmatrix_test point_test intersect_test
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

intersect_test_SOURCES = intersect_test.cpp
intersect_test_LDFLAGS = -lm
intersect_test_LDADD = libwfmath.la
