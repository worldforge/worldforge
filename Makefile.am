SUBDIRS = Eris bindings test

EXTRA_DIST = autogen.sh Eris.pbproj/project.pbxproj eris.spec CHANGES-1.4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eris-1.3.pc @ERIS_POLL_PKGCONFIGS@

doc:
	doxygen eris.dox
