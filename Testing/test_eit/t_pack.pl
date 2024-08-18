#!/usr/bin/perl -w

use strict;
use LWP::UserAgent;
use IO::Socket;

my $lport = '2000';
my $eit_ip = '192.168.1.200';
my $buffg_time = '1000';
my $end_char = '0x0d';

#First connect to the device.
my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/packing',
  [
    'E' => '0x0',
    'FW' => '0',
    'bt' => $buffg_time,
    'pl' => '0',
    'it' => '30',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value="1000"/) {
  print "Assigned Buffering time:", $buffg_time, " mseconds\n";
}else{
  print "Cannot assign Buffering time....\n";
}

my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock or die "no socket :$!\n";

open FILE, ">rdng.csv";
print FILE "Buffering time: 1000 mseconds\n";

my $i = 0;
for( $i = 0; $i < 10; $i++){
  print $sock "*X01\r\n";
  my $bytes_read = sysread ($sock, my($buf), 20);
  my $now_str = localtime;
  print $i,",";
  print $now_str,",";
  print $buf,"\n";

  print FILE $i,",";
  print FILE $now_str,",";
  print FILE $buf,"\n";
}
close FILE;
close $sock;

$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.200/packing',
  [
    'E' => $end_char,
    'FW' => '1',
    'bt' => $buffg_time,
    'pl' => '0',
    'it' => '30',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value="D"/) {
  print "Assigned end character:",$end_char,"\n";
}else{
  print "Cannot assign end character....\n";
}

$sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock or die "no socket :$!\n";

open FILE, ">>rdng.csv";
print FILE "Enabling character 0x0d\n";

$i = 0;
for( $i = 0; $i < 10; $i++){
  print $sock "*X01\r\n";
  my $bytes_read = sysread ($sock, my($buf), 20);
  my $now_str = localtime;
  print $i,",";
  print $now_str,",";
  print $buf,"\n";

  print FILE $i,",";
  print FILE $now_str,",";
  print FILE $buf,"\n";
}

close FILE;
close $sock;

$end_char = '0';
$buffg_time = '0'; 
my $pk_len = '10';


$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.200/packing',
  [
    'E' => $end_char,
    'FW' => '1',
    'bt' => $buffg_time,
    'pl' => $pk_len,
    'it' => '30',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value="8"/) {
  print "Assigned packet length:",$pk_len,"\n";
}else{
  print "Cannot assign packet length....\n";
}

$sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock or die "no socket :$!\n";

open FILE, ">>rdng.csv";
print FILE "Enabling packet length 10\n";

$i = 0;
for( $i = 0; $i < 10; $i++){
  print $sock "*X01\r\n";
  my $bytes_read = sysread ($sock, my($buf), 20);
  my $now_str = localtime;
  print $i,",";
  print $now_str,",";
  print $buf,"\n";

  print FILE $i,",";
  print FILE $now_str,",";
  print FILE $buf,"\n";
}

close FILE;
close $sock;
exit;

