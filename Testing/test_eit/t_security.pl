#!/usr/bin/perl -w

use strict;
use LWP::Simple;
use LWP::UserAgent;
use IO::Socket;

my $test_telnet_console = 0;
my $test_webserver_access = 1;

my $telnet_port = '2002';
my $eit_ip = '192.168.1.200';

if( $test_telnet_console eq 1 ){
print "Disabling telnet console...\n";

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/security',
  [
    #'ica' => '1',
    'wsa' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/name=ica value="1" checked/) {
  print "Telnet console enabled...\n";
}else{
  print "Telnet console is not enabled...\n";
}





print "Waiting for 5 seconds before opening connection to port 2002\n";
sleep(5);

use IO::Socket;

my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $telnet_port,
                   Proto    => 'tcp');
$sock or print "Cannot open connection to Telnet console:\n $!";
#$sock or die "Cannot open connection to Telnet console:\n $!";



print "Enabling telnet console...\n";

$browser = LWP::UserAgent->new;
$response = $browser->post(
  'http://192.168.1.200/security',
  [
    'ica' => '1',
    'wsa' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/name=ica value="1" checked/) {
  print "Telnet console enabled...\n";
}else{
  print "Telnet console is not enabled...\n";
}


print "Waiting for 5 seconds before opening connection to port 2002\n";
sleep(5);

use IO::Socket;

$sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $telnet_port,
                   Proto    => 'tcp');

$sock or die "Cannot open connection to Telnet console:\n $!";

print "Connection opened..\n";


}



if($test_webserver_access eq 1){

#Test Webserver access
my $security_page = get("http://192.168.1.200/security.htm");
print "Cannot open security page..." unless $security_page;

print "Webserver is enabled, Opened security page...\n\n";

if( $security_page =~ m{name=wsa value="1" checked}gi ){
  print "Webserver accessed is enabled...\n";
}


print "Disabling webserver access...\n";

my $browser = LWP::UserAgent->new;
my $response = $browser->post(
  'http://192.168.1.200/security',
  [
    'ica' => '1',
    #'wsa' => '1',
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/name=wsa value="1" checked/) {
  print "Webserver access is enabled...\n";
}else{
  print "Webserver access is not enabled...\n";
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
}else{
  print "Some problem...\n";
} 

print "Waiting for device to reboot...\n";
sleep(15);
print "Resuming...\n";


$security_page = get("http://192.168.1.200/security.htm");
print "Cannot open security page..." unless $security_page;

print "Opened security page...\n\n";

print "Enabling webserver access from telnet console...\n";

my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $telnet_port,
                   Proto    => 'tcp');
$sock or die "\nno socket :$!\n";


my $bytes_read = sysread ($sock, my($buf), 25);
if( $buf eq "Administrator Password?" ){
  print $sock "00000000\r\n"
}

$bytes_read = sysread ($sock, $buf, 20);
if( $buf eq "EIT>" ){
  print $sock "s -WS1\r\n"
}

$bytes_read = sysread ($sock, $buf, 20);
if( $buf eq "CMD_OK\r\nEIT>" ){
  print $sock "c\r\n"
}

$bytes_read = sysread ($sock, $buf, 20);
if( $buf eq "SAVE_OK\r\nEIT>" ){
  print $sock "RESET\r\n"
}

$bytes_read = sysread ($sock, $buf, 50);
if( $buf eq "PLEASE_WAIT_15_SECONDS_FOR_DEVICE_TO_REBOOT\r\n" ){
  print "done\n";
}

print "Waiting for device to reboot...\n";
sleep(15);
print "Resuming...\n";

$security_page = get("http://192.168.1.200/security.htm");
print "Cannot open security page..." unless $security_page;

print "Opened security page...\n\n";

}
exit;
