dnl Process this file with autoconf to produce a configure script.
AC_INIT(wfmath/wfmath.h)

WFMATH_MAJOR=0
WFMATH_MINOR=3
WFMATH_MICRO=2
WFMATH_VERSION=$WFMATH_MAJOR.$WFMATH_MINOR.$WFMATH_MICRO

AC_SUBST(WFMATH_MAJOR)
AC_SUBST(WFMATH_MINOR)
AC_SUBST(WFMATH_MICRO)
AC_SUBST(WFMATH_VERSION)

dnl versioning info for libtool
WFMATH_CURRENT=3
WFMATH_REVISION=2
WFMATH_AGE=0
WFMATH_VERSION_INFO=$WFMATH_CURRENT:$WFMATH_REVISION:$WFMATH_AGE
WFMATH_VERSION_NO=$WFMATH_CURRENT.$WFMATH_AGE.$WFMATH_REVISION

AC_SUBST(WFMATH_CURRENT)
AC_SUBST(WFMATH_VERSION_INFO)
AC_SUBST(WFMATH_VERSION_NO)

AM_INIT_AUTOMAKE(wfmath, $WFMATH_VERSION)
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CXX
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

dnl Checks for libraries.
dnl Replace `main' with a function in -libs:
SYSTEM_LIBS="-lm"
WFMATH_CFLAGS=""
WFMATH_LIB_SUFFIX="-0.3"
WFMATH_LIBS="-lwfmath$WFMATH_LIB_SUFFIX"

AC_MSG_NOTICE([Ignore the Atlas checks, they're just used for building test code])

PKG_CHECK_MODULES(ATLAS_0_4, atlascpp-0.4 >= 0.4.5, 
	[ found_atlas_0_4=yes
	dnl Don't add Atlas CFLAGS, LIBS, since Atlas is only used in
    dnl conversion functions to Atlas::Message::Object. Anyone using
    dnl those should already be using Atlas explicitly.
	], found_atlas_0_4=no)

PKG_CHECK_MODULES(ATLAS_0_6, atlascpp-0.6 >= 0.5.0, 
	[ found_atlas_0_6=yes
	dnl Don't add Atlas CFLAGS, LIBS, since Atlas is only used in
    dnl conversion functions to Atlas::Message::Object. Anyone using
    dnl those should already be using Atlas explicitly.
	], found_atlas_0_6=no)

AM_CONDITIONAL(HAVE_ATLAS_0_4, test $found_atlas_0_4 == yes)
AM_CONDITIONAL(HAVE_ATLAS_0_6, test $found_atlas_0_6 == yes)

AC_LANG_PUSH(C++)

AC_CHECK_HEADERS(sstream strstream, [ found_string_stream=yes break ],
				    [ found_string_stream=no ])
if test $found_string_stream == no ; then
    AC_MSG_ERROR(Couldn't find C++ string <-> stream conversion method.)
fi

AC_LANG_POP

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_C_CONST

dnl Checks for library functions.

AC_SUBST(SYSTEM_LIBS)
AC_SUBST(WFMATH_CFLAGS)
AC_SUBST(WFMATH_LIB_SUFFIX)
AC_SUBST(WFMATH_LIBS)

AC_OUTPUT([Makefile 
           wfmath/Makefile 
           atlas/Makefile 
           doc/Makefile
           doc/Doxyfile
           wfmath.spec
           wfmath-0.3.pc
])
