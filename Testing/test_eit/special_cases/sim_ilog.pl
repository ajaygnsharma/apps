#!/usr/bin/perl -w
# IO::Socket
use strict;
use IO::Socket;
my $host = shift || '128.100.101.113';
my $port = shift || 2000;
my $bytes_read = '0';
my $buf= 0;

my $sock = new IO::Socket::INET(
                   PeerAddr => $host,
                   PeerPort => $port,
                   Proto    => 'tcp');
$sock or die "no socket :$!";

open FILE,">>ilog.csv";

for( my $i=0; $i < 100; $i++ ){ 
    print $sock "*X01\r\n";
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    my $now_str = localtime;
    print $i,",";
    print $now_str,",";
    print $buf,",";
    print FILE $i,",";
    print FILE $now_str,",";
    print FILE $buf,",";
    
    print $sock "*X02\r\n";
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    print $buf,",";
    print FILE $buf,",";
    
    print $sock "*X03\r\n";
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    print $buf,"\n";
    print FILE $buf,"\n";

    sleep( 3 );
}

close FILE;
print "\r\n";

close $sock;
