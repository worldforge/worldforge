# Autoconf setup
AC_INIT
AC_PREREQ(2.5)
AC_CONFIG_SRCDIR([Eris/Entity.cpp])

# Detect the canonical host and target environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Information
ERIS_MAJOR=1
ERIS_MINOR=1
ERIS_MICRO=1
ERIS_PATCH=0
ERIS_VERSION=$ERIS_MAJOR.$ERIS_MINOR.$ERIS_MICRO.$ERIS_PATCH

# Automake setup
AM_INIT_AUTOMAKE(eris, $ERIS_VERSION)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)	

AC_SUBST(ERIS_VERSION)

dnl versioning info for libtool
ERIS_CURRENT=1
ERIS_REVISION=0
ERIS_AGE=0
ERIS_VERSION_INFO=$ERIS_CURRENT:$ERIS_REVISION:$ERIS_AGE

ERIS_REQUIRES=""

AC_SUBST(ERIS_VERSION_INFO)

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AC_HEADER_STDC([])
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -ansi"

dnl Make sure we've got sensible versions of the sources (eg to get snprintf)

AC_DEFINE(_XOPEN_SOURCE, 500, [This is XOpen source])
AC_DEFINE(_ISOC99_SOURCE,1, [This is C99 source])

AC_LANG_PUSH(C++)

PKG_CHECK_MODULES(SIGC, sigc++-1.2 >= 1.2,
    [
        CXXFLAGS="$CXXFLAGS $SIGC_CFLAGS"
        LDFLAGS="$LDFLAGS $SIGC_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES sigc++-1.2"
    ],
AC_MSG_ERROR(Couldn't find libsigc++ config [or config failed]))

# check for all the WorldForge libs we use
PKG_CHECK_MODULES(WF, skstream-0.3 atlascpp-0.5 >= 0.4.93 wfmath-0.3 >= 0.3.2,
    [
        CXXFLAGS="$CXXFLAGS $WF_CFLAGS"
        LDFLAGS="$LDFLAGS $WF_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES skstream-0.3 atlascpp-0.5 >= 0.4.93 wfmath-0.3"
    ],
    AC_MSG_ERROR(Couldn't find the required WorldForge libraries))

AC_LANG_POP(C++)

AC_ARG_ENABLE(perl, 
[  --enable-perl         Enable building of Perl bindings
],
[
dnl testing for sigcperl should be sufficient, since that requires
dnl perl and sucks in its CFLAGS and LIBS
	PKG_CHECK_MODULES(SIGCPERL, sigcperl >= 0.2, [
		AC_MSG_CHECKING(for Perl module SigC)
		if (`perl -e "use SigC;"`)
		then
			AC_MSG_RESULT(yes)
			enable_perl='yes'
		else
			AC_MSG_RESULT(no)
			AC_MSG_WARN(SigC module not found, not building perl bindings)
			enable_perl='no'
		fi 
	],[
		AC_MSG_WARN(libsigcperl not found, not building perl bindings)
		enable_perl='no'
	])
])
AM_CONDITIONAL(PERL_BINDINGS, test "x$enable_perl" = "xyes")

ERIS_POLL_PKGCONFIGS=
ERIS_POLL_GLIB_REQUIRES=

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.0.0,
    [
	ERIS_POLL_PKGCONFIGS="$ERIS_POLL_PKGCONFIGS eris-poll-glib-1.2.pc"
	ERIS_POLL_GLIB_REQUIRES=glib-2.0
	have_glib='yes'
    ],
    [
        AM_PATH_GLIB(1.2.0,[
		ERIS_POLL_PKGCONFIGS="$ERIS_POLL_PKGCONFIGS eris-poll-glib-1.2.pc"
		have_glib='yes'
	        ],[
		AC_MSG_WARN(Couldn't find glib, not building glib poll)
		have_glib='no'
		]
        )
    ])
AM_CONDITIONAL(HAVE_GLIB, test "x$have_glib" = "xyes")

AC_ARG_ENABLE(janus, 
[  --enable-janus         Enable building of experimental Atlas UI extension
],
[
  AC_LANG_PUSH(C++)
  AC_MSG_CHECKING(for experimental Atlas UI extension)
  AC_TRY_COMPILE([#include <Atlas/Objects/Entity/UIEntity.h>],[],
      [
	have_atlas_janus=yes
	AC_DEFINE(HAVE_ATLAS_JANUS,[],["Experimental janus extension"])
	AC_MSG_RESULT(yes)
      ],
      [
	have_atlas_janus=no
	AC_MSG_RESULT(no)
      ]
  )
  AC_LANG_POP(C++)
])
AM_CONDITIONAL(HAVE_ATLAS_JANUS, test "x$have_atlas_janus" = "xyes")

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

SYSTEM_LIBS=""
ERIS_LIB_SUFFIX="-1.2"
ERIS_LIBS="-leris$ERIS_LIB_SUFFIX -lm"
ERIS_CFLAGS=""
AC_SUBST(SYSTEM_LIBS)
AC_SUBST(ERIS_LIB_SUFFIX)
AC_SUBST(ERIS_LIBS)
AC_SUBST(ERIS_CFLAGS)
AC_SUBST(ERIS_REQUIRES)
AC_SUBST(ERIS_POLL_PKGCONFIGS)
AC_SUBST(ERIS_POLL_GLIB_REQUIRES)

AC_CONFIG_FILES([
Makefile
Eris/Makefile
bindings/Makefile
bindings/polls/Makefile
bindings/polls/glib/Makefile
bindings/perl/compile_flags.pl
test/Makefile
eris.dox
eris.spec
eris-1.2.pc
eris-poll-glib-1.2.pc
])
AC_OUTPUT
