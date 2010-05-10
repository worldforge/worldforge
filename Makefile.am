SUBDIRS = Eris bindings test

EXTRA_DIST = autogen.sh Eris.pbproj/project.pbxproj eris.spec CHANGES-1.4 ChangeLog-CVS generate-ChangeLog.sh

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eris-1.3.pc

doc:
	doxygen eris.dox

dist-hook:
	sh $(top_srcdir)/generate-ChangeLog.sh $(top_srcdir) $(distdir)
#chmod u+w $(distdir)/ChangeLog && git log 6951ccbc623ff06dc9598bef83c3d820f9b51aec..HEAD --stat --name-only --date=short --abbrev-commit > $(distdir)/ChangeLog && cat $(top_srcdir)/ChangeLog >> $(distdir)/ChangeLog 
