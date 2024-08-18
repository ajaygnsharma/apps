#!/usr/bin/perl -w

use strict;
use LWP::Simple;
use LWP::UserAgent;
use IO::Socket;

my $eit_ip = '192.168.1.200';

print "Setting Device parameters...\n";

my $browser = LWP::UserAgent->new;

my $response = $browser->post(
  'http://192.168.1.200/updatedevparam',
  [
    'unit_num' => '1',
    'id_num' => '1',
    'dev_name' => '01',
    'dev_addr' => '*01',
    'rdng_cmd' => 'X01',
    'setpt1_cmd' => 'R01',
    'setpt2_cmd' => 'R02',
    'setpt3_cmd' => 'R03',
    'setpt4_cmd' => 'R04',
    'resp_unit' => 'F',
    'disp_frmt' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/X01/) {
  print "Device setup successful...\n";
}else{
  print "Device setup failed...\n";
}


my $home_page = get("http://192.168.1.200/load?PAGENO=9");
die "Couldn't get it?!" unless $home_page;
my $count = 0;
$count++ while $home_page =~ m{4}gi;
print "$count\n";

exit;
