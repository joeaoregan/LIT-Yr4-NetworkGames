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

	sockfd = createUDPClient((argc == 1) ? SRV_IP : argv[1]);
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);
/*INPUT USERNAME*/
	printf("Please Enter Your Username: ");
	fgets(userInput, LINESIZE, stdin);								// Enter username		
/*SEND USERNAME*/
	if (sendto(sockfd, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto() Failed");	
	

printf("test: %s", userInput);	// XXX OK

/*	userInput[0] = '\0';
	if ((count = recvfrom(sockfd, userInput, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == 0) displayErrMsg("recvfrom() Failed");

	write(fileno(stdout), userInput, count);
	printf("test 3\n");
*/
	stdineof = 0;
	FD_ZERO(&rset);											// Clear the set
//	drawHangman();											// Display game logo

	for ( ; ; ) {											// Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);					// FD_SET - add stdin to file descriptor list
		FD_SET(sockfd, &rset);									// Add socket to file descriptor list
		maxfdp1 = (fileno(stdin) > sockfd ? fileno(stdin) : sockfd) + 1;			// Choose the biggest of the 2 and add 1 to it
		select(maxfdp1, &rset, NULL, NULL, NULL);

		partword[0]='\0';

		if (FD_ISSET(sockfd, &rset)) {								// socket is readable, FD_ISSET = check socket is in file descriptor list
			//if ( (count = read(sockfd, userInput, LINESIZE)) == 0) {			// Get input from socket
/* RECV PART */		if ((byteCount = recvfrom(sockfd, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == 0) {
				if (stdineof == 1) break;						// normal termination, EOF already received
				else displayErrMsg("str_cli: server terminated prematurely");
			}

			//partword[byteCount-1] = '\0';
			write(fileno(stdout), partword, byteCount);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  							// input is readable, check standard input is in list of file descriptors
/*INPUT GUESS*/		if ( (byteCount = read(fileno(stdin), userInput, LINESIZE)) == 0) {			// Get input from keyboard, and if none
				stdineof = 1;								// EOF received, set stdineof flag, call shutdown, and send FIN
				shutdown(sockfd, SHUT_WR);						// send FIN, SHUT_WR = close write half of connection
				FD_CLR(fileno(stdin), &rset);						// Remove standard input from the file descriptors list
				continue;								// Skip the write to server socket, and loop again
			}
printf("test invalid argument");
			//write(sockfd, buf, count);							// Otherwise, Send input to socket
/*SEND GUESS*/		if (sendto(sockfd, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1)
				 displayErrMsg("sendto() Failed");
		}
	}

	printf("Client Finished\n");

	exit(0);											// Terminate the program, terminating open descriptors
}


/*
// PLAY THE GAME
	while(partword[0] != 'b') {					// change to check lives, as this will exit on word beginning with b
// RECEIVE THE PART WORD
		partword[0]='\0';					// Reset the part-word string

		byteCount = recvfrom(sockfd, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);	 	
		partword[byteCount-1] = '\0'; 				// bytecount -1 to remove \n or \r or whatever is giving new line		
		printf("Received:  %s", partword);			// Display the string received from the server	

// SEND GUESS
		fgets(userInput, LINESIZE, stdin);

		//if (sendto(s, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) {
		//	 displayErrMsg("sendto() Failed");
		//}							
	}
*/


// receive part word	
//	count = recvfrom(sockfd, userInput, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);		// Read 1st (empty) part word from server
//if ( (count = read(fileno(stdin), userInput, LINESIZE)) == 0) displayErrMsg("stdin");
//			write(fileno(stdout), userInput, count);					// Write to screen

//printf("test1");
