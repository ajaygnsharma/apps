#!/usr/bin/perl
#install the module: libnet-smtp-server-perl
#Note must be root user to run this program
use strict;
use LWP::UserAgent;
use IO::Socket;
use Net::SMTP::Server;
use Net::SMTP::Server::Client;
use Data::Dumper;
use threads;

my $alarm_to_test = "trap_power_reset";
my $smtp_subj = 'power_reset';
my $smtp_svr_ip = '192.168.1.101';

my $pr_msg = 'Power_reset';
my $ipc_msg = 'IP address changed';
my $isa_msg = 'iServer accessed';
my $ch_msg = 'Char 1 or 2 found';

#my $test_post_email = 0;
my $test_e_pr = 0;
my $test_e_ipc = 0;
my $test_e_isa = 0;
my $test_e_c = 0;
my $test_t_pr = 0;
my $test_e_all = 1;

my $eit_ip = '192.168.1.200';
my $eit_msk = '255.255.255.0';
my $eit_gwy = '192.168.1.1';
my $eit_dns = '0.0.0.0';
my $eit_hostname = 'eitb96f';
my $eit_protocol = '1';
my $http_port = '80';
my $lport = '2000';
my $new_eit_ip = '192.168.1.201';

my $browser;
my $response;

########################################################################
# Defaults 
########################################################################
sub defaults{
  
  $response = $browser->post(
    "http://".$eit_ip."/rdef",
    [
      'none' => '1',
    ],
  );
  die "Error: ", $response->status_line
   unless $response->is_success;
  
  print "Restored defaults\n";   
}

########################################################################
# Network page setup
########################################################################
sub s_network_page{
  $browser = LWP::UserAgent->new;
  $response = $browser->post(
  "http://".$eit_ip."/network",
  [
    'ip' => $new_eit_ip,
    'msk' => $eit_msk,
    'gw' => $eit_gwy,
    'dns' => $eit_dns,
    'hname' => $eit_hostname,
    'protocol' => $eit_protocol,
    'port' => $http_port,
  ],
);
die "Error: ", $response->status_line
 unless $response->is_success;

if($response->content =~ m/value=1 selected="selected"> TCP/) {
  print FILE "Assigned Port as TCP, IP address: $new_eit_ip and http port: $http_port...\n";
  print "Assigned Port as TCP and IP address: $new_eit_ip and http port: $http_port...\n";
}else{
  print FILE "Cannot assign tcp port and IP address...\n";
  print "Cannot assign tcp port and IP address...\n";
}
}

########################################################################
# Email page setup
########################################################################
sub s_email_page{
  my $smtp_from = 'eit';
  my $a1_ri = '1';
  my $a1_to = 'ajay';

  #First connect to the device.
  $browser = LWP::UserAgent->new;
  $response = $browser->post(
    "http://".$eit_ip."/smtpsnmp",
    [
      'smtp' => '1',
      'smtpip' => $smtp_svr_ip,
      'from' => $smtp_from,
      'sub' => $smtp_subj,
      'a1_ri' => $a1_ri,
      'a1_to' => $a1_to
    ],
  );
  die "Error: ", $response->status_line
   unless $response->is_success;

  if($response->content =~ m/$smtp_svr_ip/) {
    print "Assigned SMTP server ip:", $smtp_svr_ip, "\n";
  }else{
    print "Cannot assign SMTP server ip....\n";
  }
}

########################################################################
# Trap page setup
########################################################################
sub s_trap_page{
  $browser = LWP::UserAgent->new;
  $response = $browser->post(
    "http://".$eit_ip."/smtpsnmp",
    [
      'snmp' => '1',
      'c_str' => 'Public',
      'Contact' => 'None',
      'Location' => 'None',
      'trap' => '1',
      'trap_ip' => '128.100.101.113'
    ],
  );
  die "Error: ", $response->status_line
   unless $response->is_success;

  if($response->content =~ m/"128.100.101.113"/) {
    print "Assigned SNMP server ip\n";
  }else{
    print "Cannot assign SNMP server ip....\n";
  }
}

