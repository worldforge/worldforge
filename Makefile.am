## Process this file with automake to generate Makefile.in

SUBDIRS = varconf doc tests

bin_SCRIPTS = varconf-config

m4datadir = $(datadir)/aclocal
m4data_DATA = varconf.m4

manpagedir = $(mandir)/man1
manpage_DATA = varconf-config.1

EXTRA_DIST = autogen.sh varconf.spec.in varconf.m4 varconf-config.1

docdir = $(datadir)/doc/varconf-$(VERSION)
doc_DATA = README COPYING AUTHORS THANKS NEWS
