#!/usr/bin/perl -w
# IO::Socket
use strict;
use IO::Socket;
my $host = shift || '192.168.1.200';
my $port = shift || 2000;
my $bytes_read = '0';
my $buf = 0;
my $no_of_rdng = 5000;
my $interval_sec = 5; 
my $close_socket = 0;
my $model = "ei";
my $i = 0;
my @cmd;


if( $model eq "ei" ){
  @cmd = ("*X01\r\n", "*X02\r\n", "*X03\r\n");
}elsif( $model eq "c4ei" ){
  @cmd = ("*01X01\r\n", "*01X02\r\n", "*01X03\r\n");
}

for( $i=0; $i < $no_of_rdng; $i++ ){ 
my $sock = new IO::Socket::INET(
                   PeerAddr => $host,
                   PeerPort => $port,
                   Proto    => 'tcp');
$sock or die "no socket :$!";

    open FILE,">>ilog.csv";    

    print $sock $cmd[0];
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    my $now_str = localtime;
    print $i,",";
    print $now_str,",";
    print $buf,",";
    
    print FILE $i,",";
    print FILE $now_str,",";
    print FILE $buf,",";

    print $sock $cmd[1];
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    print $buf,",";
    print FILE $buf,",";
    
    print $sock $cmd[2];
    $bytes_read = sysread ($sock, $buf, 20);
    chop($buf);
    print $buf,"\n";
    print FILE $buf,"\n";
	
    sleep( $interval_sec );

  close FILE;
  if( $close_socket eq 1 ){
    close $sock;
  }
}