########################################################################
# Alarm page setup
########################################################################
sub s_alarm_page{
  
  $browser = LWP::UserAgent->new;
  if( $alarm_to_test eq "power_reset" ){
	  $response = $browser->post("http://".$eit_ip."/alarm1",['a1_e_pr_chk' => '0']);    
  }elsif( $alarm_to_test eq "ip_changed" ){
	  $response = $browser->post("http://".$eit_ip."/alarm1",['a1_e_ipc_chk' => '0']);
  }elsif( $alarm_to_test eq "iserver_accessed" ){
	  $response = $browser->post("http://".$eit_ip."/alarm1",['a1_e_isa_chk' => '0']);
  }elsif( $alarm_to_test eq "character" ){
	  $response = $browser->post("http://".$eit_ip."/alarm1",['a1_e_c_chk' => '0', 'a1_c1' => '0x0d',]);
  }elsif( $alarm_to_test eq "trap_power_reset" ){
	  $response = $browser->post("http://".$eit_ip."/alarm1",['a1_t_pr_chk' => '0']);
  }elsif( $alarm_to_test eq "all" ){
  	$response = $browser->post(
	    "http://".$eit_ip."/alarm1",
	    ['a1_e_pr_chk' => '0',
	     'a1_e_ipc_chk' => '0',
	     'a1_e_isa_chk' => '0',
	     'a1_e_c_chk' => '0', 
	     'pr_msg' => $pr_msg,
	     'ipc_msg' => $ipc_msg,
	     'isa_msg' => $isa_msg,
	     'ch_msg' => $ch_msg,
	     'a1_c1' => '0x0d',
	     'a1_c2' => '0x0',
	    ]);
  }
  
  die "Error: ", $response->status_line
    unless $response->is_success;

  if($response->content =~ m/"checked"/) {
    print "Enabled Alarm\n";
  }else{
    print "Cannot enable Alarm\n";
  }
}

########################################################################
# Reboot 
########################################################################
sub reboot{
  $response = $browser->post(
    "http://".$eit_ip."/reboot",
    [
      'none' => '1',
    ],
  );
  die "Error: ", $response->status_line
   unless $response->is_success;
  if($response->content =~ m/Device Rebooting/) {
    print "Device is rebooting\n";
  }else{
    print "Some problem\n";
  } 
}

########################################################################
# start smtp server
########################################################################
sub smtp_server{
  my $count_num_of_email;
  our $host = $ARGV[0] || "0.0.0.0" ;

  our $server = new Net::SMTP::Server($host) || die("Unable to open SMTP server socket");
  
  print "Waiting for incoming SMTP connections on ".($host eq "0.0.0.0" ? "all IP addresses":$host)."\n";
  $| = 1;

  while(my $conn = $server->accept()){
    #eval{
    print "Incoming mail ... from " . $conn->peerhost() ;
    my $client = new Net::SMTP::Server::Client($conn) || die("Unable to process incoming request");
    if (!$client->process) { 
      print "\nfailed to receive complete e-mail message\n"; next; 
    }
  
    $count_num_of_email++;
    print " received\n";
    print "From: $client->{FROM}\n";
    my $to = $client->{TO};
    my  $toList = @{$to} ? join(",",@{$to}) : $to;
    print "To:   $toList\n";
    print "\n" ;
    print $client->{MSG};
	  
    #alarm 70;
     #local $SIG{ALRM} = sub { die "alarm time out\n" };
    #local $SIG{ALRM} = sub { print "alarm time out\n" };
    #}or die "recv from timeout after 40 seconds\n";
    #}or print "recv from timeout after 40 seconds\n";
    if( $count_num_of_email > 4 ){
    	last;
    }
    #$client->quit;
    #$server->quit; 
  }
}

########################################################################
# Print trap info
########################################################################
sub print_trap ($$) {
   my ( $this, $trap, $community, $ent, $agent, $gen, $spec, $dt, @bindings, $encoded_pair, $prefix, $oid, $value );
   
   ($this, $trap) = @_;
   ($community, $ent, $agent, $gen, $spec, $dt, @bindings) = \
    $this->decode_trap_request ($trap);
   print "   Community:\t".$community."\n";
   print "   Enterprise:\t".BER::pretty_oid ($ent)."\n";
#    print "   Agent addr:\t".inet_ntoa($agent)."\n";
    print "   Generic ID:\t$gen\n";
    print "   Specific ID:\t$spec\n";
    print "   Uptime:\t".BER::pretty_uptime_value ($dt)."\n";
    $prefix = "   bindings:\t";
    foreach $encoded_pair (@bindings){
        ($oid, $value) = decode_by_template ($encoded_pair, "%{%O%@");
        #next unless defined $oid;
        print $prefix.BER::pretty_oid ($oid)." => ".pretty_print ($value)."\n";
        $prefix = "  ";
    }
}

