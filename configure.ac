# Worldforge Next Generation MetaServer
#
# Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

AC_INIT([metaserver-ng],[1.0.3],[sryan@evercrack.com])
LT_INIT

AC_CONFIG_SRCDIR(src)
AC_CONFIG_MACRO_DIR([m4])

# Detect the canonical host and target build environment.
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([tar-ustar nostdinc dist-bzip2 color-tests parallel-tests])
AM_CONFIG_HEADER(src/config.h)

AC_PREREQ(2.62)
AC_COPYRIGHT([This script is under the GPL and is owned by the people in AUTHORS])

AC_ENABLE_SHARED
AC_ENABLE_STATIC

# Checks for Programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# set default lang
AC_LANG([C++])
AC_REQUIRE_CPP
AX_CXX_COMPILE_STDCXX_11(noext)


# Check for C libraries:
AC_LANG_PUSH([C])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(math.h)
dnl Test for <math.h> header file
AC_CHECK_HEADERS([signal.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset select])

# end C check block
AC_LANG_POP([C])


# check sighandler_t in C++
AC_CHECK_TYPES([sighandler_t],[],[],[
#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif])

dnl Check for boost asio header
AC_CHECK_HEADERS([boost/asio.hpp])
AC_CHECK_HEADERS([boost/filesystem.hpp])

# check for boost_system
AC_CHECK_LIB(boost_system, main, , [
	AC_MSG_ERROR("Linking against boost::system library failed.")
])

AC_CHECK_LIB(boost_date_time, main, , [
	AC_MSG_ERROR("Linking against boost::date_time library failed.")
])

AC_CHECK_LIB(boost_program_options, main, , [
	AC_MSG_ERROR("Linking against boost::program_options library failed.")
])

AC_CHECK_LIB(boost_filesystem, main, , [
	AC_MSG_ERROR("Linking against boost::filesystem library failed.")
])

AC_CHECK_LIB(pthread, main, ,[
	AC_MSG_ERROR("Linking against pthread library failed.")
])

# check for glog header
AC_CHECK_HEADERS([glog/logging.h])
AC_CHECK_LIB(glog, main, , [
	AC_MSG_ERROR("Linking against google::glog library failed.")
])

PKG_CHECK_MODULES( CPPUNIT, cppunit >= 1.12 )

# debugging option
AC_ARG_ENABLE(debug,[  --enable-debug     creates debugging code [default=no]],
[ 
   if test x$enableval = xno; then 
		ac_use_debug_code="no"	
   else 
		ac_use_debug_code="yes"
   fi
], [ac_use_debug_code="no"])

if test "$ac_use_debug_code" = "yes"; then
      CFLAGS="$CFLAGS -DDEBUG"
      CXXFLAGS="$CXXFLAGS -DDEBUG"
else
      CFLAGS="$CFLAGS -DNDEBUG"
      CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

# gprof debugging option
AC_ARG_ENABLE(gprof,[  --enable-gprof     Add flags for gprof [default=no]],
[
   if test x$enableval = xno; then 
		ac_use_gprof_code="no"	
   else 
		ac_use_gprof_code="yes"
   fi
], [ac_use_gprof_code="no"])

if test "$ac_use_gprof_code" = "yes"; then
      CFLAGS="$CFLAGS -pg -g"
      CXXFLAGS="$CXXFLAGS -pg -g"
fi

# gprof debugging option
AC_ARG_ENABLE(effc,[  --enable-effc++     Add warning flags for eff++ [default=no]],
[
   if test x$enableval = xno; then 
		ac_use_effc_code="no"	
   else 
		ac_use_effc_code="yes"
   fi
], [ac_use_effc_code="no"])

if test "$ac_use_effc_code" = "yes"; then
	  # c++ only option
      CXXFLAGS="$CXXFLAGS -Weffc++"
fi

# conditional server arg to allow --disable-server in the
# case of an API only build
AC_ARG_ENABLE([server],
  [
    AS_HELP_STRING([--disable-server],[Make as an API install only])
  ],
  [
    # Needs more chicken waving ... can't find
    # autoconf docs on AC_CASE anywhere
    #AS_CASE([$ac_enable_server],[yes|no],[true],
    #[
    #AC_MSG_ERROR([Use --enable-server or --disable-server])
    #])
    case "${enableval}" in
	yes) ac_enable_server=true;;
	no) ac_enable_server=false;;
	*) AC_MSG_ERROR([--enable-server|--disable-server must be toggles or have 'yes' or 'no' as parameters.]);;
    esac
  ],
  [ac_enable_server=true]
)

AM_CONDITIONAL([BUILD_SERVER],[test "x$ac_enable_server" = "xtrue"])

AS_IF([test x"$logdir" = "x"],[logdir="${prefix}/log"])
AS_IF([test x"$rundir" = "x"],[rundir="${prefix}/var"])
AS_IF([test x"$BUILDID" = "x"], [BUILDID=`git show-ref HEAD | awk '{ print $1 }'`])
AS_IF([test x"$BUILDUSER" = "x"], [BUILDUSER=`whoami`])
AS_IF([test x"$BUILDHOST" = "x"], [BUILDHOST=`uname -n`])
AS_IF([test x"$BUILDSIG" = "x"], [BUILDSIG=`uname -a`])
AC_SUBST([logdir])
AC_SUBST([rundir])
AC_SUBST([APIVER],[1:0:0])
AC_SUBST([BUILDID])
AC_SUBST([BUILDUSER])
AC_SUBST([BUILDHOST])
AC_SUBST([BUILDSIG])
AC_CONFIG_FILES([Makefile
		 src/Makefile
		 metaserver-ng.dox
		 metaserver-ng.conf
		 mswatchdog.sh
		 src/MetaServerVersion.hpp
		 src/MetaServerAPI.hpp
		 src/MetaServer.hpp
])

AC_OUTPUT

