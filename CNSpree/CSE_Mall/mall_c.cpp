#include "headers.h"


#define IN_PORT	15000	//Incoming Port for clients to connect
#define TIS_VER 12000	//TIS Verfication port
#define TIS_CNL 13000	//TIS Cancellation port
	
#define PORT_S	16000	//Port of Server S

#define MAXLINE	512
#define MAX_CLI	5

#define BLOCK 1
#define VERFY 2
//---------------Global Variables------------
int numConnected;

struct sockaddr_in myaddr, cliaddr, serv_addr;
struct hostent *hp;


int clilen;
int serv_sfd, cli_sfd;
//---------------Function Prototypes----------
void serv_init();
void cli_init(char*,int);

int main(int argc, char *argv[]){
	serv_init();

	char message[MAXLINE];
	for(;;){
		clilen = sizeof(cliaddr);
		int nsfd = accept(serv_sfd,(struct sockaddr *) &cliaddr, &clilen);
		if(numConnected < MAX_CLI){
			bzero(message,MAXLINE);
			int n = recv(nsfd,message,MAXLINE,0);
			int c = fork();
			if(c == 0){
				close(serv_sfd);
				cli_init("localhost",VERFY);

				send(cli_sfd,message,n,0);
				bzero(message,MAXLINE)
				n = recv(cli_sfd,message,MAXLINE,0);

				int check = strcmp("ACCEPT",message);
				
				close(cli_sfd);

				if(check == 0){
					for(;;){
						//The Ticket is valid - hence serve here
						send(nsfd,"Serving You, Type QUIT to Exit",33,0);
						n = recv(nsfd,message,MAXLINE,0);
						check = strcmp("QUIT",message);
						if (check == 0){
							close(nsfd);
							break;
						}
					}
				}
				else{
					//The ticket provided is wrong
					send(nsfd,"Wrong Ticket - Terminating Connection",50,0);
					close(nsfd);
				}
			}

			numConnected++;
		}else{
			//Code when the server is full
			bzero(message,MAXLINE);
			sprintf(message,"%d",PORT_S);
			send(nsfd,message,strlen(message),0);
			close(nsfd);
		}
	}
}

void serv_init(){
	if((serv_sfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		error("Socket Error on Server C");
	}

	bzero((char *) &myaddr, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(IN_PORT);

	if(bind(serv_sfd,(struct sockaddr *) &myaddr, sizeof(myaddr)) < 0)
		error("Binding error on Server C");

	listen(serv_sfd,MAX_CLI);
}

void cli_init(char *hostname,int block){
	hp = gethostname(hostname);

	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
	
	if(block == 1)
		serv_addr.sin_port = htons(TIS_CNL);

	else serv_addr.sin_port = htons(TIS_VER);

	if((cli_sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		error("Socket Error on Client C");

	if(connect(cli_sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error_("Connection error when trying to connect to TIS on block - ",block);
}

