SUBDIRS = Eris bindings test

EXTRA_DIST = autogen.sh Eris.pbproj/project.pbxproj eris.spec

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eris-1.2.pc @ERIS_POLL_PKGCONFIGS@

doc:
	doxygen eris.dox
