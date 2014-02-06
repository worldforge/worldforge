SUBDIRS = Eris bindings test

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = autogen.sh Eris.pbproj/project.pbxproj eris.spec CHANGES-1.4 support/ChangeLog-CVS support/generate-ChangeLog.sh 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eris-1.4.pc

docs:
	doxygen eris.dox

dist-hook:
	sh $(top_srcdir)/support/generate-ChangeLog.sh $(top_srcdir) $(distdir) 6951ccbc623ff06dc9598bef83c3d820f9b51aec
