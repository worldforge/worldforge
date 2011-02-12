#!/bin/sh
# Run this to generate all the initial makefiles, etc.
#
# NOTE: This autogen.sh copied from GNOME and modified for Eris.
#       Original autogen.sh was licensed under LGPL.

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to compile Eris."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

(grep "^AM_PROG_LIBTOOL" ./configure.ac >/dev/null) && {
  (libtoolize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "**Error**: You must have \`libtool' installed to compile Eris."
    echo "Get ftp://ftp.gnu.org/pub/gnu/libtool-1.2d.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
  }
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed to compile Eris."
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

# this happens often enough that I'm sticking it in here...
aclocalinclude="$ACLOCAL_FLAGS"

echo "Running libtoolize..."
libtoolize --automake --force --copy

echo "Running aclocal $aclocalinclude ..."
aclocal $aclocalinclude
echo "Running autoheader..."
autoheader

echo "Running automake --gnu $am_opt ..."
automake --add-missing --gnu $am_opt

echo "Running autoconf ..."
autoconf


if test "x$NOCONFIGURE" = "x" ; then
    conf_flags="--enable-debug" #--enable-iso-c
    echo Running ./configure $conf_flags "$@" ...
    ./configure $conf_flags "$@" && echo Now type \`make\' to compile Eris
fi
