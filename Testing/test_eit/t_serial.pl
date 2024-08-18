#!/usr/bin/perl -w
 
use strict;
use LWP::UserAgent;
 

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://128.100.101.145/serial',
  [
    'B' => '6',
    'D' => '4', 
    'P' => '3', 
    'S' => '1', 
    'F' => '1', 
    'rs485Tmr' => '0', 
    'rs485Type' => '0', 
    'rs485Mode' => '0'
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;
 
if($response->content =~ m/value=6 selected="selected">/) {
  print "Done\n";
}else{
  print "Cannot assign baud rate = 9600 ";
} 


exit;
