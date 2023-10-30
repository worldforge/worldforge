#!/usr/bin/perl -w
$pre_flag=0;
while(<>) {
  if(/PRE>/) {$pre_flag=!$pre_flag;}
  elsif($pre_flag) {s/</&lt;/g;s/>/&gt;/g;}
  print;
}
