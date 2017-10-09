#!/usr/bin/perl
use Socket;

####test arguments
if ($#ARGV != 2) {die "usage: DNS_name/IP file_to_get port\n";}
#####load values
$host = @ARGV[0];$port = @ARGV[2];$target = inet_aton($host);$toget= @ARGV[1];
#####build request
$xtosend=<<EOT
GET /$toget\\ HTTP/1.0
Host: $host
User-Agent: SensePostData
Content-Type: application/x-www-form-urlencoded
Translate: f

EOT
;
$xtosend=~s/\n/\r\n/g;
####send request
#print $xtosend;
my @results=sendraw($xtosend);
print  @results;
#### Sendraw - thanx RFP rfp@wiretrip.net
sub sendraw {   # this saves the whole transaction anyway
        my ($pstr)=@_;
        socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) ||
                die("Socket problems\n");
        if(connect(S,pack "SnA4x8",2,$port,$target)){
                my @in;
                select(S);      $|=1;   print $pstr;
                while(<S>){ push @in, $_;
                        print STDOUT "." if(defined $args{X});}
                select(STDOUT); close(S); return @in;
        } else { die("Can't connect...\n"); }
}

