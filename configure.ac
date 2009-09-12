dnl Initialise autoconf

AC_INIT([Atlas-C++],[0.6.1],[alriddoch@googlemail.com],[Atlas-C++])
AC_CONFIG_SRCDIR([Atlas/Bridge.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Initialise automake

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 check-news])

dnl Initialise automake

ATLAS_CURRENT=0
INTERFACE_VERSION="-version-info $ATLAS_CURRENT:0:0"

AC_SUBST(ATLAS_CURRENT)
AC_SUBST(INTERFACE_VERSION)

dnl Test for GNU libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ compiler

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -Werror -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -Werror -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -Werror -DNDEBUG"
    ]
)

AC_LANG(C++)

dnl Test for <cassert> header file

AC_CHECK_HEADERS(cassert)

dnl Test for <cstdio> header file

AC_CHECK_HEADERS(cstdio)

dnl Test for <iostream> header file

AC_CHECK_HEADERS(iostream)

dnl Test for <list> header file

AC_CHECK_HEADERS(list)

dnl Test for <map> header file

AC_CHECK_HEADERS(map)

dnl Test for <cstdlib> header file

AC_CHECK_HEADERS(string)

dnl Test for <vector> header file

AC_CHECK_HEADERS(vector)

OTHER_LIBS=

dnl Test for gzip compression library

AC_ARG_WITH(zlib,
[  --with-zlib=DIR         gzip compression library (optional)],
[with_zlib="$withval"],
[with_zlib=""])

ac_save_CFLAGS="$CFLAGS"
ac_save_LIBS="$LIBS"
CFLAGS="$CFLAGS -I$with_zlib/include"
LIBS="$LIBS -L$with_zlib/lib"

AC_CHECK_LIB(z, inflate,
    [
        AC_DEFINE(HAVE_LIBZ, 1, ["True if zlib is available"])
        OTHER_LIBS="$OTHER_LIBS -lz"
    ])

AC_CHECK_HEADERS(zlib.h)

CFLAGS="$ac_save_CFLAGS"
LIBS="$ac_save_LIBS"

dnl Test for bzip2 compression library

AC_ARG_WITH(bzlib,
[  --with-bzlib=DIR        bzip2 compression library (optional)],
[with_bzlib="$withval"],
[with_bzlib=""])

ac_save_CFLAGS="$CFLAGS"
ac_save_LIBS="$LIBS"
CFLAGS="$CFLAGS -I$with_bzlib/include"
LIBS="$LIBS -L$with_bzlib/lib"

AC_CHECK_LIB(bz2, BZ2_bzDecompressInit,
    [
        AC_DEFINE(HAVE_LIBBZ2, 1, ["True if libbz2 is available"])
        OTHER_LIBS="$OTHER_LIBS -lbz2"
    ])

AC_CHECK_HEADERS(bzlib.h)

CFLAGS="$ac_save_CFLAGS"
LIBS="$ac_save_LIBS"

SYSTEM_LIBS="$SYSTEM_LIBS -lm"
ATLAS_CFLAGS=""
ATLAS_LIB_SUFFIX="-0.6"
ATLAS_LIBS="-lAtlasObjects$ATLAS_LIB_SUFFIX -lAtlasNet$ATLAS_LIB_SUFFIX -lAtlas$ATLAS_LIB_SUFFIX -lAtlasCodecs$ATLAS_LIB_SUFFIX -lAtlasMessage$ATLAS_LIB_SUFFIX -lAtlas$ATLAS_LIB_SUFFIX"

AC_SUBST(ATLAS_CFLAGS)
AC_SUBST(ATLAS_LIB_SUFFIX)
AC_SUBST(ATLAS_LIBS)
AC_SUBST(SYSTEM_LIBS)
AC_SUBST(OTHER_LIBS)

dnl Generate files

AC_CONFIG_FILES([
	Makefile
	Atlas/Makefile
	Atlas/Codecs/Makefile
	Atlas/Filters/Makefile
	Atlas/Funky/Makefile
	Atlas/Message/Makefile
	Atlas/Net/Makefile
	Atlas/Objects/Makefile
	tests/Makefile
	tests/Objects/Makefile
	tools/Makefile
	benchmark/Makefile
	tutorial/Makefile
	atlas-cpp.spec
	atlascpp-0.6.pc
])
AC_OUTPUT