########################################################################
# start SNMP server
########################################################################
sub snmp_server{
  use SNMP_Session "0.60";
  use BER;
  use Socket;
 
  my $session = SNMPv1_Session->open_trap_session();
  my $trap;
  my $sender;
  my $sender_port;
  my $DATE;
  
  while (($trap, $sender, $sender_port) = $session->receive_trap()){
    chomp ($DATE=`/bin/date \'+%a %b %e %T\'`);
    print STDERR "$DATE - " . inet_ntoa($sender) . " - port: $sender_port\n";
    print_trap ($session, $trap);
    last;
  }
  1;

}

########################################################################
# Send command to the unit
########################################################################
sub send_cmd{
  
  my $sock = new IO::Socket::INET(
                   PeerAddr => $eit_ip,
                   PeerPort => $lport,
                   Proto    => 'tcp');
  $sock or die "\nno socket :$!\n";

  print "Cmd: *X01\n";
  print $sock "*X01\r\n";
  my $bytes_read = sysread ($sock, my($buf), 20);
  chop($buf);
  print "Reply:", $buf, "\n";

}


########################################################################
# Run main script
########################################################################
if( $test_e_pr eq 1 ){
  $smtp_subj = 'power_reset';	
  &s_email_page;
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(1);
  print "Resuming...\n";
  &smtp_server;
  &defaults;
  &reboot;
}

if( $test_e_ipc eq 1 ){
  $alarm_to_test = "ip_changed";
  $smtp_subj = 'ip_changed';
  &s_email_page;  
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(10);
  print "Resuming...\n";
  print "Setting a different IP\n";
  &s_network_page;
  &smtp_server;
  &defaults;
  &reboot;
}


if( $test_e_isa eq 1 ){
  $alarm_to_test = "iserver_accessed";
  $smtp_subj = 'iserver_accessed';
  &s_email_page;  
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(10);
  print "Resuming...\n";
  &send_cmd;
  &smtp_server;
  &defaults;
  &reboot;
}


if( $test_e_c eq 1 ){
  $alarm_to_test = "character";
  $smtp_subj = 'character alarm';
  &s_email_page;  
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(10);
  print "Resuming...\n";
  &send_cmd;
  &smtp_server;
  &defaults;
  &reboot;
}

if( $test_t_pr eq 1 ){
  &s_trap_page;
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(1);
  print "Resuming...\n";
  &snmp_server;
  &defaults;
  &reboot;
}

sub t1{
  print "In Thread1...\n";
  &smtp_server;  
}


sub t2{
  print "Thread2 sleeping for 30 seconds...\n";
  sleep(30);
  
  print "Setting a different IP\n";
  $new_eit_ip = '192.168.1.201';
  &s_network_page;  
  $new_eit_ip = '192.168.1.200';
  &s_network_page;
}

sub t3{
  print "Thread3 sleeping for 45 seconds...\n";
  sleep(45);
  &send_cmd;
}


########################################################################
# Perform all email test.
########################################################################
if( $test_e_all eq 1 ){
  $alarm_to_test = "all";
  $smtp_subj = 'all alarms test';
  &s_email_page;
  &s_alarm_page;
  &reboot;
  print "Waiting for device to reboot...\n";
  sleep(10);
  print "Resuming...\n";
  
  my ($thr1) = threads->create(\&t1);
  my ($thr2) = threads->create(\&t2);
  my ($thr3) = threads->create(\&t3);
  
  my @ret_val1 = $thr1->join();
  my @ret_val2 = $thr2->join();
  my @ret_val3 = $thr3->join();
  #&smtp_server;
  
  &defaults;
  &reboot;
}

