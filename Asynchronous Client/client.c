// Joe O'Regan 17/11/2017
// 17/11/2017 Asynchronous TCP Client using select()  - strcliselect02.h P228

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>										// printf()
#include <errno.h>
#include <string.h>										// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>										// 23/09/2017 Warning for exit
#include <unistd.h>										// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>										// 23/09/2017 inet_ntop()
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../CreateTCPSocket.h"
	
void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr = createTCPClientSocket(&sockfd, (argc == 1) ?  SRV_IP : argv[1]);
/*
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(TCP_PORT_NUM);
	inet_pton(AF_INET, (argc == 1) ?  SRV_IP : argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	printf("Client Connecting On Port: %d \n", ntohs(servaddr.sin_port));			// Display port number
*/

//	str_cli(stdin, sockfd);									// do it all

	//exit(0);
//}

//void str_cli(FILE *fp, int sockfd) {
	int maxfdp1, stdineof, count;
	fd_set rset;
	char buf[LINESIZE];

	stdineof = 0;
	FD_ZERO(&rset);


	count = read (sockfd, buf, LINESIZE);							// Read from server
	write (1, buf, count);									// Display on screen

	for ( ; ; ) {										// loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(stdin), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {							// socket is readable
			if ( (count = read(sockfd, buf, LINESIZE)) == 0) {			// Read from socket
				//if (stdineof == 1) return;					// normal termination
				if (stdineof == 1) break;					// normal termination
				else displayErrMsg("str_cli: server terminated prematurely");
			}

			write(fileno(stdout), buf, count);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  						// input is readable
			if ( (count = read(fileno(stdin), buf, LINESIZE)) == 0) {
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);					// send FIN
				FD_CLR(fileno(stdin), &rset);
				continue;
			}

			write(sockfd, buf, count);						// Send to socket
		}
	}
	exit(0);
}
