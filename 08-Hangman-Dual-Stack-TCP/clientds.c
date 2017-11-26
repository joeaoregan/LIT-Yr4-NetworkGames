// Jason Foley, Joe O'Regan
// Hangman client connecting with IPv4 and IPv6 addresses
/* Hangclient.c - Client for hangman server.  */

#include <netdb.h>								// gai_strerror()
#include <stdlib.h>								// exit(), rand(), atoi()
#include "../HandleErrors.h"							// Error handling functions header in parent directory
#include "../DrawHangman.h"							// Hangman graphics
#include "../CreateTCPSocket.h"							// Functions for creating TCP sockets

#define DRAW 0									// Turn Graphics on/off: 0 = Don't Draw Hangman Graphics, 1 = Draw Hangman Graphics

int main (int argc, char * argv []) {
	int sock, count, livesLeft;						// Client socket, byte count for data, number of lives/guesses left
	char i_line[LINESIZE];							// Input buffer
	char o_line[LINESIZE];							// Output buffer

	/* 
	   GET THE IP ADDRESS PROTOCOL
	   If the user enters an IP parameter, decide if it is IPv4 or IPv6, otherwise the default value localhost is used
	*/	
	int sockType = createTCPClientDualStack(				// CreateTCPSocket.h: sockType specifies the protocol used to create a socket
				(argc == 1) ? "localhost" : argv[1], 		// Get the address family of the input address, or use the default IPv4 protocol
				TCP_PORT_NUM);					// Specify the port number
	printf("Socket Type IPv%d\n",sockType);

	/*
	   CREATE A SOCKET:
	   If the user entered parameters, use them, otherwise use default values of IPv4 localhost 127.0.0.1 on port 1066, 
	   sockType decides the protocol and defaults to IPv4 if no command line parameters were read.
	*/
	createClientSocketType(&sock, 						// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? "localhost" : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM,		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
			sockType);						// sockType variable is used to decide to create IPv4 or IPv6 socket

	drawHangmanLogo(DS_CLI_TCP);						// Draw the game logo, with socket title parameter

 	/*
	   CONNECTED TO THE SERVER: 
 	   Take a line from the server and show it, take a line and send the user input to the server. 
 	   Repeat until the server terminates the connection.
	*/
 	while ((count = read(sock, i_line, LINESIZE)) > 0) {			// Read input from the server until the server closes the connection so read returns 0
 		//write(1, i_line, count);					// Display the part word to the user using standard input
		livesLeft = parseWordAndLives(i_line, DRAW);			// DrawHangman.h: Parse input from Server & Display lives left & partword, with optional graphics
 		count = read(0, o_line, LINESIZE);				// Get the Player guess from standard input. 0 = STDIN
 		write(sock, o_line, count);					// Send the guess to the Server to be processed
 	}

	gameOverText(livesLeft);						// DrawHangman.h: Select game over message to display, with 3 different messages, win/lose/perfect

	close(sock);								// Close the socket
	return 0;								// End the program
 }
