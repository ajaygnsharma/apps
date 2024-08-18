#!/usr/bin/perl -w
use strict;
use LWP::Simple;
  
my $home_page = get("http://128.100.101.116/index.htm");
die "Couldn't get it?!" unless $home_page;
my $count = 0;
$count++ while $home_page =~ m{newportUS}gi;
print "$count\n";
