ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wfmath-1.0.pc

EXTRA_DIST = TODO autogen.sh wfmath.pbproj/project.pbxproj wfmath.spec support/ChangeLog-CVS support/generate-ChangeLog.sh

SUBDIRS = wfmath atlas doc

docs:
	cd doc && make docs

dist-hook:
	sh $(top_srcdir)/support/generate-ChangeLog.sh $(top_srcdir) $(distdir) 9c7a80fc453236b1c376297e7e77c1728c31b592
