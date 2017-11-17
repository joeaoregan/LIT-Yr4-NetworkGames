// 20171116 CreateUDPSocket.h - abstracts the UDP socket creation code
// 20171115 Fixed client continuing to connect, 
// 		add command line parameter check
// 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../DrawHangman.h"
#include "../CreateUDPSocket.h"

 int main (int argc, char * argv []) {									// Added arguments to specify another IP address other than the default localhost/127.0.0.1
	struct sockaddr_in si_other;
	int sockfd, maxfdp1, stdineof;
	fd_set rset;
	int slen=sizeof(si_other);
	char partword[LINESIZE];
	char userInput[LINESIZE];
	ssize_t byteCount;
	int count;

	sockfd = createUDPClient((argc == 1) ? SRV_IP : argv[1]);
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);

//	drawHangman();											// Display game logo

/*INPUT USERNAME*/
	printf("Please Enter Your Username: ");
	fgets(userInput, LINESIZE, stdin);								// Enter username		
/*SEND USERNAME*/
	if (sendto(sockfd, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto() Failed");	
	
	userInput[0] = '\0';
	
	stdineof = 0;
	FD_ZERO(&rset);											// Clear the set

	for ( ; ; ) {											// Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);					// FD_SET - add stdin to file descriptor list
		FD_SET(sockfd, &rset);									// Add socket to file descriptor list
		maxfdp1 = (fileno(stdin) > sockfd ? fileno(stdin) : sockfd) + 1;			// Choose the biggest of the 2 and add 1 to it
		select(maxfdp1, &rset, NULL, NULL, NULL);

//		partword[0]='\0';

		if (FD_ISSET(sockfd, &rset)) {								// socket is readable, FD_ISSET = check socket is in file descriptor list
/* RECV PART */		if ((byteCount = recvfrom(sockfd, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == 0) {	// recvfrom returns 0 if closed by other side
				if (stdineof == 1) break;						// normal termination, EOF already received
				else displayErrMsg("str_cli: server terminated prematurely");
			}
//			printf("Recv partword bytecount = %ld", byteCount);
			partword[byteCount] = '\0';
			write(fileno(stdout), partword, byteCount);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  							// input is readable, check standard input is in list of file descriptors
/*INPUT GUESS*/		if ((count = read(fileno(stdin), userInput, LINESIZE)) == 0) {		// Get input from keyboard, and if none
				stdineof = 1;								// EOF received, set stdineof flag, call shutdown, and send FIN
				shutdown(sockfd, SHUT_WR);						// send FIN, SHUT_WR = close write half of connection
				FD_CLR(fileno(stdin), &rset);						// Remove standard input from the file descriptors list
				continue;								// Skip the write to server socket, and loop again
			}
			userInput[1]='\0';
			//printf("read stdin bytecount: %ld",byteCount);
			//write(sockfd, buf, count);							// Otherwise, Send input to socket
/*SEND GUESS*/		if (sendto(sockfd, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1)
/*SEND GUESS*/		//if (sendto(sockfd, userInput, byteCount, 0,(struct sockaddr *) &si_other, slen) == -1)
				 displayErrMsg("sendto() Failed");
		}
	}

	printf("Client Finished\n");

	exit(0);											// Terminate the program, terminating open descriptors
}
