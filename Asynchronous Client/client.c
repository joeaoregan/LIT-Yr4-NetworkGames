// Joe O'Regan 17/11/2017
// 17/11/2017 Asynchronous TCP Client using select()  - strcliselect02.h P228

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>											// printf()
#include <errno.h>
#include <string.h>											// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>											// 23/09/2017 Warning for exit
#include <unistd.h>											// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>											// 23/09/2017 inet_ntop()
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../DrawHangman.h"
#include "../CreateTCPSocket.h"
	
void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {
	int sockfd, maxfdp1, stdineof, count;
	fd_set rset;

	struct sockaddr_in servaddr = createTCPClientSocket(&sockfd, (argc == 1) ?  SRV_IP : argv[1]);	// Make the socket connection, with default localhost, or entered IP address
	char buf[LINESIZE];
	stdineof = 0;
	FD_ZERO(&rset);											// Clear the set

	drawHangman();											// Display game logo
	
	count = read (sockfd, buf, LINESIZE);								// Read 1st (empty) part word from server
	write (1, buf, count);										// Display on screen

	for ( ; ; ) {											// Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);					// FD_SET - add stdin to file descriptor list
		FD_SET(sockfd, &rset);									// Add socket to file descriptor list
		maxfdp1 = max(fileno(stdin), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {								// socket is readable, FD_ISSET = check socket is in file descriptor list
			if ( (count = read(sockfd, buf, LINESIZE)) == 0) {				// Get input from socket
				if (stdineof == 1) break;						// normal termination, EOF already received
				else displayErrMsg("str_cli: server terminated prematurely");
			}

			write(fileno(stdout), buf, count);						// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  							// input is readable, check standard input is in list of file descriptors
			if ( (count = read(fileno(stdin), buf, LINESIZE)) == 0) {			// Get input from keyboard, and if none
				stdineof = 1;								// EOF received, set stdineof flag, call shutdown, and send FIN
				shutdown(sockfd, SHUT_WR);						// send FIN, SHUT_WR = close write half of connection
				FD_CLR(fileno(stdin), &rset);						// Remove standard input from the file descriptors list
				continue;								// Skip the write to server socket, and loop again
			}

			write(sockfd, buf, count);							// Otherwise, Send input to socket
		}
	}

	exit(0);											// Terminate the program, terminating open descriptors
}
