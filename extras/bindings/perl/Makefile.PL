use ExtUtils::MakeMaker;
use SigC;

SigC::create_glue_code("conversion_wrapper.h",  ["perlobject.map", "typemap"],
	{"Person *" => "WorldForge::Eris::Person",
	"Room *" => "WorldForge::Eris::Room",
	"Entity *" => "WorldForge::Eris::Entity"});

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'		=> 'WorldForge::Eris',
    'VERSION_FROM'	=> 'Eris.pm', # finds $VERSION
    'PREREQ_PM'		=> {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?    ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM => 'Eris.pm', # retrieve abstract from module
       AUTHOR     => 'A. U. Thor <a.u.thor@a.galaxy.far.far.away>') : ()),
);
