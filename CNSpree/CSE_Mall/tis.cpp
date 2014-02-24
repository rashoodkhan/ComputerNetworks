#include "headers.h"
#include <sys/select.h>

//Ports for listening to clients
const int PORTS[] = {11000,11010,11020};

/*
	Port 12000 serves as verification Port

	Port 15000 serves are termination Port - After a connection is received on this,
	the server cannot accept any more connections as corresponding servicing servers
	are full with load.
*/
const int SERV_PORTS[] = {12000,15000};

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