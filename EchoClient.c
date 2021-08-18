#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>


void exitSysWithError(const char *call){
	fprintf(stderr, "Syscall %s failed with errno= %d: %s\n", call, errno,
	 strerror(errno));
	exit(-1);
}
void tcpClient(struct sockaddr_in ina,char buff[]){
	printf("My TCP Client\n");
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s ==-1){
		exitSysWithError("socket()");
	}
	printf("Tcp socket created successfully\n");
	int c = connect (s, (struct sockaddr*)&ina, sizeof(ina));
	if (c==-1){
		exitSysWithError("connect()");
	}
	printf("TCP Connection is successful\n");
	int len = strlen(buff);
	write(s, buff, len);
	bzero(buff , len);
	int r = read(s, buff, len);
	if(r==-1){
		exitSysWithError("read()");
	}
	printf("From Server: %s\n",buff);
	int cls = close(s);
	if (cls==0){
		printf("TCP Connection Closed Successfully\n");
	}
	else {
		exitSysWithError("close()");
	}

}
void udpClient(struct sockaddr_in ina, char buff[]){
	printf("My UDP Client\n");
	char wrt[100];
	int n,r;
	int serverlen;
	struct sockaddr_in addr;
	int len = sizeof(addr);
	serverlen = sizeof(ina);
	int s = socket(AF_INET, SOCK_DGRAM,0);
	if(s==-1){
		exitSysWithError("socket()");
	}
	else
		printf("socket created successfully\n");
	n = sendto(s, buff, strlen(buff),0, (const struct sockaddr *)&ina , serverlen);
	if(n==-1){
		printf("Send to failed\n");
		exitSysWithError("sendto()");
	}
	else
		printf("Message sent\n");
	struct timeval tv;
	tv.tv_sec= 0;
	tv.tv_usec = 100000;
	if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv))< 0)
	{ printf("Error");
	}
	r = recvfrom(s,(char *)wrt, strlen(buff) ,0, (struct sockaddr *)&addr , &len);
	if (r==-1){
		exitSysWithError("recvfrom()");
	}
	else {
		wrt[r] = '\0';
		printf("Response:");puts(wrt);
	}
	
	int c = close(s);
	if (c == 0)
		printf("UDP Connection closed successfully\n");
	else
		exitSysWithError("close()");
}
void exitWithError(){
	printf("Please provide valid arguments\n");
	exit(-1);
}
int main(int argc, const char *argv[]){

	//myclient
	struct servent *sp;
	printf("Number of arguments=%d\n", argc);
	for(int i=0; i<argc; i++){
	  printf("argv[%d]=%s\n", i, argv[i]);
	} 
  //first argument will be filename to run the file on linux (Eg: ./a.out)
	if (argc != 4){
		exitWithError();
	}
	struct sockaddr_in ina;
	int slen = strlen(argv[1]);/Users/hellybhatt/Desktop/Virtualbox-Ubuntu/myclient.c
	int len = strlen(argv[3]);
	char por[len];
	char buff[slen];
	char *tcpport = "tcp";
	char *udpport = "udp";
	strcpy(por, argv[3]);
	strcpy(buff, argv[1]);
	char *pch = strstr(por, udpport);
	char *tch = strstr(por, tcpport);
	int r = inet_pton(AF_INET,argv[2], &(ina.sin_addr));
	if (r == 0){
		printf("Invalid conversion from text to IP address");
	}
	if (pch != NULL)
		sp = getservbyname("echo",udpport);
	else
		sp = getservbyname("echo",tcpport);
	if (sp == NULL)
		printf("Port# can not be fetched due to invalid parameters");
	memset(&ina, 0, sizeof(ina));
	ina.sin_family = AF_INET;
	ina.sin_addr.s_addr=inet_addr(argv[2]);
	ina.sin_port = htons(ntohs(sp->s_port));
	if (strcmp(argv[3], "-tcp") == 0){
		tcpClient(ina,buff);
	} 
  else if (strcmp(argv[3], "-udp") == 0){
		udpClient(ina,buff);
	}
  else{
		exitWithError();
	}
	exit(0);
}
