dnl Autoconf setup

AC_INIT([wfmath], [0.3.11], [alriddoch@googlemail.com])
AC_CONFIG_SRCDIR([wfmath/const.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Automake setup

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 check-news color-tests parallel-tests])

dnl versioning info for libtool
WFMATH_CURRENT=5
WFMATH_REVISION=0
WFMATH_AGE=0
WFMATH_VERSION_INFO=$WFMATH_CURRENT:$WFMATH_REVISION:$WFMATH_AGE

AC_SUBST(WFMATH_VERSION_INFO)

dnl Checks for programs.

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
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

dnl Checks for libraries.
dnl Replace `main' with a function in -libs:
SYSTEM_LIBS="-lm"
WFMATH_LIB_SUFFIX="-0.3"
WFMATH_LIBS="-lwfmath$WFMATH_LIB_SUFFIX"

AC_MSG_NOTICE([Ignore the Atlas checks, they are just used for building test code])

AC_LANG(C++)

AC_CHECK_FUNCS([fabsf frexpf sqrtf atan2f sinf cosf])

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(ATLAS_0_6, atlascpp-0.6 >= 0.6.0, 
	[ found_atlas_0_6=yes
	dnl Don't add Atlas CFLAGS, LIBS, since Atlas is only used in
	dnl conversion functions to Atlas::Message::Object. Anyone using
	dnl those should already be using Atlas explicitly.
	], found_atlas_0_6=no)

AM_CONDITIONAL(HAVE_ATLAS_0_6, test $found_atlas_0_6 == yes)

AC_CHECK_HEADERS(sstream strstream, [ found_string_stream=yes break ],
				    [ found_string_stream=no ])
if test $found_string_stream == no ; then
    AC_MSG_ERROR(Couldn't find C++ string <-> stream conversion method.)
fi

AC_SUBST(SYSTEM_LIBS)
AC_SUBST(WFMATH_LIB_SUFFIX)
AC_SUBST(WFMATH_LIBS)

AC_CONFIG_FILES([Makefile
	wfmath/Makefile
	atlas/Makefile
	doc/Makefile
	doc/Doxyfile
	wfmath.spec
	mingw32-wfmath.spec
	wfmath-0.3.pc
])
AC_OUTPUT
