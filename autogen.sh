#!/bin/sh

LIBTOOLIZE=libtoolize
OS=$(uname -s)

if [ "x$OS" = "xDarwin" ]; then
   LIBTOOLIZE=glibtoolize
fi

rm -f config.cache

if test -d /usr/local/share/aclocal ; then
	ACLOCAL_FLAGS="$ACLOCAL_FLAGS -I /usr/local/share/aclocal"
fi
aclocal $ACLOCAL_FLAGS
$LIBTOOLIZE --force --copy
autoheader
automake --gnu --add-missing --copy
autoconf
