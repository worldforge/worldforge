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
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

AC_INIT(metaserver-ng, 0.0.2, sryan@evercrack.com)

AC_CONFIG_SRCDIR(src)
AC_CONFIG_MACRO_DIR([m4])

# Detect the canonical host and target build environment.
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([tar-ustar nostdinc dist-bzip2 color-tests parallel-tests])
AM_CONFIG_HEADER(src/config.h)

AC_PREREQ(2.50)
AC_COPYRIGHT([This script is under the GPL and is owned by the people in AUTHORS])

AC_DISABLE_SHARED

# Checks for Programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# set default lang
AC_LANG([C++])
AC_REQUIRE_CPP

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


PKG_CHECK_MODULES( CPPUNIT, cppunit >= 1.12.0 )

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

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 metaserver-ng.dox
])

AC_OUTPUT

