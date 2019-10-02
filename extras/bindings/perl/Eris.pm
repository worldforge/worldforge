package WorldForge::Eris;

use 5.006;
use strict;
use warnings;

require Exporter;
use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use WorldForge::Eris ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);
our $VERSION = '0.01';


# Preloaded methods go here.

use SigC;
use WorldForge::Eris::Avatar;
use WorldForge::Eris::Connection;
use WorldForge::Eris::Entity;
use WorldForge::Eris::Room;
use WorldForge::Eris::Lobby;
use WorldForge::Eris::Log;
use WorldForge::Eris::Meta;
use WorldForge::Eris::Player;
use WorldForge::Eris::Timeout;
use WorldForge::Eris::TypeInfo;
use WorldForge::Eris::World;

sub configure {return SigC::basic_configure(shift, 'Eris', 'eris-config');}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

WorldForge::Eris - Perl extension for blah blah blah

=head1 SYNOPSIS

  use WorldForge::Eris;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for WorldForge::Eris, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.


=head1 AUTHOR

A. U. Thor, E<lt>a.u.thor@a.galaxy.far.far.awayE<gt>

=head1 SEE ALSO

L<perl>.

=cut
