## Process this file with automake to generate Makefile.in

SUBDIRS = varconf doc tests

bin_SCRIPTS = varconf-config

m4datadir = $(datadir)/aclocal
m4data_DATA = varconf.m4

EXTRA_DIST = autogen.sh varconf.spec.in varconf.m4

install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
	$(INSTALL_DATA) README $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
	$(INSTALL_DATA) THANKS $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(datadir)/doc/varconf-$(VERSION)
