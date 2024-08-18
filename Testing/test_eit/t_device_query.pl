#!/usr/bin/perl -w

use strict;
use LWP::Simple;
use LWP::UserAgent;
use IO::Socket;

my $eit_ip = '192.168.1.200';

print "Posting device command...\n";

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/command',
  [
    'cmd' => '*X01',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/name=response size=43 value="X01/i) {
  print "Device Query successful...\n";
}else{
  print "Device Query unsuccessful, No reply...\n";
}


exit;
