dnl Initialise autoconf

AC_INIT(atlas_cpp_tutorial, 0.0.0)
AC_CONFIG_SRCDIR(simple_server.cpp)
AC_CONFIG_HEADERS([config.h])

dnl Initialise automake

AM_INIT_AUTOMAKE

dnl Test for C++ compiler

AC_PROG_CXX

CFLAGS='-Wall -g -I/usr/X11R6/include'
CXXFLAGS='-Wall -g -I/usr/X11R6/include'
LIBS='-L/usr/X11R6/lib -L/usr/local/lib'

AC_CHECK_FUNC(cos,,[AC_CHECK_LIB(m,cos)])

dnl Test for WorldForge Atlas Libraries

PKG_CHECK_MODULES(ATLAS, atlascpp-0.7 skstream-0.3,
    [
        CPPFLAGS="$CPPFLAGS $ATLAS_CFLAGS"
        LIBS="$COMMON_LIBS $ATLAS_LIBS"
    ],
    [
        AC_MSG_ERROR([Cannot find valid version of Atlas-C++ installed.])
    ])

AC_LANG(C++)

dnl Generate files
AC_OUTPUT([
	Makefile
])
