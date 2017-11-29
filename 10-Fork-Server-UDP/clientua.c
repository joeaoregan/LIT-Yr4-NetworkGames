/* 	
	Joe O'Regan K00203642
	17/11/2017

	TEST PURPOSES: clientua.c

	Asynchronous UDP Client
*/

#include <stdio.h>											// printf(), sscanf()
#include <string.h>											// strlen(), strcmp()
#include <stdlib.h>											// rand(), exit()
#include "../DrawHangman.h"										// Function do draw hangman graphics (needs to be included before Hangman.h"
#include "../Hangman.h"											// Functions to play Hangman game
#include "../CreateUDPSocket.h"										// Functions to create and handle UDP Client and Server sockets

 int main (int argc, char * argv []) {									// Added arguments to specify an IP address or default localhost/127.0.0.1
	struct sockaddr_in si_other;									// Client address structure
	int sock, maxfdp1, stdineof, slen=sizeof(si_other);						// Client socket, file descriptor amount, select on stdin, client address size
	fd_set rset;											// List of descriptors
	char partword[LINESIZE], userInput[LINESIZE];							// Input and output buffers
	ssize_t byteCount;										// Number of bytes to send/receive

	sock = createUDPClient((argc == 2) ? argv[1]: SRV_IP);						// Create the IPv4 UDP socket
	si_other = getServerAddress((argc == 2) ? argv[1]: SRV_IP, 					// CreateUDPSocket.h: Set up the address stucture for sending data to the server
			(argc == 3) ? argv[2] : UDP_PORT);						// Enter IP address/port from command line, or use defaults

	drawHangmanLogo(ASYNC_UDP_CLI);									// Display game logo, with socket title

/*INPUT USERNAME*/
	printf("Please Enter Your Username: ");
	fgets(userInput, LINESIZE, stdin);								// Enter username		
/*SEND USERNAME*/
	if (sendto(sock, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto() Failed");	
		
	stdineof = 0;											// Flag to select on standard input, each loop
	FD_ZERO(&rset);											// Clear the discriptor list

	while ( partword[0] != 'b' ) {									// Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);					// FD_SET - add stdin to file descriptor list
		FD_SET(sock, &rset);									// Add socket to file descriptor list
		maxfdp1 = (fileno(stdin) > sock ? fileno(stdin) : sock) + 1;				// Choose the biggest of the 2 and add 1 to it
		select(maxfdp1, &rset, NULL, NULL, NULL);						// Get the minimum of the 2 descriptors using select() and use it next
		
		if (FD_ISSET(sock, &rset)) {								// socket is readable, FD_ISSET = check socket is in file descriptor list
/* RECV PART */		if ((byteCount = recvfrom(sock, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == 0) {	// recvfrom returns 0 if closed by other side
				if (stdineof == 1) break;						// normal termination, EOF already received
				else displayErrMsg("str_cli: server terminated prematurely");		// HandleErrors.h: dispaly error if server closes connection (recv () return 0)
			}
			partword[byteCount] = '\0';							// Add string terminating character
			write(fileno(stdout), partword, byteCount);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  							// Input is readable, check standard input is in list of file descriptors
/*INPUT GUESS*/		if ((byteCount = read(fileno(stdin), userInput, LINESIZE)) == 0) {		// Get input from keyboard, and if none
				stdineof = 1;								// EOF received, set stdineof flag, call shutdown, and send FIN
				shutdown(sock, SHUT_WR);						// send FIN, SHUT_WR = close write half of connection
				FD_CLR(fileno(stdin), &rset);						// Remove standard input from the file descriptors list
				continue;								// Skip the write to server socket, and loop again
			}
			userInput[1]='\0';								// Terminate the userInput string
/*SEND GUESS*/		if (sendto(sock, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1)
				 displayErrMsg("sendto() Failed");					// sendto() returns -1 if there is an error
		}
	}

	printf("Client Finished\n");
	close(sock);											// Close the socket
	exit(0);											// Terminate the program, terminating open descriptors
}
