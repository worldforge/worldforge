m4datadir = $(datadir)/aclocal
m4data_DATA = wfmath.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wfmath-0.3.pc

EXTRA_DIST = TODO autogen.sh wfmath.m4 \
	wfmath.pbproj/project.pbxproj

SUBDIRS = wfmath atlas doc

doc:
	cd doc && make doc
