dnl Autoconf setup

AC_INIT([eris], [1.4.0], [erik@ogenvik.org])
AC_PREREQ(2.5)
AC_CONFIG_SRCDIR([Eris/Entity.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

dnl Detect the canonical host environment

AC_CANONICAL_HOST

dnl Automake setup

AM_INIT_AUTOMAKE([nostdinc dist-bzip2 color-tests parallel-tests no-define silent-rules foreign])
AM_SILENT_RULES([yes])

dnl versioning info for libtool
ERIS_CURRENT=0
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

AX_CXX_COMPILE_STDCXX_11

PKG_PROG_PKG_CONFIG

BOOST_REQUIRE([1.46.0])
BOOST_THREADS
BOOST_DATE_TIME
BOOST_ASIO

LDFLAGS="$LDFLAGS $BOOST_THREAD_LDFLAGS $BOOST_DATE_TIME_LDFLAGS"
ERIS_DEPS="$ERIS_DEPS $BOOST_THREAD_LIBS $BOOST_DATE_TIME_LIBS -lboost_system"
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"


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
PKG_CHECK_MODULES(WF, atlascpp-0.6 >= 0.6.3 wfmath-1.0 >= 1.0.0 mercator-0.3 >= 0.3.3,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        ERIS_DEPS="$ERIS_DEPS $WF_LIBS"
        ERIS_REQUIRES="$ERIS_REQUIRES atlascpp-0.6 wfmath-1.0 mercator-0.3"
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
			AC_MSG_RESULT([yes])
			enable_perl='yes'
		else
			AC_MSG_RESULT([no])
			AC_MSG_WARN([SigC module not found, not building perl bindings])
			enable_perl='no'
		fi
	],[
		AC_MSG_RESULT([no])
		AC_MSG_WARN([libsigcperl not found, not building perl bindings])
		enable_perl='no'
	])
])
AM_CONDITIONAL(PERL_BINDINGS, test "x$enable_perl" = "xyes")

ERIS_LIB_SUFFIX="-1.4"
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
bindings/perl/compile_flags.pl
test/Makefile
eris.dox
eris.spec
mingw32-eris.spec
eris-1.4.pc
])
AC_OUTPUT
