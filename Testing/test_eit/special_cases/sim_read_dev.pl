#!/usr/bin/perl -w
# IO::Socket
use strict;
use IO::Socket;
my $host = shift || '128.100.101.116';
my $port = shift || 2000;
my $bytes_read = '0';
my $buf= 0;

my $sock = new IO::Socket::INET(
                   PeerAddr => $host,
                   PeerPort => $port,
                   Proto    => 'tcp');
$sock or die "no socket :$!";

for( my $i=0; $i < 8; $i++ ){ 
    print $sock "*01X01\r\n";
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    my $now_str = localtime;
    print $i,",";
    print $now_str,",";
    print $buf,"\n";	
}


print "\r\n";

close $sock;
