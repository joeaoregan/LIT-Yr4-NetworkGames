  /* 
 	File: 		serverds.c
	Version: 	Dual Stack TCP Server
	Modified by: 	Jason Foley, Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
	
	Dual stack Server can accept connections from Clients connecting with IPv4 or IPv6 sockets. 
	Using createDualStackServerSocket() a sockaddr_in6 structure is used to store an IPv6 or IPv4 address (formatted as IPv6).
*/


#include <stdio.h>								// printf()
#include <stdlib.h> 								// rand(), exit() atoi(), srand()
#include "../DrawHangman.h"							// Functions to draw the games graphics
#include "../CreateTCPSocket.h"							// Functions to handle socket creation and connection
#include "../TCPPlayHangman.h"							// Functions to play Hangman game using TCP protocol

extern time_t time();

int main (int argc, char * argv []) {						// User can specify port to accept Client connections
 	struct sockaddr_in6 server, client;					// IPv6 address structure to hold Client address details
 	int sock, fd, client_len, cliPort;					// Client & server socket, size of client, Client port number
	char cliName[INET6_ADDRSTRLEN];						// Client address string
	socklen_t len = sizeof client;						// Size of the address stucture

 	srand ((int) time((long *) 0)); 					// Randomize the seed for selecting the word to guess

	sock = createDualStackServerSocket((argc == 2) ? argv[1] : TCP_PORT);	// CreateTCPSocket.h: socket create, bind, listen

	drawHangmanLogo(DS_SRV_TCP);						// Draw the game logo, with socket title parameter

 	while (1) {
 		client_len = sizeof (client);
 		if((fd=accept(sock,(struct sockaddr*)&client,&client_len)) < 0)	// Accept client connections
			displayErrMsgStatus("Accepting Connection", 3);		// accept return an error, display message & exit with status 3

		displayIPAndPort((struct sockaddr_storage* )&client, len, 	// CreateTCPSocket.h: Display the Client IP and Port number
					cliName, &sock, &cliPort);

 		playHangmanTCP(fd, fd, cliName, cliPort);			// TCPPlayHangman.h: Play the game

 		close (fd);							// Close the socket
 	}

	return 0;								// Return type added to main (void main() works fine too)
 }
