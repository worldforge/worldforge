## Process this file with automake to generate Makefile.in

SUBDIRS = varconf doc tests

EXTRA_SCRIPTS = varconf-config

bin_SCRIPTS = @VARCONF_SCRIPT@

m4datadir = $(datadir)/aclocal
m4data_DATA = @VARCONF_MACRO@

manpagedir = $(mandir)/man1
manpage_DATA = varconf-config.1

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = varconf-0.5.pc

EXTRA_DIST = autogen.sh varconf.spec.in varconf.m4 varconf-config.1

docdir = $(datadir)/doc/varconf-$(VERSION)
doc_DATA = README COPYING AUTHORS THANKS NEWS
