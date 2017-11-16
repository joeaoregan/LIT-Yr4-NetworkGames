#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>											// printf()
#include <errno.h>
#include <string.h>											// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>											// 23/09/2017 Warning for exit
#include <unistd.h>											// 23/09/2017 gethostname(), wgccrite(), read(), close()
#include "../CreateTCPServer.h"										// Create a TCP Server Socket

#define	LISTENQ		1024										/* 2nd argument to listen() */
#define	MAXLINE		4096										/* max text line length */
char* SERV_PORT = "9877";										/* TCP and UDP */

int main(int argc, char **argv) {
	int 			i, maxi, maxfd, listenfd, connfd, sockfd, nready, client[FD_SETSIZE];
	ssize_t			n;
	fd_set			rset, allset;
	char			buf[MAXLINE];
	socklen_t		clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = createTCPServerSocket(SERV_PORT);							// Create the socket

	maxfd = listenfd;										/* initialize */
	maxi = -1;											/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;										/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset;										/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {							/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;						/* save descriptor */
					break;
				}
			if (i == FD_SETSIZE) printf("too many clients");				//err_quit("too many clients");

			FD_SET(connfd, &allset);							/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;						/* for select */
			if (i > maxi) maxi = i;								/* max index in client[] array */

			if (--nready <= 0) continue;							/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {								/* check all clients for data */
			if ( (sockfd = client[i]) < 0) continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, buf, MAXLINE)) == 0) {				/* connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else
					write(sockfd, buf, n);

				if (--nready <= 0) break;						/* no more readable descriptors */
			}
		}
	}
}
