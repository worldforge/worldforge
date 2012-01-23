## Process this file with automake to generate Makefile.in

check_PROGRAMS = symbol-check conftest

TESTS = conftest

EXTRA_DIST = conf.cfg 

DISTCLEANFILES = conf2.cfg

INCLUDES = -I$(top_srcdir) -DSRCDIR=\"$(srcdir)\"
LDADD = $(top_builddir)/varconf/libvarconf-1.0.la

conftest_SOURCES = conftest.cpp

symbol-check:
	@set -e; \
	newsymbols=$$(mktemp); \
	test -f ${srcdir}/symbols.${host_cpu} && \
	nm --defined-only --dynamic --demangle $(top_builddir)/varconf/.libs/libvarconf-1.0.so \
	| cut --delimiter=' ' --fields=3- \
	| sort -u > $${newsymbols} && \
	diff ${srcdir}/symbols.${host_cpu} $${newsymbols} && \
	echo "symbol check passed" || \
	echo "*** symbols have changed"

nodist_symbol_check_SOURCES =
.PHONY: symbol-check

