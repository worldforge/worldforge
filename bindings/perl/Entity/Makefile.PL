use ExtUtils::MakeMaker;

do "../compile_flags.pl";

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'		=> 'WorldForge::Eris::Entity',
    'VERSION_FROM'	=> 'Entity.pm', # finds $VERSION
    'PREREQ_PM'		=> {SigC => 0.03}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?    ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM => 'Entity.pm', # retrieve abstract from module
       AUTHOR     => 'A. U. Thor <a.u.thor@a.galaxy.far.far.away>') : ()),
    'LIBS'		=> [$LIBS], # e.g., '-lm'
    'DEFINE'		=> '', # e.g., '-DHAVE_SOMETHING'
    'CC'		=> $CC,
    'LD'		=> '$(CC)',
	# Insert -I. if you add *.h files later:
    'INC'		=> $CFLAGS, # e.g., '-I/usr/include/other'
	# Un-comment this if you add C files to link with later:
    # 'OBJECT'		=> '$(O_FILES)', # link all the C files too
    'XSOPT'		=> '-C++',
    'TYPEMAPS'		=> ['../perlobject.map'],
);
