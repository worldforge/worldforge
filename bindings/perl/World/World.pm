package WorldForge::Eris::World;

use 5.006;
use strict;
use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use WorldForge::Eris::World ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);
our $VERSION = '0.01';

bootstrap WorldForge::Eris::World $VERSION;

# Preloaded methods go here.

use SigC;

# Factory class implementation
package WorldForge::Eris::Factory;

sub new
{
  my $class = shift;
  my $accept_func = shift;

  if(!$accept_func->isa("SigC::Slot")) {
    $accept_func = new SigC::Slot($accept_func);
    return if !$accept_func;
  }

  my $instantiate_func;

  if(($_[0])->isa("SigC::Slot")) {
    $instantiate_func = shift;
    $instantiate_func = $instantiate_func->bind(@_) if @_;
  }
  else {
    $instantiate_func = new SigC::Slot(@_);
    return if !$instantiate_func;
  }

  return bless {_Eris_Factory_accept_func => $accept_func,
                _Eris_Factory_instantiate_func => $instantiate_func}, $class;
}

package WorldForge::Eris::World;

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

WorldForge::Eris::World - Perl extension for blah blah blah

=head1 SYNOPSIS

  use WorldForge::Eris::World;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for WorldForge::Eris::World, created by h2xs. It looks like the
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
