
SUBDIRS = Atlas tests tools tutorial benchmark

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atlascpp-0.6.pc

EXTRA_DIST = HACKING ROADMAP Doxyfile footer.html index.dox protocol

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
