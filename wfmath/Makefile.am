AM_CPPFLAGS = -O2 -Wall -I..
EXTRA_DIST = bc_sloppy_mag_3

headerdir = $(includedir)/wfmath
header_HEADERS = error.h const.h stringconv.h \
	vector.h vector_funcs.h \
	matrix.h matrix_funcs.h \
	point.h point_funcs.h \
	intersect_decls.h intersect.h \
	axisbox.h axisbox_funcs.h \
	ball.h ball_funcs.h \
	segment.h segment_funcs.h \
	rotbox.h rotbox_funcs.h \
	wfmath.h stream.h

lib_LTLIBRARIES = libwfmath.la
libwfmath_la_SOURCES = basis.h const.cpp stringconv.cpp \
	matrix.cpp vector.cpp point.cpp intersect.cpp
libwfmath_la_LDFLAGS = -lm

check_PROGRAMS = vector_test matrix_test point_test shape_test
TESTS = $(check_PROGRAMS)

vector_test_SOURCES = vector_test.cpp
vector_test_LDFLAGS = -lm
vector_test_LDADD = libwfmath.la

matrix_test_SOURCES = matrix_test.cpp
matrix_test_LDFLAGS = -lm
matrix_test_LDADD = libwfmath.la

point_test_SOURCES = point_test.cpp
point_test_LDFLAGS = -lm
point_test_LDADD = libwfmath.la

shape_test_SOURCES = shape_test.cpp
shape_test_LDFLAGS = -lm
shape_test_LDADD = libwfmath.la
