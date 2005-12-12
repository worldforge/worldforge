dnl Process this file with autoconf to generate configure

dnl Autoconf Initialization
AC_INIT(varconf, 0.6.3, alriddoch@zepler.org)
AC_CONFIG_SRCDIR([varconf/variable.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Information

AM_INIT_AUTOMAKE([dist-bzip2 check-news])

dnl versioning info for libtool
dnl DO NOT CHANGE VARCONF_CURRENT HERE. See the checks for sigc++
dnl further down. If we link against sigc++ 1.2, then this should be
dnl N, if we link against sigc++ 2.0 it should be N+1. If you change
dnl this because of API changes, it needs to be incremented by
dnl 2, and the changes made to the values assigned in the sigc++ check.
VARCONF_CURRENT=6
VARCONF_REVISION=0
VARCONF_AGE=0
VARCONF_REQUIRES=""
VARCONF_CONFLICTS=""
VARCONF_LIB_SUFFIX="-1.0"

MIN_SIGC_VERSION=1.2.0

dnl General Initialization

dnl Test for GNU libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ Language

dnl Test for C++ Compiler
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

AC_LANG(C++)

dnl Test for libsigc++
PKG_CHECK_MODULES(SIGC, sigc++-2.0 >= $MIN_SIGC_VERSION,
    [
        CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
        LDFLAGS="$LDFLAGS $SIGC_LIBS"
        VARCONF_REQUIRES="$VARCONF_REQUIRES sigc++-2.0"
        VARCONF_CONFLICTS="$VARCONF_CONFLICTS sigc++-1.2 < 2.0"
        VARCONF_CURRENT=7
    ],
    [
        PKG_CHECK_MODULES(SIGC, sigc++-1.2 >= $MIN_SIGC_VERSION,
            [
                CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
                LDFLAGS="$LDFLAGS $SIGC_LIBS"
                VARCONF_REQUIRES="$VARCONF_REQUIRES sigc++-1.2"
                VARCONF_CONFLICTS="$VARCONF_CONFLICTS sigc++-2.0 > 1.9"
                VARCONF_CURRENT=6
            ],
            [
                AC_MSG_ERROR([Couldn't find sigc++ library!])
            ])
    ])

VARCONF_LIBS="-lvarconf$VARCONF_LIB_SUFFIX"
VARCONF_VERSION_INFO=$VARCONF_CURRENT:$VARCONF_REVISION:$VARCONF_AGE

AC_SUBST(VARCONF_LIBS)
AC_SUBST(VARCONF_REQUIRES)
AC_SUBST(VARCONF_CONFLICTS)
AC_SUBST(VARCONF_VERSION_INFO)
AC_SUBST(VARCONF_LIB_SUFFIX)
AC_SUBST(MIN_SIGC_VERSION)

dnl Generate files                         
AC_CONFIG_FILES([
	Makefile
	varconf.spec
	doc/Makefile
	doc/old/Makefile
	tests/Makefile
	varconf/Makefile
	varconf-1.0.pc
])
AC_OUTPUT
