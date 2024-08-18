#!/usr/bin/perl -w
use strict;
use LWP::UserAgent;
use IO::Socket;

my $protocol = '1';
my $eit_ip = '192.168.1.201';
my $eit_msk = '255.255.255.0';
my $eit_gwy = '192.168.1.1';
my $eit_dns = '0.0.0.0';
my $eit_hostname = 'eitb96f';
my $eit_protocol = '1';
my $http_port = '800';

my $lport = '2000';

open FILE, ">network.log";

#First connect to the device.
my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/network',
  [
    #'dhcp' => '1',
    'ip' => $eit_ip,
    'msk' => $eit_msk,
    'gw' => $eit_gwy,
    'dns' => $eit_dns,
    'hname' => $eit_hostname,
    'protocol' => $eit_protocol,
    'port' => $http_port,
    'au' => '1',
    'spd1' => '1',
    'spd2' => '0',
    'dpx1' => '1',
    'dpx2' => '0',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value=1 selected="selected"> TCP/) {
  print FILE "Assigned Port as TCP, IP address: $eit_ip and http port: $http_port...\n";
  print "Assigned Port as TCP and IP address: $eit_ip and http port: $http_port...\n";
}else{
  print FILE "Cannot assign tcp port and IP address...\n";
  print "Cannot assign tcp port and IP address...\n";
}

sleep(5);

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

my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
$sock or die "no socket :$!\n";

print FILE "TCP protocol and IP: $eit_ip\n";

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
close $sock;


$eit_protocol = '2';

$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.201:800/network',
  [
    #'dhcp' => '1',
    'ip' => $eit_ip,
    'msk' => $eit_msk,
    'gw' => $eit_gwy,
    'dns' => $eit_dns,
    'hname' => $eit_hostname,
    'protocol' => $eit_protocol,
    'port' => $http_port,
    'au' => '1',
    'spd1' => '1',
    'spd2' => '0',
    'dpx1' => '1',
    'dpx2' => '0',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value=2 selected="selected"> UDP/) {
  print "Assigned Port as UDP...\n";
  print FILE "Assigned Port as UDP...\n";
}else{
  print "Cannot assign udp port...\n";
  print FILE "Cannot assign udp port...\n";
}

sleep(5);

$response = $browser->post(
  'http://192.168.1.201:800/reboot',
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
  print FILE "Device is rebooting...\n";
} 

print "Waiting for device to reboot...\n";
print FILE "Waiting for device to reboot...\n";
sleep(15);
print "Resuming...\n";
print FILE "Resuming...\n";

$sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'udp');
$sock or die "no socket :$!\n";

#open FILE, ">>network.log";
print FILE "UDP protocol\n";

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

$eit_ip = '192.168.1.200';
$eit_protocol = '1';
$http_port = '80';

#First connect to the device.
$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.201:800/network',
  [
    #'dhcp' => '1',
    'ip' => $eit_ip,
    'msk' => $eit_msk,
    'gw' => $eit_gwy,
    'dns' => $eit_dns,
    'hname' => $eit_hostname,
    'protocol' => $eit_protocol,
    'port' => $http_port,
    'au' => '1',
    'spd1' => '1',
    'spd2' => '0',
    'dpx1' => '1',
    'dpx2' => '0',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value=1 selected="selected"> TCP/) {
  print FILE "Reassigned Port as TCP and IP: $eit_ip and http port...\n";
  print "Reassigned Port as TCP and IP: $eit_ip and http port...\n";
}else{
  print FILE "Cannot reassign tcp port and IP: $eit_ip...\n";
  print "Cannot reassign tcp port and IP: $eit_ip...\n";
}

sleep(5);

$response = $browser->post(
  'http://192.168.1.201:800/reboot',
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

close FILE;
close $sock;

sleep(15);

