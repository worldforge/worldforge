SUBDIRS = src bindings test

EXTRA_DIST = eris.m4 eris-config.1

bin_SCRIPTS = eris-config

m4datadir = $(datadir)/aclocal
m4data_DATA = eris.m4

manpagedir = $(mandir)/man1
manpage_DATA = eris-config.1

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eris-1.0.pc

doc:
	doxygen eris.dox
