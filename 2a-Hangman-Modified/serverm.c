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

//struct sockaddr_in client;	
struct sockaddr_storage client;							// Made global to display client name outside main
//#define CLI_PORT ntohs(client.sin_port)				
//struct sockaddr_in6 client;			
//#define CLI_PORT ntohs(client.sin6_port)
	
int main (int argc, char * argv []) {						// Option to specify port as a command line parameter
 	int sock, fd, client_len;						// Client socket, server socket, size of client
	char cliName[INET6_ADDRSTRLEN];						// Client address string
	socklen_t len = sizeof client;						// Size of the address stucture
	int cliPort;								// Client port number

 	srand((int) time ((long *) 0)); 					// Randomise the seed, so a new word is displayed each game

	//Interchangeable dualstack and IPv4 socket creation
	//sock = createDualStackServerSocket((argc == 2) ? argv[1] : TCP_PORT);	// CreateTCPSocket.h: Create & bind the TCP socket, dual stack IPv4/IPv6
	sock = createTCPServerSocket((argc == 2) ? argv[1] : TCP_PORT);	// Create & bind the TCP socket, before proceeding to accept connections,
										// with an option to enter the port number, or leave blank for default
	drawHangmanNew();							// DrawHangman.h: Draw the hangman graphic in colour

 	while (1) {								// Infinite Loop, as soon as one Client finishes game, another can play
 		client_len = sizeof(client);
 		if((fd=accept(sock,(struct sockaddr*)&client,&client_len))<0)	// Create the listening socket, and if its return value is less than 0
			displayErrMsgStatus("Accepting Connection", 3);		// Display error message, and exit with return status 3

		displayIPAndPort(&client, len, cliName, &sock, &cliPort);	// CreateTCPSocket.h: Display the Client IP and Port number
	
//		inet_ntop(AF_INET, &client.sin_addr, cliName, INET_ADDRSTRLEN);	// XXX OK

 		playHangmanTCP(fd, fd, cliName, cliPort);			// TCPPlayHangman.h: Play the game

 		close(fd);							// Close the client socket
 	}
}

/*
struct sockaddr_storage client;								// Made global to display client name outside main
//#define CLI_PORT ntohs(client.sin_port)	
void displayIPstuff(struct sockaddr_storage* client,int clen,  char* ip, int size,  int* sock, int* port);

int main (int argc, char * argv []) {							// Option to specify port as a command line parameter
 	int sock, fd, client_len;							// Client socket, server socket, size of client
	char cliName[INET6_ADDRSTRLEN];							// Client address string
	int port;									// Port number for Client
	socklen_t len = sizeof client;							// Size of the address stucture

 	srand((int) time ((long *) 0)); 						// Randomise the seed, so a new word is displayed each game

	//Interchangeable dualstack and IPv4 socket creation
	sock = createDualStackServerSocket((argc == 2) ? argv[1] : TCP_PORT);		// CreateTCPSocket.h: Create & bind the TCP socket, dual stack IPv4/IPv6
	//sock = createTCPServerSocket((argc == 2) ? argv[1] : TCP_PORT);		// Create & bind the TCP socket, before proceeding to accept connections,
											// with an option to enter the port number, or leave blank for default
	drawHangmanNew();								// DrawHangman.h: Draw the hangman graphic in colour

 	while (1) {									// Infinite Loop, as soon as one Client finishes game, another can play
 		client_len = sizeof(client);
 		if ((fd = accept(sock, (struct sockaddr *) &client, &client_len)) < 0)  // Create the listening socket, and if its return value is less than 0
			displayErrMsgStatus("Accepting Connection", 3);			// Display error message, and exit with return status 3
		

	//	cliName = displayNameAndPort(&client, cliName, sock, &port);		// CreateTCPSocket.h: Display the Client IP and Port number
		displayNameAndPort(&client, len, cliName, &sock, &port);		// CreateTCPSocket.h: Display the Client IP and Port number
	//	displayIPstuff(&client, len, &(*cliName),INET6_ADDRSTRLEN, &sock, &port);

 		playHangmanTCP(fd, fd, cliName, port);					// TCPPlayHangman.h: Play the game

 		close(fd);								// Close the client socket
 	}
}
*/
/*
	Function takes a sockaddr_storage stucture, and displays the IP and port
	of the remotely connected socket using getpeername()

void displayIPstuff(struct sockaddr_storage* client, int clen, char* cliName, int size, int* sock, int* port){
	getpeername((*sock), (struct sockaddr*) &client, &clen);

	// deal with both IPv4 and IPv6:
	if ((*client).ss_family == AF_INET) {
	    struct sockaddr_in *sock = (struct sockaddr_in *)&client;
	    (*port) = ntohs(sock->sin_port);
	    inet_ntop(AF_INET, &sock->sin_addr, cliName, size);
	} else { // AF_INET6
	    struct sockaddr_in6 *sock = (struct sockaddr_in6 *)&client;
	    (*port) = ntohs((*sock).sin6_port);
	    inet_ntop(AF_INET6, &sock->sin6_addr, cliName, size);
	}

	printf("Handling client %s/%d\n", cliName, (*port));
}
*/
