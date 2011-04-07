dnl Autoconf setup

AC_INIT([eris], [1.3.18], [alriddoch@googlemail.com])
AC_PREREQ(2.5)
AC_CONFIG_SRCDIR([Eris/Entity.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Automake setup

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 color-tests parallel-tests])

dnl versioning info for libtool
ERIS_CURRENT=18
ERIS_REVISION=0
ERIS_AGE=0

ERIS_REQUIRES=""
ERIS_DEPS=""

LT_INIT([disable-static])

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -Wno-unknown-pragmas -DNDEBUG"
    ]
)


dnl Make sure we've got sensible versions of the sources (eg to get snprintf)

dnl AC_DEFINE(_XOPEN_SOURCE, 500, [This is XOpen source])
dnl AC_DEFINE(_ISOC99_SOURCE,1, [This is C99 source])

AC_LANG_PUSH(C++)

dnl Check for float to int converters

AC_CHECK_FUNCS([llrint lrintf rintf rint])
AC_CHECK_FUNCS([fabsf])

AC_MSG_CHECKING(for mingw32)
AC_TRY_COMPILE(,[
#ifdef __MINGW32__
#error MingW32 detected, adding -lws2_32 and -lwsock32 to LIBS
#endif
],[
    AC_MSG_RESULT(no)
    have_winsock='no'
],[
    AC_MSG_RESULT(yes)
    have_winsock='yes'
])
AM_CONDITIONAL(HAVE_WINSOCK, test "x$have_winsock" = "xyes")


PKG_PROG_PKG_CONFIG


PKG_CHECK_MODULES(SIGC, sigc++-2.0 != 2.0.8,
    [
        CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
        LIBS="$LIBS $SIGC_LIBS"
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
PKG_CHECK_MODULES(WF, skstream-0.3 atlascpp-0.6 >= 0.5.94 wfmath-0.3 >= 0.3.9 mercator-0.3 >= 0.2.6,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        ERIS_DEPS="$ERIS_DEPS $WF_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES skstream-0.3 atlascpp-0.6 wfmath-0.3 mercator-0.3"
    ],[
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid versions of required WorldForge libraries:
$WF_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/libraries/eris for details of the
libraries required and where to obtain them.])
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

ERIS_LIB_SUFFIX="-1.3"
ERIS_LIBS="-leris$ERIS_LIB_SUFFIX"
ERIS_VERSION_INFO=$ERIS_CURRENT:$ERIS_REVISION:$ERIS_AGE

AC_SUBST(ERIS_LIB_SUFFIX)
AC_SUBST(ERIS_DEPS)
AC_SUBST(ERIS_LIBS)
AC_SUBST(ERIS_REQUIRES)
AC_SUBST(ERIS_VERSION_INFO)

AC_CONFIG_FILES([
Makefile
Eris/Makefile
bindings/Makefile
bindings/polls/Makefile
bindings/polls/glib/Makefile
bindings/polls/winsock/Makefile
bindings/perl/compile_flags.pl
test/Makefile
eris.dox
eris.spec
mingw32-eris.spec
eris-1.3.pc
])
AC_OUTPUT
