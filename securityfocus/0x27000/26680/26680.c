 #include <sys/types.h>
 #include <unistd.h>
 #include <pwd.h>
 #include <stdio.h>
 
int main() 
{
         struct passwd *pw;
         pw = getpwnam("abi");
         FILE *pipe;
         char buf[25];
         setgid(pw->pw_gid);
         setuid(pw->pw_uid);
 
         printf("my gid: %d\n", getegid());
         printf("my uid: %d\n", getuid());
 
         pipe = popen("/usr/bin/id", "r");
         while (fgets(buf, sizeof buf, pipe)) {
                 printf("%s", buf);
         }
         printf("\n");
         pclose(pipe);
 }