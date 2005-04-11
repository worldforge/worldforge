dnl Initialise autoconf

AC_INIT(mercator, 0.2.2)
AC_CONFIG_SRCDIR([Mercator/Mercator.cpp])

dnl Detect the canonical host and target environment

AC_CANONICAL_SYSTEM

dnl Initialise automake

AM_INIT_AUTOMAKE([dist-bzip2])
AM_CONFIG_HEADER(config.h)

dnl versioning info for libtool
MERCATOR_CURRENT=2
MERCATOR_REVISION=0
MERCATOR_AGE=0
MERCATOR_VERSION_INFO=$MERCATOR_CURRENT:$MERCATOR_REVISION:$MERCATOR_AGE

MERCATOR_LIB_SUFFIX="-0.2"
MERCATOR_LIBS="-lmercator$MERCATOR_LIB_SUFFIX"
MERCATOR_REQUIRES=""

AC_SUBST(MERCATOR_VERSION)
AC_SUBST(MERCATOR_VERSION_INFO)

dnl Test for GNU libtool

AM_DISABLE_STATIC
AM_PROG_LIBTOOL

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

dnl Make sure we've got sensible versions of the sources (eg to get snprintf)

AC_DEFINE(_XOPEN_SOURCE, 500, [This is XOpen source])
AC_DEFINE(_ISOC99_SOURCE,1, [This is C99 source])

AC_LANG_PUSH(C++)

dnl Check for float to int converters

AC_CHECK_FUNCS(lrintf rintf rint)

dnl Check for wfmath library
PKG_CHECK_MODULES(WF, wfmath-0.3 >= 0.3.2,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
        MERCATOR_REQUIRES="$MERCATOR_REQUIRES wfmath-0.3"
    ],
    [
        AC_MSG_ERROR(Couldn't find wfmath or wfmath or version is insufficent)
    ])

AC_LANG_POP(C++)

SYSTEM_LIBS=""
MERCATOR_CFLAGS=""
AC_SUBST(SYSTEM_LIBS)
AC_SUBST(MERCATOR_LIB_SUFFIX)
AC_SUBST(MERCATOR_LIBS)
AC_SUBST(MERCATOR_CFLAGS)
AC_SUBST(MERCATOR_REQUIRES)

AC_OUTPUT([
Makefile
Mercator/Makefile
tests/Makefile
mercator.spec
mercator-0.2.pc
])
