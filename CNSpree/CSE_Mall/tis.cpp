#include "headers.h"
#include <sys/select.h>

//Ports for listening to clients
const int PORTS[] = {11100,11110,11120};

/*
	Port 12000 serves as verification Port

	Port 13000 serves are termination Port - After a connection is received on this,
	the server cannot accept any more connections as corresponding servicing servers
	are full with load.
*/
const int SERV_PORTS[] = {12000,13000};

#define PORT_C 15000
#define PORT_S 16000
#define PORT_E 17000

#define MAXLINE 512
#define MAXBUF	4096
#define CLI_FD 	3
#define MAX_T	50

//-------------Global Variables---------------
//Current ticket counter
int t_issued=0;

//Tickets counter - keeps track of what all tickets have been issued
int tickets[MAX_T] = {0};

//Sockets g1,g2,g3 - on which the server is listening for clients
int g_sfd[3],service_sfd[2];
struct sockaddr_in clients[3], g_addr[3],service[2], serv_cli;
socklen_t clilen[3];

bool serversAreFull = false;

struct fd_set read_fd;
struct timeval timeout;
//--------------Function Prototypes------------
int getTicket();
void init();
void _listen();
int getMaxFD();
void refreshSelect();
bool verifyTicket(int);
void setLimit();
void service_(int);

int main(int argc, char *argv[]){
	init();
	_listen();
	char ticketMessage[32];
	char read_buf[MAXBUF];
	int i;
	int sfd;
	for(;;){
		refreshSelect();
		int maxfd = getMaxFD();
		bool isClient = true;
		if (select(maxfd+1,&read_fd,NULL,NULL,NULL)){
			for(i=0;i<CLI_FD;i++){
				if(FD_ISSET(g_sfd[i],&read_fd)){
					sfd = g_sfd[i];
					break;
				}
				if(FD_ISSET(service_sfd[i],&read_fd)){
					service_(service_sfd[i]);
					isClient = false;
					break;
				}
			}
		}
		if(isClient){
			clilen[i] = sizeof(clients[i]);
			int nsfd = accept(sfd,(struct sockaddr *) &clients[i], &clilen[i]);
			char *buf;
			bzero(ticketMessage, 32);
			bzero(read_buf,MAXBUF);
			//strcpy(buf,"Which mall do you want to connect to?");
			buf = "Which mall do you want to connect to?";
			int len = strlen(buf);
			send(nsfd, buf, len, 0);

			int n;
			n = recv(nsfd,read_buf,MAXBUF,0);
			printf("The client wants to connect to - %s\n",read_buf);

			int ticket = getTicket();
			sprintf(ticketMessage, "%d:%d ", PORT_C, ticket);
			send(nsfd, ticketMessage, strlen(ticketMessage), 0);
			close(nsfd);
		}	
	}
}

void service_(int sfd){
	socklen_t cli_len = sizeof(serv_cli);
	int nsfd = accept(sfd,(struct sockaddr *) &serv_cli, &cli_len);
	char ticketBuffer[MAXLINE];
	int n = recv(nsfd,ticketBuffer,MAXLINE,0);
	int t_id = atoi(ticketBuffer);
	printf("ticket id - %d\n",t_id);
	bool check = verifyTicket(t_id);

	if(check)
		send(nsfd,"ACCEPT",6,0);
	else
		send(nsfd,"REJECT",6,0);

	close(nsfd);

	return;
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
		if (bind (g_sfd[i],(struct sockaddr *)&g_addr[i], sizeof(g_addr[i])) < 0)
			error_("Bind Error at ",i);

		printf("Client Ports have been initialized\n");
	}

	for(i=0;i<2;i++){
		if((service_sfd[i] = socket(AF_INET,SOCK_STREAM,0)) < 0){
			error("Socket error at service");
		}

		bzero((char *) &service[i], sizeof(service[i]));
		service[i].sin_family = AF_INET;
		service[i].sin_addr.s_addr = htonl(INADDR_ANY);
		service[i].sin_port = htons(SERV_PORTS[i]);

		if(bind(service_sfd[i], (struct sockaddr *) &service[i], sizeof(service[i])) < 0){
			error_("Bind Error at Service",i);
		}

		printf("Service ports have been initialized\n");
	}
}

void refreshSelect(){
	FD_ZERO(&read_fd);
	int i=0;
	for(;i<CLI_FD;i++)
		FD_SET(g_sfd[i],&read_fd);
	for(i=0;i<2;i++)
		FD_SET(service_sfd[i], &read_fd);
}

void _listen(){
	int i=0;
	for(i=0;i<CLI_FD;i++){
		listen(g_sfd[i],5);
	}
	for(i = 0;i<2;i++){
		listen(service_sfd[i],5);
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

	if(service_sfd[0] > maxfd)
		maxfd = service_sfd[0];

	if(service_sfd[1] > maxfd)
		maxfd = service_sfd[1];

	return maxfd;
}

bool verifyTicket(int tick){
	bool check = false;
	int i = 0;
	for(;i<t_issued;i++){
		if (tickets[i] == tick)
			return true;
	}
	return check;
}

void setLimit(){
	serversAreFull = true;
}

void unsetLimit(){
	serversAreFull = false;
}