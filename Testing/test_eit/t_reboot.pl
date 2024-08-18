#!/usr/bin/perl -w
 
use strict;
use LWP::UserAgent;
 

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://128.100.101.145/reboot',
  [
    'none' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;
 
if($response->content =~ m/Device Rebooting/) {
  print "Device is rebooting\n";
}else{
  print "Some problem ";
} 


exit;
