m4datadir = $(datadir)/aclocal
m4data_DATA = wfmath.m4

bin_SCRIPTS = wfmath-config

manpagedir = $(mandir)/man1/
manpage_DATA = wfmath-config.1

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wfmath-0.2.pc

EXTRA_DIST = TODO autogen.sh wfmath.m4 wfmath-config.in wfmath-config.1 \
	wfmath.pbproj/jmt.pbxuser wfmath.pbproj/project.pbxproj

SUBDIRS = wfmath atlas doc

doc:
	cd doc && make doc
