SUBDIRS = polls

## **************** Generic Stuff *****************

.PHONY: perl-all perl-clean perl-check perl-dist-hook perl-install

all: perl-all

clean-local: perl-clean

check: perl-check

dist-hook: perl-dist-hook

install-exec-hook: perl-install

## ************** Specific Modules ****************

if PERL_BINDINGS

## rebuild the perl module makefile if we touch the headers,
## so that the module gets rebuilt, the dependence on the build lib
## ensures the rebuild happens
perl-makefile.stamp: perl/Makefile.PL perl/*/Makefile.PL perl/compile_flags.pl \
			$(top_builddir)/Eris/.libs/liberis-1.2.so perl/*.h
	cd perl && perl Makefile.PL PREFIX=$(prefix) LIB=$(libdir)/perl
	touch perl-makefile.stamp

perl-all: perl-makefile.stamp
	cd perl && make

perl-clean:
	test -e perl-makefile.stamp && cd perl && make clean
	rm -f perl-makefile.stamp

perl-check: perl-makefile.stamp
	cd perl && make test

perl-install: perl-makefile.stamp
	cd perl && make install

endif
## PERL_BINDINGS

## We need to distribute the perl code in the tarball, regardless of
## whether we're building bindings

## This assumes only one level of directories under bindings/perl,
## that's all we need for now and I don't know how portable mkdir -p is

perl-dist-hook:
	perl_dist_dir=`cd $(distdir) && pwd`/perl; \
	test -d $$perl_dist_dir || mkdir $$perl_dist_dir || exit 1; \
	perl_dist_files=`cat perl/MANIFEST`; \
	for file in $$perl_dist_files; do \
		test -e perl/$$file || exit 1; \
		dir=`echo $$file | sed -e 's#/*[^/]*$$##'`; \
		test "$$dir" = "" || test -d $$perl_dist_dir/$$dir \
			|| mkdir $$perl_dist_dir/$$dir || exit 1; \
		cp perl/$$file $$perl_dist_dir/$$file || exit 1; \
	done
