# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use Test;
BEGIN { plan tests => 1 };
use WorldForge::Eris;
ok(1); # If we made it this far, we're ok.

#########################

# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.

my ($host, $account, $password) = ("localhost", "BotBert", "foobar");

my $finished;
my $player;
my $conn;

sub failure {
  my ($reason) = @_;
  print "Failed to connect: $reason\n";
  $finished = 1;
}

sub connected {
  print "Received connected signal\n";

  $player = new WorldForge::Eris::Player($conn);
  $player->LoginFailure->connect("login_failure");
  $player->LoginSuccess->connect("login_success");
  $player->LogoutComplete->connect("logout_complete");

  $player->login($account, $password);
#  $player->createAccount($account, $account, $password);
}

sub disconnected {
  print "Received disconnected signal\n";
  $finished = 1;
}

sub disconnecting {
  print "Received disconnecting signal\n";
  return false;
}

sub login_failure {
  my ($reason, $error) = @_;
  print "Failed to login: $reason: $error\n";
  disconnect $conn;
}

sub login_success {
  print "Successful login\n";
  $conn->getLobby()->say("Testing...Testing...1...2...3...Testing");
  logout $player;
}

sub logout_complete {
  print "Logged out\n";
  disconnect $conn;
}

$conn = new WorldForge::Eris::Connection("test_client", true)
	or die "Unable to create connection";

ok(1);

$conn->Failure->connect("failure");
$conn->Connected->connect("connected");
$conn->Disconnected->connect("disconnected");
$conn->Disconnecting->connect("disconnecting");

ok(1);

$conn->connect($host);

ok(1);

WorldForge::Eris::Poll::poll(1) until $finished;

ok(1);
