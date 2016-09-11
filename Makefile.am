SUBDIRS = Mercator tests

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mercator-0.4.pc

EXTRA_DIST = mercator.spec Doxyfile autogen.sh support/ChangeLog-CVS support/generate-ChangeLog.sh

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."

dist-hook:
	sh $(top_srcdir)/support/generate-ChangeLog.sh $(top_srcdir) $(distdir) 8ee345f7d1bcc72f08dc295359d7d0b65bb829e6