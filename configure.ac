dnl Process this file with autoconf to produce a configure script.
AC_INIT(wfmath/wfmath.h)

WFMATH_MAJOR=0
WFMATH_MINOR=2
WFMATH_MICRO=10
WFMATH_VERSION=$WFMATH_MAJOR.$WFMATH_MINOR.$WFMATH_MICRO

AC_SUBST(WFMATH_MAJOR)
AC_SUBST(WFMATH_MINOR)
AC_SUBST(WFMATH_MICRO)
AC_SUBST(WFMATH_VERSION)

dnl versioning info for libtool
WFMATH_CURRENT=1
WFMATH_REVISION=0
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
WFMATH_CFLAGS=""
WFMATH_LIBS="-lwfmath-0.2 -lm"


PKG_CHECK_MODULES(ATLAS, atlascpp-0.4 >= 0.4.5, 
	[ found_atlas=yes
	dnl Don't add Atlas CFLAGS, LIBS, since Atlas is only used in
    dnl conversion functions to Atlas::Message::Object. Anyone using
    dnl those should already be using Atlas explicitly.
	], found_atlas=no)

if test $found_atlas == no; then
	AC_MSG_NOTICE([
*** stable libAtlas >= 0.4.5 not found -- conversion tests will not be built
*** The STABLE release version of atlas can be found at:
*** ftp://ftp.worldforge.org/pub/worldforge/libs/Atlas-C++/Atlas-C++-0.4.6.tar.bz2])
fi

AM_CONDITIONAL(HAVE_ATLAS, test $found_atlas == yes)

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

AC_SUBST(WFMATH_CFLAGS)
AC_SUBST(WFMATH_LIBS)

AC_OUTPUT([Makefile 
           wfmath/Makefile 
           atlas/Makefile 
           doc/Makefile
           doc/Doxyfile
           wfmath-config
           wfmath.spec
           wfmath-0.2.pc
])
