#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>									// printf()
#include <errno.h>
#include <string.h>									// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>									// 23/09/2017 Warning for exit
#include <unistd.h>									// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>									// 23/09/2017 inet_ntop()
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../CreateTCPSocket.h"
	
#define	max(a,b)	((a) > (b) ? (a) : (b))

//void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
static ssize_t my_read(int fd, char *ptr);

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(TCP_PORT_NUM);
	inet_pton(AF_INET, (argc == 1) ?  SRV_IP : argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	printf("Client Connecting On Port: %d \n", ntohs(servaddr.sin_port));		// Display port number

	str_cli(stdin, sockfd);								/* do it all */

	exit(0);
}

/*
void str_cli(FILE *fp, int sockfd) {
	char	sendline[LINESIZE], recvline[LINESIZE];

	while (fgets(sendline, LINESIZE, fp) != NULL) {

		write(sockfd, sendline, strlen(sendline));

		if (read(sockfd, recvline, LINESIZE) == 0) displayErrMsg("str_cli: server terminated prematurely");

		fputs(recvline, stdout);
	}
}*/

void str_cli(FILE *fp, int sockfd) {
	int n, maxfdp1, stdineof;
	fd_set rset;
	char buf[LINESIZE];

	stdineof = 0;
	FD_ZERO(&rset);

	for ( ; ; ) {
		if (stdineof == 0) FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		//Select(maxfdp1, &rset, NULL, NULL, NULL);
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {						/* socket is readable */
			//if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
			if ( (n = read(sockfd, buf, LINESIZE)) == 0) {
				if (stdineof == 1) return;				/* normal termination */
				//else err_quit("str_cli: server terminated prematurely");
				else displayErrMsg("str_cli: server terminated prematurely");
			}

			//Write(fileno(stdout), buf, n);
			write(fileno(stdout), buf, n);
		}

		if (FD_ISSET(fileno(fp), &rset)) {  					/* input is readable */
			//if ( (n = Read(fileno(fp), buf, MAXLINE)) == 0) {
			if ( (n = read(fileno(fp), buf, LINESIZE)) == 0) {
				stdineof = 1;
				//Shutdown(sockfd, SHUT_WR);				/* send FIN */
				shutdown(sockfd, SHUT_WR);				/* send FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			//Writen(sockfd, buf, n);
			write(sockfd, buf, n);
		}
	}
}
