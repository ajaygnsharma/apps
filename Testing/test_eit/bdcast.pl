#!/usr/bin/perl -w
#---------------------------------------------------------------
# GR Perl Mongers
# Jeff Williams
# 11/23/1999
# Sockets: Simple UDP client - can not use a telnet to talk directly
# to server process so we use this client
#---------------------------------------------------------------

use IO::Socket;
use strict;

my($sock, $server_host, $msg, $port, $ipaddr, 
   $MAXLEN, $server_port, $local_port, $TIMEOUT);

#-- Define the port to listen on
$server_port = 7011;

#-- Listening on this PC. Must be changed for a real host
#$server_host = "128.100.101.123";
$server_host = inet_ntoa(INADDR_BROADCAST);

$local_port = 7011;


#$server_host = shift;   #<-???????

#-- Maximum message length - corresponds to value on server
$MAXLEN  = 1024;

#-- Since we are getting a message from the server
#-- we have a timeout in case the expected datagram is not sent
$TIMEOUT = 180;

#-- Grab what we type as a parameter and send to the server
$msg         = "AFFIRM\n";

#-- Build the socket and bind to port
$sock = IO::Socket::INET->new(Proto     => 'udp',
			      LocalPort => $local_port,
                              PeerPort  => $server_port,
                              PeerAddr  => $server_host,
			      Broadcast => 1)
    or die "Creating socket: $!\n";

#-- Send a msg to the server    
$sock->send($msg) or die "send: $!";

close $sock;




#Now start recieving udp packets.
my($datagram, $MAX_TO_READ, $server, $oldmsg, $hishost);

#-- Maximum message size of datagram
$MAX_TO_READ = 1024;

$server = IO::Socket::INET->new(LocalPort => $server_port,
                                Proto     => "udp")
       or die "Couldn't be a udp server on port $server_port : $@\n";


print "Host List\n\n";

#-- Now go into a loop receiving messages:
while( $server->recv($datagram, $MAX_TO_READ ) ){
  eval{
    ($port, $ipaddr) = sockaddr_in($server->peername);
    my $client_ip = inet_ntoa($ipaddr);
    
    local $SIG{ALRM} = sub { die "alarm time out" };
    alarm 2;  
    
    #my @split_data = split( /\d+/, $datagram ); 
    print "$client_ip\t$datagram\n";
    #print "ip: $client_ip";
    
    #foreach my $val( @split_data ){
    #  print "$val";
    #}
    #print "\n";
  
  } or die "recv from timeout after 2 seconds \n";
}
print "Server terminated.\n"

