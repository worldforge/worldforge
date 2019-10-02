#!/usr/bin/perl -w
$wikiFlag=0;
while(<>) {
  if(/<\/body/i) {$wikiFlag=0;}
  if($wikiFlag) {print;}
  if(/<body/i) {$wikiFlag=1;}
}
