/* 	
	Joe O'Regan K00203642
	17/11/2017

	cliAsync.c

	Asynchronous UDP Client

	20171117	Asynchronous UDP client sends and receives, and terminates when finished
	20171116	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115	Fixed client continuing to connect, 
 	  		add command line parameter check
*/

#include <stdio.h>											// printf(), sscanf()
#include <string.h>											// strlen(), strcmp()
#include <stdlib.h>											// rand(), exit()
#include "../DrawHangman.h"										// Function do draw hangman graphics (needs to be included before Hangman.h"
#include "../Hangman.h"
#include "../CreateUDPSocket.h"

 int main (int argc, char * argv []) {									// Added arguments to specify an IP address or default localhost/127.0.0.1
	struct sockaddr_in si_other;
	int sock, maxfdp1, stdineof, slen=sizeof(si_other);
	fd_set rset;
	char partword[LINESIZE], userInput[LINESIZE];
	ssize_t byteCount;

	sock = createUDPClient((argc == 1) ? SRV_IP : argv[1]);						// Creat the socket
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);					// 

	drawHangmanLogo(ASYNC_UDP_CLI);									// Display game logo, with socket title

/*INPUT USERNAME*/
	printf("Please Enter Your Username: ");
	fgets(userInput, LINESIZE, stdin);								// Enter username		
/*SEND USERNAME*/
	if (sendto(sock, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto() Failed");	
		
	stdineof = 0;
	FD_ZERO(&rset);											// Clear the set

	while ( partword[0] != 'b' ) {									// Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);					// FD_SET - add stdin to file descriptor list
		FD_SET(sock, &rset);									// Add socket to file descriptor list
		maxfdp1 = (fileno(stdin) > sock ? fileno(stdin) : sock) + 1;				// Choose the biggest of the 2 and add 1 to it
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sock, &rset)) {								// socket is readable, FD_ISSET = check socket is in file descriptor list
/* RECV PART */		if ((byteCount = recvfrom(sock, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == 0) {	// recvfrom returns 0 if closed by other side
				if (stdineof == 1) break;						// normal termination, EOF already received
				else displayErrMsg("str_cli: server terminated prematurely");
			}
			partword[byteCount] = '\0';							// Add string terminating character
			write(fileno(stdout), partword, byteCount);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  							// input is readable, check standard input is in list of file descriptors
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
