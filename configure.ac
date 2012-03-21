dnl Initialise autoconf

AC_INIT([mercator], [0.3.0], [alriddoch@googlemail.com])
AC_CONFIG_SRCDIR([Mercator/Mercator.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host and target environment

AC_CANONICAL_HOST

dnl Initialise automake

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 check-news color-tests parallel-tests no-define silent-rules])
AM_SILENT_RULES([yes])

dnl versioning info for libtool
MERCATOR_CURRENT=1
MERCATOR_REVISION=0
MERCATOR_AGE=0
MERCATOR_VERSION_INFO=$MERCATOR_CURRENT:$MERCATOR_REVISION:$MERCATOR_AGE

MERCATOR_LIB_SUFFIX="-0.3"
MERCATOR_LIBS="-lmercator$MERCATOR_LIB_SUFFIX"
MERCATOR_REQUIRES=""

AC_SUBST(MERCATOR_VERSION_INFO)

dnl Test for GNU libtool

LT_INIT([disable-static])

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
    ]
)

dnl Make sure we've got sensible versions of the sources (eg to get lrintf)

AC_DEFINE(_XOPEN_SOURCE, 500, [This is XOpen source])
AC_DEFINE(_ISOC99_SOURCE,1, [This is C99 source])

AC_LANG(C++)

dnl Check for float to int converters

AC_CHECK_FUNCS([lrintf rintf rint])
AC_CHECK_FUNCS([fabsf powf floorf ceilf])

PKG_PROG_PKG_CONFIG

dnl Check for wfmath library
PKG_CHECK_MODULES(WF, wfmath-1.0 >= 1.0.0,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
        MERCATOR_REQUIRES="$MERCATOR_REQUIRES wfmath-1.0"
    ],
    [
        AC_MSG_ERROR([
Cannot find valid versions of required WorldForge maths library, wfmath:
$WF_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://www.worldforge.org/dev/eng/libraries/mercator for details of
the libraries required and where to obtain them.])
    ])

SYSTEM_LIBS=""
MERCATOR_CFLAGS=""
AC_SUBST(SYSTEM_LIBS)
AC_SUBST(MERCATOR_LIB_SUFFIX)
AC_SUBST(MERCATOR_LIBS)
AC_SUBST(MERCATOR_CFLAGS)
AC_SUBST(MERCATOR_REQUIRES)

AC_CONFIG_FILES([
	Makefile
	Mercator/Makefile
	tests/Makefile
	mercator.spec
	mingw32-mercator.spec
	mercator-0.3.pc
])
AC_OUTPUT
