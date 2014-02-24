#include "headers.h"
#include <sys/select.h>

//Ports for listening to clients
const int PORTS[] = {11000,11010,11020};

#define G1 9000
#define G2 9010
#define G3 9020

#define MAXLINE 512
#define MAXBUF	4096
#define CLI_FD 	3
#define MAX_T	50

//-------------Global Variables---------------
int t_issued=0;
int tickets[MAX_T] = {0};

int g_sfd[3];
struct sockaddr_in clients[3], g_addr[3];
int clilen[3];

struct fd_set read_fd;
struct timeval timeout;
//--------------Function Prototypes------------
int getTicket();
void init();
void _listen();
int getMaxFD();
void refreshSelect();

int main(int argc, char *argv[]){
	init();
	_listen();
	/*int sockfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv;
	bzero ((char *) &serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port = htons(G1);
	bind(sockfd,(struct sockaddr *) &serv, sizeof(serv));
	listen(sockfd,5);
	clilen[0] = sizeof(clients[0]);
	int asfd = accept(sockfd,(struct sockaddr *) &clients[0], &clilen[0]);
	write(asfd,"Hello World",11);
	return 0;
	*/
	char ticketMessage[32];
	char read_buf[MAXBUF];
	int i;
	int sfd;
	for(;;){
		refreshSelect();
		int maxfd = getMaxFD();
		if (select(maxfd+1,&read_fd,NULL,NULL,NULL)){
			for(i=0;i<CLI_FD;i++){
				if(FD_ISSET(g_sfd[i],&read_fd)){
					sfd = g_sfd[i];
					break;
				}
			}
		}
		clilen[i] = sizeof(clients[i]);
		int nsfd = accept(sfd,(struct sockaddr *) &clients[i], &clilen[i]);
		char *buf;
		bzero(ticketMessage, 32);
		bzero(read_buf,MAXBUF);
		buf = "Which mall do you want to connect to?";
		int len = strlen(buf);
		send(nsfd, buf, len, 0);

		int n;
		n = recv(nsfd,read_buf,MAXBUF,0);
		printf("The client wants to connect to - %s\n",read_buf);
		
		int ticket = getTicket();
		sprintf(ticketMessage, "Your Ticket Number is - %d ", ticket);
		send(nsfd, ticketMessage, strlen(ticketMessage), 0);
		close(nsfd);	
	}
}

void init(){
	int i;
	for(i=0;i<CLI_FD;i++){
		if ((g_sfd[i] = socket (AF_INET, SOCK_STREAM, 0)) < 0){
			error ("Socket error");
		}

		bzero ((char *) &g_addr[i], sizeof(g_addr[i]));
		g_addr[i].sin_family = AF_INET;
		g_addr[i].sin_addr.s_addr = htonl(INADDR_ANY);
		g_addr[i].sin_port = htons(PORTS[i]);
		printf("sfd - %d , Binding to port - %d\n",g_sfd[i], PORTS[i]);
		if (bind (g_sfd[i],(struct sockaddr *)&g_addr[i], sizeof(g_addr[i])) < 0)
			error_("Bind Error at ",i);
	}
}

void refreshSelect(){
	FD_ZERO(&read_fd);
	int i=0;
	for(;i<CLI_FD;i++)
		FD_SET(g_sfd[i],&read_fd);
}

void _listen(){
	int i=0;
	for(i=0;i<CLI_FD;i++){
		listen(g_sfd[i],5);
	}
}

int getTicket(){
	tickets[t_issued] = t_issued;
	t_issued++;
	return tickets[t_issued - 1];
}

int getMaxFD(){
	int maxfd;
	if(g_sfd[0] > g_sfd[1])
		maxfd = g_sfd[0];
	else maxfd = g_sfd[1];

	if(g_sfd[2] > maxfd)
		maxfd = g_sfd[2];

	return maxfd;
}