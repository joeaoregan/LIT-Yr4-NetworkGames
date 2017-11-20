 /* 
 	File: 		serverm.c
	Version: 	Modified version of Hangman Server
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Network server for hangman game

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	18/11/2017	Moved play_hangman functionality to TCPPlayHangman.h
			Tidied up Modified Server and Client
			Moved functionality to display Client IP/Port to CreateTCPSocket.h displayNameAndPort()
	23/09/2017 	Added includes to get rid of compile warnings
			Client IP and Port number is displayed when a connection is made
			Win/Lose message displayed, and sent to Client to display
			Added hangman graphic
*/

#include <stdio.h>									// printf(), sprintf()
#include <string.h>									// 23/09/2017 strcpy(), strlen()
#include <stdlib.h>									// 23/09/2017 exit()
#include <unistd.h>									// 23/09/2017 gethostname(), write(), read(), close()
#include "../DrawHangman.h"								// Functions to draw the games graphics
#include "../Hangman.h"									// Functions to play the game
#include "../CreateTCPSocket.h"								// 16/11/2017 Functions to handle socket creation and connection
#include "../TCPPlayHangman.h"								// 18/11/2017 Moved hang_man() function to separate file

extern time_t time();									// Time used to seed random numbers	
struct sockaddr_in client;								// Made global to display client name outside main
#define CLI_PORT ntohs(client.sin_port)							

void main () {										// No command line arguments
 	int sock, fd, client_len;							// Client socket, server socket, size of client
	char cliName[INET_ADDRSTRLEN];							// Client address string

 	srand ((int) time ((long *) 0)); 						// Randomise the seed

	//sock = createTCPServerSocket();						// Create and bind the TCP socket
	sock = createDualStackServerSocket();						// Create and bind the TCP socket, with dual stack IPv4 and IPv6 support

	drawHangman();									// Draw the hangman graphic

 	while (1) {									// Loop
 		client_len = sizeof(client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) < 0) // Create the listening socket, and if its return value is less than 0
			displayErrMsgStatus("Accepting Connection", 3);			// Display error message, and exit with return status 3

		displayNameAndPort(client, cliName);					// Display the Client IP and Port number

 		playHangmanTCP(fd, fd, cliName, CLI_PORT);				// Play the game

 		close(fd);								// Close the connection to client
 	}
}
