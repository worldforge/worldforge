dnl Process this file with autoconf to generate configure

dnl Autoconf Initialization
AC_INIT([varconf], [0.6.7], [alriddoch@googlemail.com])
AC_CONFIG_SRCDIR([varconf/variable.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Information

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 check-news])

dnl versioning info for libtool
VARCONF_CURRENT=7
VARCONF_REVISION=0
VARCONF_AGE=0
VARCONF_REQUIRES=""
VARCONF_LIB_SUFFIX="-1.0"

MIN_SIGC_VERSION=2.0.0

dnl General Initialization

dnl Test for GNU libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ Language

dnl Test for C++ Compiler
AC_PROG_CXX

debug_flags="-DNDEBUG"
AC_ARG_ENABLE(debug,
    [AS_HELP_STRING([--enable-debug],
                    [enable debug information [default=no]])],
    [AS_IF([test "x$enable_debug" = xyes],
           [debug_flags="-DDEBUG"])]
)

AC_LANG(C++)

dnl Test for libsigc++
PKG_CHECK_MODULES(SIGC, sigc++-2.0 >= $MIN_SIGC_VERSION,
    [
        CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
        LIBS="$LIBS $SIGC_LIBS"
        VARCONF_REQUIRES="$VARCONF_REQUIRES sigc++-2.0"
    ],
        AC_MSG_ERROR([Couldn't find sigc++ library!])
    ])

VARCONF_LIBS="-lvarconf$VARCONF_LIB_SUFFIX"
VARCONF_VERSION_INFO=$VARCONF_CURRENT:$VARCONF_REVISION:$VARCONF_AGE

AC_SUBST(VARCONF_LIBS)
AC_SUBST(VARCONF_REQUIRES)
AC_SUBST(VARCONF_VERSION_INFO)
AC_SUBST(VARCONF_LIB_SUFFIX)
AC_SUBST(MIN_SIGC_VERSION)
AC_SUBST(AM_CXXFLAGS, "-Wall -Wextra -pedantic $debug_flags")

dnl Generate files                         
AC_CONFIG_FILES([
	Makefile
	varconf.spec
	mingw32-varconf.spec
	doc/Makefile
	doc/old/Makefile
	tests/Makefile
	varconf/Makefile
	varconf-1.0.pc
])
AC_OUTPUT
