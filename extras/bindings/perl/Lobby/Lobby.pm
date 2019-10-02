package WorldForge::Eris::Lobby;

use 5.006;
use strict;
use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader WorldForge::Eris::Room);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use WorldForge::Eris::Lobby ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);
our $VERSION = '0.01';

bootstrap WorldForge::Eris::Lobby $VERSION;

# Preloaded methods go here.

use SigC;
use WorldForge::Eris::Room;

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

WorldForge::Eris::Lobby - Perl extension for blah blah blah

=head1 SYNOPSIS

  use WorldForge::Eris::Lobby;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for WorldForge::Eris::Lobby, created by h2xs. It looks like the
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
