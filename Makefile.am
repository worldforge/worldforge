m4datadir = $(datadir)/aclocal
m4data_DATA = wfmath.m4

bin_SCRIPTS = wfmath-config

manpagedir = $(mandir)/man1/
manpage_DATA = wfmath-config.1

EXTRA_DIST = TODO autogen.sh wfmath.m4 wfmath-config.in wfmath-config.1

SUBDIRS = wfmath atlas doc

doc:
	cd doc && make doc
