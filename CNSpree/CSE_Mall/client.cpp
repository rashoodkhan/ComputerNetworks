#include "headers.h"

#define MAXLINE	512

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *hp;
	char *host="locahost";
	hp = gethostbyname(host);

	int port = atoi(argv[1]);
	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
	serv_addr.sin_port = htons(port);

	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		error("Socket Error");

	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Connect Error");

	char buf[MAXLINE];

	bzero(buf,MAXLINE);
	printf("Connected to server\n");
	int n = read(sockfd,buf,MAXLINE);
	printf("%s\n", buf);

	bzero(buf,MAXLINE);
	scanf("%s",buf);
	printf("You have decided to connect to %s\n",buf);
	send(sockfd,buf,sizeof(buf),0);
	
	bzero(buf,MAXLINE);
	n = readline(sockfd,buf,MAXLINE);
	string msg = buf;
	int hash = msg.find(":");
	printf("%s %d\n",buf,hash);
	close(sockfd);
}