/*
  icadecrypt.c

  Decrypt stored Citrix ICA passwords (in appsrv.ini).

  Dug Song <dugsong@monkey.org>
*/

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int
hex_decode(char *src, u_char *dst, int outsize)
{
  char *p, *pe;
  u_char *q, *qe, ch, cl;
  
  pe = src + strlen(src);
  qe = dst + outsize;

  for (p = src, q = dst; p < pe && q < qe && isxdigit((int)*p); p += 2) {
    ch = tolower(p[0]);
    cl = tolower(p[1]);
    
    if ((ch >= '0') && (ch <= '9')) ch -= '0';
    else if ((ch >= 'a') && (ch <= 'f')) ch -= 'a' - 10;
    else return (-1);
    
    if ((cl >= '0') && (cl <= '9')) cl -= '0';
    else if ((cl >= 'a') && (cl <= 'f')) cl -= 'a' - 10;
    else return (-1);
    
    *q++ = (ch << 4) | cl;
  }
  return (q - dst);
}

int
ica_decrypt(u_char *pass, int len)
{
  u_short i;
  u_char *p, key;

  if (len < 4)
    return (0);

  i = ntohs(*(u_short *)pass);
  
  if (i != len - 2)
    return (0);
  
  key = pass[2];
  p = pass + 3;
  
  for (i -= 2; i > 0; i--)
    p[i] = p[i - 1] ^ p[i] ^ key;

  p[0] ^= (key | 'C');
  
  i = len - 3;
  memmove(pass, pass + 3, i);
  pass[i] = '\0';
  
  return (1);
}

void
usage(void)
{
  fprintf(stderr, "Usage: icadecrypt <file>\n");
  exit(1);
}

int
main(int argc, char *argv[])
{
  FILE *f;
  u_char line[1024], pass[128];
  int len;

  if (argc != 2 || *argv[1] == '-')
    usage();

  if ((f = fopen(argv[1], "r")) == NULL) {
    perror("fopen");
    exit(1);
  }
  while (fgets(line, sizeof(line), f) != NULL) {
    if (strncmp(line, "Password=", 9) == 0) {
      len = hex_decode(line + 9, pass, sizeof(pass));
      if (ica_decrypt(pass, len))
        printf("; icadecrypt: [%s]\n", pass);
    }
    printf("%s", line);
  }
  fclose(f);

  exit(0);
}

/* 5000. */
