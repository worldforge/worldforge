
SUBDIRS = src tests tutorial benchmark

bin_SCRIPTS = atlas-config

EXTRA_DIST = ROADMAP Doxyfile footer.html index.dox atlas.m4

m4datadir = $(datadir)/aclocal
m4data_DATA = atlas.m4

libAtlasincludedir = $(includedir)/Atlas

libAtlasinclude_HEADERS = config.h

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
