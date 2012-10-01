#!/bin/sh
# Run this to generate all the initial makefiles, etc.
#
# NOTE: This autogen.sh copied from GNOME and modified for WFMath.
#       Original autogen.sh was licensed under LGPL.
#
# Extra NOTE: This script was then recopied from WFMath to WFMath

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to compile WFMath."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

(grep "^AM_PROG_LIBTOOL" ./configure.ac >/dev/null) && {
  (libtoolize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "**Error**: You must have \`libtool' installed to compile WFMath."
    echo "Get ftp://ftp.gnu.org/pub/gnu/libtool-1.2d.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
  }
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed to compile WFMath."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.3.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.3.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

if test -z "$*"; then
  echo "**Warning**: I am going to run \`configure' with no arguments."
  echo "If you wish to pass any to it, please specify them on the"
  echo \`$0\'" command line."
  echo
fi

aclocalinclude="$ACLOCAL_FLAGS"

#echo "Running libtoolize..."
libtoolize --automake --copy --force

#libtoolize --force --copy

echo "Running aclocal $aclocalinclude ..."
aclocal $aclocalinclude -I m4
echo "Running autoheader..."
autoheader

echo "Running automake --gnu $am_opt ..."
automake --add-missing --copy --gnu $am_opt

echo "Running autoconf ..."
autoconf


if test "x$NOCONFIGURE" = "x" ; then
    conf_flags="--enable-debug"
    echo Running ./configure $conf_flags "$@" ...
    ./configure $conf_flags "$@" && echo Now type \`make\' to compile WFMath
fi
