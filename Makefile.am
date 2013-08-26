## Process this file with automake to generate Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = varconf doc tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = varconf-1.0.pc

EXTRA_DIST = autogen.sh varconf.spec.in varconf.spec support/ChangeLog-CVS support/generate-ChangeLog.sh

dist-hook:
	sh $(top_srcdir)/support/generate-ChangeLog.sh $(top_srcdir) $(distdir) df83c0658e12f0714501017f0325cdf9e19e9498

