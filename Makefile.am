
SUBDIRS = Atlas tests tutorial benchmark

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atlascpp-0.6.pc

EXTRA_DIST = ROADMAP Doxyfile footer.html index.dox

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
