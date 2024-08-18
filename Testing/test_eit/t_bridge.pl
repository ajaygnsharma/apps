#!/usr/bin/perl -w

use strict;
use LWP::UserAgent;
use IO::Socket;

my $lport = '3000';
my $eit_ip = '192.168.1.200';

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/bridge',
  [
    #'N' => '1', #No: of connection = 0
    'N' => '2', #No: of connection = 1
    'lp' => $lport,
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

open FILE, ">bridge.log";

if($response->content =~ m/value="3000"/) {
  print "Assigned port number...\n";
  print FILE "Assigned port number...\n";
}else{
  print "Cannot assign the port number..\n";
  print FILE "Cannot assign the port number..\n";
}



$response = $browser->post(
  'http://192.168.1.200/reboot',
  [
    'none' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;
 
if($response->content =~ m/Device Rebooting/) {
  print "Device is rebooting...\n";
  print FILE "Device is rebooting...\n";
}else{
  print "Some problem...\n";
  print FILE "Some problem...\n";
} 

print "Waiting for device to reboot...\n";
print FILE "Waiting for device to reboot...\n";
sleep(15);
print "Resuming...\n";
print FILE "Resuming...\n";

use IO::Socket;

my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock or die "\nno socket :$!\n";

print "Socket 1...Sending cmd: *X01\n";
print FILE "Socket 1...Sending cmd: *X01\n";
print $sock "*X01\r\n";
my $bytes_read = sysread ($sock, my($buf), 20);
chop($buf);
print "Reply:", $buf, "\n";
print FILE "Reply:", $buf, "\n";

print "Sleeping for 2 seconds...\n";
print FILE "Sleeping for 2 seconds...\n";
sleep(2);

print "Socket 2...\n";
print FILE "Socket 2...\n";
my $sock1 = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock1 or print "Cannot make second connection...\n$!";

print "Sleeping for 2 seconds...\n";
print FILE "Sleeping for 2 seconds...\n";
sleep(2);

print "Socket 3...\n";
print FILE "Socket 3...\n";
my $sock2 = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock2 or print "Cannot make third connection...\n$!";

print "Sleeping for 2 seconds...\n";
print FILE "Sleeping for 2 seconds...\n";
sleep(2);

print "Socket 4...\n";
print FILE "Socket 4...\n";
my $sock3 = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock3 or print "Cannot make fourth connection...\n$!";

print "Sleeping for 2 seconds...\n";
print FILE "Sleeping for 2 seconds...\n";
sleep(2);

print "Socket 5...\n";
print FILE "Socket 5...\n";
my $sock4 = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock4 or print "Cannot make fifth connection...$!\n";


print "Sleeping for 2 seconds...\n";
print FILE "Sleeping for 2 seconds...\n";
sleep(2);

print FILE "Socket 6...\n";
my $sock5 = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock5 or print "Cannot make sixth connection...$!\n";


$lport = '2000';
$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.200/bridge',
  [
    #'N' => '1', #No: of connection = 0
    'N' => '6', #No: of connection = 1
    'lp' => $lport,
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;


if($response->content =~ m/value="2000"/) {
  print "Assigned port number...\n";
  print FILE "Assigned port number...\n";
}else{
  print "Cannot assign the port number..\n";
  print FILE "Cannot assign the port number..\n";
}



$response = $browser->post(
  'http://192.168.1.200/reboot',
  [
    'none' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;
 
if($response->content =~ m/Device Rebooting/) {
  print "Device is rebooting...\n";
  print FILE "Device is rebooting...\n";
}else{
  print "Some problem...\n";
  print FILE "Some problem...\n";
} 

print "Waiting for device to reboot...\n";
print FILE "Waiting for device to reboot...\n";
sleep(15);
print "Resuming...\n";
print FILE "Resuming...\n";

close FILE;
exit;
