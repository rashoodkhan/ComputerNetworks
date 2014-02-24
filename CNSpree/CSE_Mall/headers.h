/*
* ------------------------------------------------------------------ *
|																	 |
|	This file contains all the header files needed for Server        |
|	connections.													 |
|																	 |
|	Author	:	Rashid Ahmed Khan									 |
|																	 |
|																	 |
* ------------------------------------------------------------------ *
*/

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define MAXLINE 512
//Reads n bytes from the fd
int readn(int fd,char *ptr,int nbytes){
	int nleft, nread;

	nleft = nbytes;
	while(nleft > 0){
		nread = read(fd,ptr,nleft);
		if (nread < 0)
			return nread;
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}
	return (nbytes - nleft);
}

int writen(int fd, char *ptr, int nbytes){
	int nleft,nwritten;

	nleft = nbytes;
	while (nleft > 0) {
		nwritten = write(fd,ptr,nleft);
		if (nwritten <= 0)
			return (nwritten);

		nleft -= nwritten;
		ptr += nwritten;
	}
	return (nbytes - nleft);
}

int readline(int fd, char *buf, int maxlen){
	int n, rc;
	char c;
 
	for (n = 1; n < maxlen; n++) {
		if ((rc = read(fd, &c, 1)) == 1) {
			*buf++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
				if (n == 1)
					return 0;
				else
					break;
		} else
			return -1;
	}
 
	*buf = '\0';
	return n;
}

//Error catching method
void error(char *error){
	printf("%s\n", error);
	exit(1);
}

void error_(char *error,int i){
	printf("%s %d\n",error, i);
	exit(1);
}