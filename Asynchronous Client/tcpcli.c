#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>								// printf()
#include <errno.h>
#include <string.h>								// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>								// 23/09/2017 Warning for exit
#include <unistd.h>								// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>								// 23/09/2017 inet_ntop()
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../CreateTCPSocket.h"
	
void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
static ssize_t my_read(int fd, char *ptr);
//ssize_t readline(int fd, void *vptr, size_t maxlen);

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(TCP_PORT_NUM);
	inet_pton(AF_INET, (argc == 1) ?  SRV_IP : argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	printf("Client Connecting On Port: %d \n", ntohs(servaddr.sin_port));	// Display port number

	str_cli(stdin, sockfd);							/* do it all */

	exit(0);
}


void str_cli(FILE *fp, int sockfd) {
	char	sendline[LINESIZE], recvline[LINESIZE];

	while (fgets(sendline, LINESIZE, fp) != NULL) {

		write(sockfd, sendline, strlen(sendline));

		//if (readline(sockfd, recvline, LINESIZE) == 0) displayErrMsg("str_cli: server terminated prematurely");
		if (read(sockfd, recvline, LINESIZE) == 0) displayErrMsg("str_cli: server terminated prematurely");

		fputs(recvline, stdout);
	}
}

/*
static int	read_cnt;
static char	*read_ptr;
static char	read_buf[LINESIZE];

static ssize_t my_read(int fd, char *ptr) {
	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR) goto again;
			return(-1);
		} else if (read_cnt == 0) return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n') break;		// newline is stored, like fgets()
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);			// EOF, n - 1 bytes were read
		} else return(-1);			// error, errno set by read()
	}

	*ptr = 0;					// null terminate like fgets()
	return(n);
}
*/
