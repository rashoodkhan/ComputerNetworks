#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_MAX 4096

int pid_server;
int pid_self;

int main(int argc, char *argv[]){
	pid_self = getpid();
	if(argc != 2){
		printf("Usage : ./client <server_pid>\n");
	}
	pid_server = atoi(argv[1]);

	printf("Trying to Connect to Server %d......\n",pid_server);

	kill(pid_server,SIGUSR1);

	printf("Wow.. Connected to Server\n");

	kill(pid_server, SIGUSR2);

	return 0;
}