AM_CPPFLAGS = -O2 -Wall -I..
EXTRA_DIST = bc_sloppy_mag_3

headerdir = $(includedir)/wfmath
header_HEADERS = error.h const.h string_funcs.h stream_funcs.h wfmath.h \
	vector.h vector_funcs.h matrix.h matrix_funcs.h \
	point.h point_funcs.h

lib_LTLIBRARIES = libwfmath.la
libwfmath_la_SOURCES = matrix.cpp vector.cpp const.cpp string_funcs.cpp
libwfmath_la_LDFLAGS = -lm

check_PROGRAMS = vector_test matrix_test point_test
TESTS = $(check_PROGRAMS)

vector_test_SOURCES = vector_test.cpp vector.cpp const.cpp string_funcs.cpp
vector_test_LDFLAGS = -lm

matrix_test_SOURCES = matrix_test.cpp matrix.cpp vector.cpp const.cpp string_funcs.cpp
matrix_test_LDFLAGS = -lm

point_test_SOURCES = point_test.cpp matrix.cpp vector.cpp const.cpp string_funcs.cpp
point_test_LDFLAGS = -lm
