ACLOCAL_AMFLAGS = -I m4

SUBDIRS = Atlas tests tools tutorial benchmark man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atlascpp-0.7.pc

EXTRA_DIST = HACKING ROADMAP Doxyfile footer.html index.dox protocol support/ChangeLog-CVS support/generate-ChangeLog.sh

docs:
	@echo "running doxygen..."
	@doxygen $(top_srcdir)/Doxyfile
	@echo "documentation is in doc/."

dist-hook:
	sh $(top_srcdir)/support/generate-ChangeLog.sh $(top_srcdir) $(distdir) 8bd480b053190ffde2afe33af66f484953036f5a
	
