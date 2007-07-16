
SUBDIRS = Atlas tests tools tutorial benchmark

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atlascpp-0.6.pc

EXTRA_DIST = HACKING ROADMAP Doxyfile footer.html index.dox \
             debian/changelog debian/compat debian/copyright debian/docs \
             debian/libatlas-cpp-0.6-0-dbg.install \
             debian/libatlas-cpp-0.6-0.install \
             debian/libatlas-cpp-0.6-dev.install

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
