 /* 
 	File: 		serveru.c
	Version: 	Hangman UDP Server
	Modified by:	Samantha Marah
			Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	20171125	Added option to specify port as a command line parameter
	20171122	Added in Hangman.h functions, to make code more modular 
			(and easier to fit in screen shots)
			Created GetUserNameUDP() function to read in the Player name
	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
			Fixed warning messages by casting sockaddr_in to struct sockaddr*
			And moved HandleErrors to parent folder	
			Added random word selection	
*/

#include <stdio.h>
#include <string.h>			// strlen()
#include <stdlib.h>			// exit(), rand(), srand()
#include <time.h>			// time(), Seed the random number
#include "../DrawHangman.h"		// Draw graphics and add colour, needs to be included before Hangman.h
#include "../Hangman.h"			// Functions to play hangman
#include "../CreateUDPSocket.h"		// Functions to create and use UDP sockets
#include "../Socket.h"			// Socket functions intended for use with both TCP and UDP; displayHostname()
#include "../UDPPlayHangman.h"		// Functions to play hangman using sendto() and recvfrom() (include after Socket.h)


int main (int argc, char * argv []) {									// Option to specify port as a command line parameter
	int sock, count;										// Client socket, and byte count variables
	char buf[LINESIZE], username[LINESIZE];								// Buffer string, the username entered by the Client

	sock = createUDPServer((argc == 2) ? argv[1] : UDP_PORT);					// Create the UDP Server socket, with port specified, or default 1068

	drawHangmanLogo(UDP_SERVER);									// DrawHangman.h: Draw the game logo, with socket type label

	while(1) {
		fflush(stdout);										// flushes the output buffer of a stream
		srand(time(NULL));									// Randomise the seed, to get a different word each game
// RECEIVE USERNAME
		if((count = recvfrom(sock,username,LINESIZE,0,(struct sockaddr*) &cliAddr, &slen))==-1) {// Server receives 1st, recvfrom returns -1 if error
			//displayErrMsg("Username recvfrom() Failed");
			printf("No more input from %s%s%s\n",BLUE,username,NORM);			// Display end of input message
			break;										// Leave the loop without fuss or errors, if no more input is received
		} else {		
			username[count-1] = '\0';							// Terminate the end of the string (before the '\n' new line character)
			printf("Username received: %s%s%s\n",BLUE,username,NORM);			// Format and display the username
		}

		//if(getUserNameUDP(sock,username,cliAddr)) playHangmanUDP(sock,sock,username);		// UDPPlayHangman.h Play the game after receiving username FREEZES

		playHangmanUDP(sock,sock,username);

		printf("Finished Playing Hangman for %s%s%s\n",BLUE,username,NORM);			// Inform on server side the game has finished
		
		//shutdown(sock, SHUT_WR);
		close(sock);										// Close the socket connection
	}	

	return 0;											// End program
}
