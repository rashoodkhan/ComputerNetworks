#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
	printf("Process p2 has started\n");
	pid_t pid;
	int status;

	if((pid = fork()) < 0) {
		printf("Fork Error\n");
	}else if(pid == 0) {
		execlp("/Users/rashid/Documents/Computer Networks/Assignments/Week-1/prog3","/Users/rashid/Documents/Computer Networks/Assignments/Week-1/prog3",(char *) 0);
	}

	if((pid = waitpid(pid,&status,0)) < 0) {
		printf("Waitpid error\n");
	}

	exit(0);
}