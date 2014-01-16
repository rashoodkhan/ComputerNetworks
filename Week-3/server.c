#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_MAX	4096
#define MAX_CLIENTS	10

static void connect(int);
static void disconnect(int);

int pid_clients[MAX_CLIENTS];
int fd_read[MAX_CLIENTS];
int fd_write[MAX_CLIENTS];
static int current_client=0;
int pid_server;
char pid_server_string[10];

int main(void){
	pid_server = getpid();
	sprintf(pid_server_string,"%d",pid_server);

	if(signal(SIGUSR1, connect) == SIG_ERR)
		printf("Can't catch SIGUSR1\n");
	if(signal(SIGUSR2, disconnect) == SIG_ERR)
		printf("Can't catch SIGUSR2\n");

	for(;;)
		pause();
}

static void connect(int sig_no){
	if(sig_no == SIGUSR1){
		if(current_client >= MAX_CLIENTS){
			printf("The server room is full. Please try again later\n");
			return;
		}

		int temp;
		printf("Please Enter the Client Process ID - ");
		scanf("%d\n",&temp);

		pid_clients[current_client] = temp;
		char str[10];
		sprintf(str,"%d",temp);

		mkfifo(str,0666);
		mkfifo(pid_server_string,0666);

		int temp_fd_read = open(str,O_RDONLY);
		int temp_fd_write = open(pid_server_string,O_WRONLY);

		fd_read[current_client]	= temp_fd_read;
		fd_write[current_client] = temp_fd_write;

		current_client++;
	}
}

static void disconnect(int sig_no){
	if(sig_no == SIGUSR2){
		if(current_client<0){
			printf("There are no clients in the room.\n");
			return;
		}
		current_client--;
		close(fd_read[current_client]);
		close(fd_write[current_client]);

		printf("The %d client is disconnected from server\n",pid_clients[current_client]);
	}
}