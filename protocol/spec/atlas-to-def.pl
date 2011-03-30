#!/usr/bin/perl -w

use strict;

#sub log_err {warn @_;}
sub log_err {}

# read in the .atlas header

die "No ATLAS line\n" unless <STDIN> =~ /^ATLAS/;
die "No IWILL line\n" unless <STDIN> =~ /^IWILL/;

# spit out the .def header

print("#Modeline for XEmacs: -*-Python-*-\n");

# main parse loop

# a stack to keep track of whether we're inside an array, map, or whatever
my @parse_type;

# don't put brackets around multi-line arrays
my @multi_line_array;
my $started_multi_line_array;

# call when you finish an element
sub push_done_type {
  return if(!@parse_type);
  if ($parse_type[-1] eq "map_have_key") {
    pop @parse_type;
  } elsif ($parse_type[-1] ne "in_array") {
    die "In invalid state $parse_type[-1] at end of element\n";
  }
  push @parse_type, "need_sep";
  log_err "Parse type stack is now:";
  foreach(@parse_type) {log_err " $_";}
  log_err "\n";

  $started_multi_line_array = 0;
};

sub indent { my $space = (@parse_type - (shift or 0)) - @multi_line_array; print "    " while $space--; };

sub valid_parse_type {return @parse_type ? $parse_type[-1] : "none";}

# Allow repeated passes through the loop for a single line, for cases
# like '{ foo:[""], bar:0 }, {'
LINE: while(my $line = <STDIN>) {

  # ignore comments
  next LINE if $line =~ /^#/;

  while($line) {
    # remove leading whitespace
    $line =~ s/^\s+//gm if !@parse_type or $parse_type[-1] ne "in_string";
    next LINE if !$line;

    if(!@parse_type or $parse_type[-1] eq "map_have_key" or $parse_type[-1] eq "in_array") {
      log_err "Starting new element\n";
      # start new element
      $line =~ s/^(.)//;
      my $char = $1;
      if($char eq "{") { # starting a map
        log_err "Staring a map\n";
        # We print a \n at the beginning of a multi-line array, so don't here
        print "\n" unless @multi_line_array and $multi_line_array[-1] == @parse_type - 1 and $started_multi_line_array;
        push @parse_type, "in_map";
        indent(1);
        print ":map:\n";
      } elsif ($char eq "[") { # starting an array
        log_err "Staring a array\n";
        if (!$line or $line =~ /^[{\n]/) {
          # multi_line_array
          my $place = @parse_type; # scalar cast
          log_err "Starting multi line array at depth $place\n";
          push @multi_line_array, $place;
          print "\n";
          $started_multi_line_array = 1;
        } else { 
          print "[";
        }
        push @parse_type, "in_array";
      } elsif ($char eq "]") { # finished an array, either a trailing comma or empty
        log_err "Finished an array\n";
        pop @parse_type;
        if(@multi_line_array and @parse_type == $multi_line_array[-1]) {
          log_err "Finished multi line array at depth $multi_line_array[-1]\n";
          pop @multi_line_array;
        } elsif(@multi_line_array and @parse_type < $multi_line_array[-1]) {
          die "Ending array at depth @parse_type, deepest multi line array is at $multi_line_array[-1]\n";
        } else {
          print "]";
        }
        push_done_type();
      } elsif ($char eq "\"") { # starting a string
        log_err "Staring a string\n";
        indent() if @multi_line_array and $multi_line_array[-1] == @parse_type - 1;
        push @parse_type, "in_string";
        print "\"";
      }
      elsif ($char =~ /[0-9]/) { # starting a number
        log_err "Staring a number\n";
        indent() if @multi_line_array and $multi_line_array[-1] == @parse_type - 1;
        # print the digits
        $line =~ s/^([0-9.]*)//;
        print "$char$1";
        push_done_type();
      }
      else {
        # $line still has a trailing \n, don't add one to 'die' statement
        die "Invalid element $1$line";
      }
    } elsif ($parse_type[-1] eq "in_map") {
      if($line =~ /^\}/) { # finished a map, either a trailing comma or empty
        $line =~ s/^.//;
        pop @parse_type; # remove "in_map"
        push_done_type();
      } else {
        die "Couldn't find key string in map"
          unless $line =~ s/([A-Za-z_]+:)//;
        log_err "Got key $1\n";
        indent();
        # some keys had name changes
        if($1 eq "summary:") {
          print "description:";
        } elsif($1 eq "description:") {
          print "long_description:";
        } elsif($1 eq "arg_description:") {
          print "args_description:";
        } elsif($1 eq "arg:") {
          print "args:";
        } else { print $1; }
        push @parse_type, "map_have_key";
      }
    } elsif ($parse_type[-1] eq "need_sep") {
      pop @parse_type; # remove "need_sep"
      $line =~ s/^(.)//;
      log_err "Printed separator $1 leaving parse type " . valid_parse_type() ."\n";
      if($1 eq "]") {
        log_err "Got a ]\n";
        die "Ended map with ]" unless $parse_type[-1] eq "in_array";
        pop @parse_type; # remove "in_array"
        if(@multi_line_array and @parse_type == $multi_line_array[-1]) {
          log_err "Finished multi line array at depth $multi_line_array[-1]\n";
          pop @multi_line_array;
        } elsif(@multi_line_array and @parse_type < $multi_line_array[-1]) {
          die "Ending array at depth @parse_type, deepest multi line array is at $multi_line_array[-1]\n";
        } else {
          print "]";
        }
        push_done_type();
      } elsif($1 eq "}") {
        log_err "Got a }\n";
        die "Ended array with }" unless $parse_type[-1] eq "in_map";
        pop @parse_type; # remove "in_map"
        push_done_type();
      } elsif ($1 eq ",") {
        if (@parse_type == 1 || $parse_type[-1] eq "in_map") {
          print "\n"; # toplevel array, maps
        } else {
          print ", "; # deeper array levels
        }
      } else {
        die "Invalid array continuation $1$line";
      }
      log_err "After checking separator, parse type is " . valid_parse_type() . "\n";
    } elsif(@parse_type and $parse_type[-1] eq "in_string") {
      print $1 while $line =~ s/([^\"]\\\")//;
      if($line =~ /([^\"]*\")(.*)/) {
        log_err "Printing string \"$1, remainder of line is \"$2\"\n";
        # handle a changed opname, hopefully this won't mess up anything else
        my $out = $1 eq "communication\"" ? "communicate\"" : $1;
        print $out;
        $line = $2;
        pop @parse_type;
        push_done_type();
        log_err "After printing string, parse type is " . valid_parse_type() . "\n";
      } else {
        print $line;
        $line = "";
      }
    } else {
      die "Invalid parse type $parse_type[-1]\n";
    }
  }
}

# fix no-newline-at-end-of-file
print "\n";

die "Still in state $parse_type[-1], unmatched brackets\n" if @parse_type;
