## **************** Generic Stuff *****************

.PHONY: perl-all perl-clean perl-check

all: perl-all

clean: perl-clean

check: perl-check

## ************** Specific Modules ****************

if PERL_BINDINGS

## rebuild the perl module makefile if we touch the headers,
## so that the module gets rebuilt
perl-makefile.stamp: perl/Makefile.PL
	cd perl && perl Makefile.PL PREFIX=$prefix LIB=$libdir/perl
	touch perl-makefile.stamp

perl-all: perl-makefile.stamp
	cd perl && make

perl-clean:
	test -e perl-makefile.stamp && cd perl && make clean
	rm -f perl-makefile.stamp

perl-check: perl-makefile.stamp
	cd perl && make test

endif
## PERL_BINDINGS
