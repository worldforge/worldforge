dnl Autoconf setup

AC_INIT(eris, 1.3.12, james@worldforge.org)
AC_PREREQ(2.5)
AC_CONFIG_SRCDIR([Eris/Entity.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Automake setup

AM_INIT_AUTOMAKE([nostdinc dist-bzip2])

dnl versioning info for libtool
ERIS_CURRENT=13
ERIS_REVISION=0
ERIS_AGE=0

ERIS_REQUIRES=""

AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -ansi -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -ansi -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -ansi -DNDEBUG"
    ]
)


dnl Make sure we've got sensible versions of the sources (eg to get snprintf)

AC_DEFINE(_XOPEN_SOURCE, 500, [This is XOpen source])
AC_DEFINE(_ISOC99_SOURCE,1, [This is C99 source])

PKG_PROG_PKG_CONFIG

AC_LANG_PUSH(C++)

PKG_CHECK_MODULES(SIGC, sigc++-2.0 != 2.0.8,
    [
        CXXFLAGS="$CXXFLAGS $SIGC_CFLAGS"
        LDFLAGS="$LDFLAGS $SIGC_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES sigc++-2.0"
    ],[
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find a valid version of the sigc++ library:
$SIGC_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://libsigc.sourceforge.net/ for details of how to download and
install the library.])
    ]
)

dnl check for all the WorldForge libs we use
PKG_CHECK_MODULES(WF, skstream-0.3 atlascpp-0.6 >= 0.5.94 wfmath-0.3 >= 0.3.2,
    [
        CXXFLAGS="$CXXFLAGS $WF_CFLAGS"
        LDFLAGS="$LDFLAGS $WF_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES skstream-0.3 atlascpp-0.6 wfmath-0.3"
    ],[
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid versions of required WorldForge libraries:
$WF_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/libraries/eris for details of the libraries required and where to obtain them.])
    ]
)

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
		AC_MSG_RESULT(no)
		AC_MSG_WARN(libsigcperl not found, not building perl bindings)
		enable_perl='no'
	])
])
AM_CONDITIONAL(PERL_BINDINGS, test "x$enable_perl" = "xyes")

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.0.0,
    [
	have_glib='yes'
    ],
    [
	AM_PATH_GLIB(1.2.0,[
		have_glib='yes'
		],[
		AC_MSG_RESULT([no])
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

ERIS_LIB_SUFFIX="-1.3"
ERIS_LIBS="-leris$ERIS_LIB_SUFFIX -lm"
ERIS_VERSION_INFO=$ERIS_CURRENT:$ERIS_REVISION:$ERIS_AGE

AC_SUBST(ERIS_LIB_SUFFIX)
AC_SUBST(ERIS_LIBS)
AC_SUBST(ERIS_REQUIRES)
AC_SUBST(ERIS_VERSION_INFO)

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
eris-1.3.pc
])
AC_OUTPUT
