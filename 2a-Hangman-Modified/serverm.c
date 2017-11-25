 /* 
 	File: 		serverm.c
	Version: 	Modified version of Hangman Server
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Network server for hangman game

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	24/11/2017	Added option to specify port the server is running on
	18/11/2017	Moved play_hangman functionality to TCPPlayHangman.h
			Tidied up Modified Server and Client
			Moved functionality to display Client IP/Port to CreateTCPSocket.h displayNameAndPort()
	23/09/2017 	Added includes to get rid of compile warnings
			Client IP and Port number is displayed when a connection is made
			Win/Lose message displayed, and sent to Client to display
			Added hangman graphic
*/
// Joe O'Regan K00203642
#include <stdio.h>								// printf(), sprintf()
#include <string.h>								// strcpy(), strlen()
#include <stdlib.h>								// exit()
#include <unistd.h>								// gethostname(), write(), read(), close()
#include "../DrawHangman.h"							// Functions to draw the games graphics
#include "../Hangman.h"								// Functions to play the game
#include "../CreateTCPSocket.h"							// Functions to handle socket creation and connection
#include "../Socket.h"								// Socket functions common to, or shared by TCP and UDP sockets
#include "../TCPPlayHangman.h"							// Moved hang_man() function to separate file

extern time_t time();								// Time used to seed random numbers	

int main (int argc, char * argv []) {						// Option to specify port as a command line parameter
	struct sockaddr_storage client;						// Hold IPv4 or IPv6 address structure
 	int sock, fd, client_len;						// Client socket, server socket, size of client
	char cliName[INET6_ADDRSTRLEN];						// Client address string
	socklen_t len = sizeof client;						// Size of the address stucture
	int cliPort;								// Client port number

 	srand((int) time ((long *) 0)); 					// Randomise the seed, so a new word is displayed each game

	//Interchangeable dualstack and IPv4 socket creation
	// Formats Address as IPv6:
	sock = createDualStackServerSocket((argc == 2) ? argv[1] : TCP_PORT);	// CreateTCPSocket.h: Create & bind the TCP socket, dual stack IPv4/IPv6
	// Formats Address as IPv4:
	//sock = createTCPServerSocket((argc == 2) ? argv[1] : TCP_PORT);		// Create & bind the TCP socket, before proceeding to accept connections,
										// with an option to enter the port number, or leave blank for default
	drawHangmanNew();							// DrawHangman.h: Draw the hangman graphic in colour

 	while (1) {								// Infinite Loop, as soon as one Client finishes game, another can play
 		client_len = sizeof(client);
 		if((fd=accept(sock,(struct sockaddr*)&client,&client_len))<0)	// Create the listening socket, and if its return value is less than 0
			displayErrMsgStatus("Accepting Connection", 3);		// Display error message, and exit with return status 3

		displayIPAndPort(&client, len, cliName, &sock, &cliPort);	// CreateTCPSocket.h: Display the Client IP and Port number
							
 		playHangmanTCP(fd, fd, cliName, cliPort);			// TCPPlayHangman.h: Play the game

 		close(fd);							// Close the client socket
 	}
}
