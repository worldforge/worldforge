SUBDIRS = Mercator tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mercator-0.3.pc

EXTRA_DIST = mercator.spec Doxyfile autogen.sh

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
