AM_CPPFLAGS = -O2 -Wall -I..
EXTRA_DIST = bc_sloppy_mag_3

headerdir = $(includedir)/wfmath
header_HEADERS = vector.h vector_funcs.h matrix.h matrix_funcs.h \
	error.h const.h stream_funcs.h wfmath.h

lib_LTLIBRARIES = libwfmath.la
libwfmath_la_SOURCES = matrix.cpp vector.cpp
libwfmath_la_LDFLAGS = -lm

check_PROGRAMS = vector_test matrix_test
TESTS = $(check_PROGRAMS)

vector_test_SOURCES = vector_test.cpp vector.cpp
vector_test_LDFLAGS = -lm

matrix_test_SOURCES = matrix_test.cpp matrix.cpp vector.cpp
matrix_test_LDFLAGS = -lm
